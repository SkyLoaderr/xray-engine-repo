////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious functions for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "..\\..\\hudmanager.h"

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
	//eCurrentState = aiSoldierFollowLeader;
	eCurrentState = aiSoldierTestMicroActions;
	m_dwLastRangeSearch = 0;
	m_dwLastSuccessfullSearch = 0;
	m_fAggressiveness = ::Random.randF(0,1);
	m_fTimorousness = ::Random.randF(0,1);
	m_bFiring = false;
	m_tpEventSay = Engine.Event.Handler_Attach	("level.entity.say",this);
	m_bLessCoverLook = false;
	q_look.o_look_speed = _FB_look_speed;
	m_tpCurrentGlobalAnimation = 
	m_tpCurrentTorsoAnimation = 
	m_tpCurrentHandsAnimation = 
	m_tpCurrentLegsAnimation = 0;
	m_tpCurrentGlobalBlend = 
	m_tpCurrentTorsoBlend = 
	m_tpCurrentHandsBlend = 
	m_tpCurrentLegsBlend = 0;
	m_bActionStarted = false;
}

CAI_Soldier::~CAI_Soldier()
{
	for (int i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
	Engine.Event.Handler_Detach (m_tpEventSay,this);
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
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
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
	
	//fire
	m_dwFireRandomMin = ini->ReadINT(section,"FireRandomMin");
	m_dwFireRandomMax = ini->ReadINT(section,"FireRandomMax");
	m_dwNoFireTimeMin = ini->ReadINT(section,"NoFireTimeMin");
	m_dwNoFireTimeMax = ini->ReadINT(section,"NoFireTimeMax");
	
	// patrol under fire
	m_dwPatrolShock = ini->ReadINT(section,"PatrolShock");
	m_dwUnderFireShock = ini->ReadINT(section,"UnderFireShock");
	m_dwUnderFireReturn = ini->ReadINT(section,"UnderFireReturn");
}

BOOL CAI_Soldier::Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	if (!inherited::Spawn(bLocal,server_id,o_pos,o_angle,P,flags))	return FALSE;
	vfCheckForPatrol();
	return TRUE;
}

// soldier update
void CAI_Soldier::Update(DWORD DT)
{
	inherited::Update(DT);
}

void CAI_Soldier::OnEvent(EVENT E, DWORD P1, DWORD P2)
{
	if (E == m_tpEventSay) {
		if (0==P2 || DWORD(this)==P2) {
			char* Test;
			Test = (char *)P1;
			Level().HUD()->outMessage(0xffffffff,cName(),"%s",Test);
		}
	}
}

void CAI_Soldier::Exec_Movement	( float dt )
{
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
			Movement.Calculate	(tAcceleration,0,1,dt,false);
			Movement.GetPosition(vPosition);
		}
		else {
			Fvector tAcceleration;
			tAcceleration.set(0,5,0);
			Movement.SetPosition(vPosition);
			Movement.Calculate	(tAcceleration,0,0,dt,false);
			Movement.GetPosition(vPosition);
		}
		UpdateTransform	();
	}
}