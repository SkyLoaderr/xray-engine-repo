////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\3dsound.h"
#include "..\\..\\..\\xr_trims.h"

CAI_Soldier::CAI_Soldier()
{
	dwHitTime = 0;
	tHitDir.set(0,0,1);
	dwSenseTime = 0;
	tSenseDir.set(0,0,1);
	tSavedEnemy = 0;
	tSavedEnemyPosition.set(0,0,0);
	tpSavedEnemyNode = 0;
	dwSavedEnemyNodeID = -1;
	dwLostEnemyTime = 0;
	bBuildPathToLostEnemy = false;
	m_dwLastRangeSearch = 0;
	m_dwLastSuccessfullSearch = 0;
	m_fAggressiveness = ::Random.randF(0,1);
	m_fTimorousness = ::Random.randF(0,1);
	m_bFiring = false;
	m_bLessCoverLook = false;
	q_look.o_look_speed = _FB_look_speed;
	m_tpCurrentGlobalAnimation = 
	m_tpCurrentTorsoAnimation = 
	m_tpCurrentLegsAnimation = 0;
	m_tpCurrentGlobalBlend = 
	m_tpCurrentTorsoBlend = 
	m_tpCurrentLegsBlend = 0;
	m_bActionStarted = false;
	m_bJumping = false;
	tpaDynamicObjects.clear();
	// event handlers
	m_tpEventSay = Engine.Event.Handler_Attach("level.entity.say",this);
	m_tpEventAssignPath = Engine.Event.Handler_Attach("level.entity.path.assign",this);
	m_dwPatrolPathIndex = -1;
	m_dwCreatePathAttempts = 0;
	m_fSoundPower = m_fStartPower = 0;
	m_dwSoundUpdate = 0;
}

CAI_Soldier::~CAI_Soldier()
{
	for (int i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
	Engine.Event.Handler_Detach (m_tpEventSay,this);
	Engine.Event.Handler_Detach (m_tpEventAssignPath,this);
}

// when soldier is dead
void CAI_Soldier::Death()
{
	// perform death operations
	inherited::Death( );
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	eCurrentState = aiSoldierDie;
	
	// removing from group
	//Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Member_Remove(this);
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	
	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
}

void CAI_Soldier::vfLoadSelectors(CInifile *ini, const char *section)
{
	SelectorAttack.Load(ini,section);
	SelectorDefend.Load(ini,section);
	SelectorFindEnemy.Load(ini,section);
	SelectorFollowLeader.Load(ini,section);
	SelectorFreeHunting.Load(ini,section);
	SelectorMoreDeadThanAlive.Load(ini,section);
	SelectorNoWeapon.Load(ini,section);
	SelectorPatrol.Load(ini,section);
	SelectorPursuit.Load(ini,section);
	SelectorReload.Load(ini,section);
	SelectorRetreat.Load(ini,section);
	SelectorSenseSomething.Load(ini,section);
	SelectorUnderFire.Load(ini,section);
}

void CAI_Soldier::Load(CInifile* ini, const char* section)
{ 
	// load parameters from ".ini" file
	inherited::Load	(ini,section);
	
	// initialize start position
	Fvector			P = vPosition;
	P.x				+= ::Random.randF();
	P.z				+= ::Random.randF();
	
	vfLoadSounds();
	vfLoadAnimations();
	vfLoadSelectors(ini,section);
	vfAssignBones(ini,section);
	
	// visibility
	m_dwMovementIdleTime = ini->ReadINT(section,"MovementIdleTime");
	m_fMaxInvisibleSpeed = ini->ReadFLOAT(section,"MaxInvisibleSpeed");
	m_fMaxViewableSpeed = ini->ReadFLOAT(section,"MaxViewableSpeed");
	m_fMovementSpeedWeight = ini->ReadFLOAT(section,"MovementSpeedWeight");
	m_fDistanceWeight = ini->ReadFLOAT(section,"DistanceWeight");
	m_fSpeedWeight = ini->ReadFLOAT(section,"SpeedWeight");
	m_fCrouchVisibilityMultiplier = ini->ReadFLOAT(section,"CrouchVisibilityMultiplier");
	m_fLieVisibilityMultiplier = ini->ReadFLOAT(section,"LieVisibilityMultiplier");
	m_fVisibilityThreshold = ini->ReadFLOAT(section,"VisibilityThreshold");
	m_fLateralMutliplier = ini->ReadFLOAT(section,"LateralMultiplier");
	
	//fire
	m_dwFireRandomMin = ini->ReadINT(section,"FireRandomMin");
	m_dwFireRandomMax = ini->ReadINT(section,"FireRandomMax");
	m_dwNoFireTimeMin = ini->ReadINT(section,"NoFireTimeMin");
	m_dwNoFireTimeMax = ini->ReadINT(section,"NoFireTimeMax");
	
	// patrol under fire
	m_dwPatrolShock = ini->ReadINT(section,"PatrolShock");
	m_dwUnderFireShock = ini->ReadINT(section,"UnderFireShock");
	m_dwUnderFireReturn = ini->ReadINT(section,"UnderFireReturn");

	m_dwMaxDynamicObjectsCount = ini->ReadINT(section,"DynamicObjectsCount");
	m_dwMaxDynamicSoundsCount = ini->ReadINT(section,"DynamicSoundsCount");

	//tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[0] = PKinematics(pVisual)->ID_Cycle(ini->ReadSTRING(section,"TestAnimation"));
	//m_fAddAngle = ini->ReadFLOAT(section,"AddAngle");
}

BOOL CAI_Soldier::Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	if (!inherited::Spawn(bLocal,server_id,o_pos,o_angle,P,flags))	return FALSE;
	eCurrentState = aiSoldierFollowLeader;
	return TRUE;
}

