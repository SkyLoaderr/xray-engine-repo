////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie.cpp
//	Created 	: 07.05.2002
//  Modified 	: 07.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "ai_zombie_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\3dsound.h"
#include "..\\..\\..\\xr_trims.h"

CAI_Zombie::CAI_Zombie()
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
	m_bLessCoverLook = false;
	//////////////////////////////////////////////////////////////////////////
	// r_spine_speed = r_torso_speed = q_look.o_look_speed = _FB_look_speed;
	// Actually I don't understand why?? it is a feature of the vc.net
	//////////////////////////////////////////////////////////////////////////
	r_spine_speed = r_torso_speed = q_look.o_look_speed = PI;//_FB_look_speed;
	m_tpCurrentGlobalAnimation = 0;
	m_tpCurrentGlobalBlend = 0;
	m_bActionStarted = false;
	m_bJumping = false;
	m_dwPatrolPathIndex = -1;
	m_dwCreatePathAttempts = 0;
	m_tpEventSay = Engine.Event.Handler_Attach("level.entity.say",this);
	m_tpEventAssignPath = Engine.Event.Handler_Attach("level.entity.path.assign",this);
}

CAI_Zombie::~CAI_Zombie()
{
	for (int i=0; i<SND_HIT_COUNT; i++) pSounds->Delete(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete(sndDie[i]);
	
	Engine.Event.Handler_Detach (m_tpEventSay,this);
	Engine.Event.Handler_Detach (m_tpEventAssignPath,this);
}

// when zombie is dead
void CAI_Zombie::Death()
{
	inherited::Death( );
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	eCurrentState = aiZombieDie;
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
}

void CAI_Zombie::vfLoadSelectors(CInifile *ini, const char *section)
{
	SelectorFreeHunting.Load(ini,section);
}

void CAI_Zombie::Load(CInifile* ini, const char* section)
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
	m_fHitPower       = ini->ReadINT(section,"HitPower");
	m_dwHitInterval   = ini->ReadINT(section,"HitInterval");
}

BOOL CAI_Zombie::Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	if (!inherited::Spawn(bLocal,server_id,o_pos,o_angle,P,flags))	return FALSE;
	eCurrentState = aiZombieFreeHunting;
	return TRUE;
}

// zombie update
void CAI_Zombie::Update(DWORD DT)
{
	inherited::Update(DT);
}

void CAI_Zombie::net_Export(NET_Packet* P)
{
	R_ASSERT				(net_Local);

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P->w_u32				(N.dwTimeStamp);
	P->w_u8					(0);
	P->w_vec3				(N.p_pos);
	P->w_angle8				(N.o_model);
	P->w_angle8				(N.o_torso.yaw);
	P->w_angle8				(N.o_torso.pitch);
}

void CAI_Zombie::net_Import(NET_Packet* P)
{
	R_ASSERT				(!net_Local);
	net_update				N;

	u8 flags;
	P->r_u32				(N.dwTimeStamp);
	P->r_u8					(flags);
	P->r_vec3				(N.p_pos);
	P->r_angle8				(N.o_model);
	P->r_angle8				(N.o_torso.yaw);
	P->r_angle8				(N.o_torso.pitch);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	bVisible				= TRUE;
	bEnabled				= TRUE;
}

void CAI_Zombie::Exec_Movement	( float dt )
{
	if (eCurrentState != aiZombieJumping)
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
			tAcceleration.set(0,-m_cBodyState == BODY_STATE_STAND ? m_fJumpSpeed : m_fJumpSpeed*.8f,0);
			Movement.SetPosition(vPosition);
			Movement.Calculate	(tAcceleration,0,0,dt > .1f ? .1f : dt,false);
			Movement.GetPosition(vPosition);
		}
		UpdateTransform	();
	}
}

void CAI_Zombie::OnEvent(EVENT E, DWORD P1, DWORD P2)
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