// soldier update
void CAI_Soldier::Update(DWORD DT)
{
	inherited::Update(DT);
	vfUpdateSounds(DT);
}

void CAI_Soldier::Exec_Movement	( float dt )
{
	AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
	/**/
	if (eCurrentState != aiSoldierJumping)
		AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
	else {
		UpdateTransform();
		if (m_bActionStarted) {
			m_bActionStarted = false;
			Fvector tAcceleration, tVelocity;
			tVelocity.set(0,1,0);
			Movement.SetPosition(vPosition);
			Movement.SetVelocity(tVelocity);
			tAcceleration.set(0,0,0);
			Movement.SetPosition(vPosition);
			Movement.Calculate	(tAcceleration,0,m_cBodyState == BODY_STATE_STAND ? m_fJumpSpeed : m_fJumpSpeed*.8f,dt > .1f ? .1f : dt,false);
			Movement.GetPosition(vPosition);
		}
		else {
			Fvector tAcceleration;
			tAcceleration.set(0,m_cBodyState == BODY_STATE_STAND ? m_fJumpSpeed : m_fJumpSpeed*.8f,0);
			Movement.SetPosition(vPosition);
			Movement.Calculate	(tAcceleration,0,0,dt > .1f ? .1f : dt,false);
			Movement.GetPosition(vPosition);
		}
		UpdateTransform	();
	}
	/**/
}

void CAI_Soldier::OnEvent(EVENT E, DWORD P1, DWORD P2)
{

	if (E == m_tpEventSay) {
		if (0==P2 || DWORD(this)==P2) {
			char* caTextToShow;
			caTextToShow = (char *)P1;
			Level().HUD()->outMessage(0xffffffff,cName(),"%s",caTextToShow);
		}
	}
	else
		if (E == m_tpEventAssignPath) {
			if (0==P2 || DWORD(this)==P2) {
				char *buf2, *buf, monster_name[100];
				buf2 = buf = (char *)P1;
				int iArgCount = _GetItemCount(buf2);
				if (iArgCount >= 4) {
					DWORD team,squad,group;
					for ( ; ; buf2++)
						if (*buf2 == ',') {
							memcpy(monster_name,buf,(buf2 - buf)*sizeof(char));
							monster_name[buf2++ - buf] = 0;
							break;
						}
						
					if ((strlen(monster_name)) && (strcmp(monster_name,cName())))
						return;
					
					if (!(strlen(monster_name))) {
						sscanf(buf2,"%d,%d,%d",&team,&squad,&group);
						
						if ((team != g_Team()) || (squad != g_Squad()) || (group != g_Group()))
							return;
						
						for (int komas=0; komas<3; buf2++)
							if (*buf2 == ',')
								komas++;
					}
				}
				
				vector<CLevel::SPatrolPath> &tpaPatrolPaths = Level().tpaPatrolPaths;
				for (int i=0; i<tpaPatrolPaths.size(); i++)
					if (!strcmp(buf2,tpaPatrolPaths[i].sName)) {
						m_dwStartPatrolNode = tpaPatrolPaths[i].dwStartNode;
						m_dwPatrolPathIndex = i;
						vfCreatePointSequence(tpaPatrolPaths[i],m_tpaPatrolPoints,m_bLooped);
						m_tpaPointDeviations.resize(m_tpaPatrolPoints.size());
						m_dwLoopCount = 0;
						AI_Path.bNeedRebuild = FALSE;
						return;
					}
				m_tpaPatrolPoints.clear();
			}
		}
}