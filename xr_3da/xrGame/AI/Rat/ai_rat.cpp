////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_fsm.cpp
//	Created 	: 23.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "ai_rat_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\3dsound.h"
#include "..\\..\\..\\xr_trims.h"

CAI_Rat::CAI_Rat()
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
	//m_fSpin = 0.0;
	//m_fMultiplier = sinf(m_fSpin);
	//AI_Path.m_bCollision = false;
	r_spine_speed = r_torso_speed = q_look.o_look_speed = PI_DIV_2*1;
	m_tpCurrentGlobalAnimation = 0;
	m_tpCurrentGlobalBlend = 0;
	m_bActionStarted = false;
	m_bMobility = true;
	// event handlers
	m_tpEventSay = Engine.Event.Handler_Attach("level.entity.say",this);
	m_tpEventAssignPath = Engine.Event.Handler_Attach("level.entity.path.assign",this);
	m_dwPatrolPathIndex = -1;
	m_bFiring = false;
}

CAI_Rat::~CAI_Rat()
{
	for (int i=0; i<SND_HIT_COUNT; i++) pSounds->Delete(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete(sndDie[i]);
	
	Engine.Event.Handler_Detach (m_tpEventSay,this);
	Engine.Event.Handler_Detach (m_tpEventAssignPath,this);
}

void CAI_Rat::Death()
{
	inherited::Death( );
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	eCurrentState = aiRatDie;
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	
	pSounds->PlayAtPos(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
}

void CAI_Rat::vfLoadSelectors(CInifile *ini, const char *section)
{
	SelectorAttack.Load(ini,section);
	SelectorFreeHunting.Load(ini,section);
}

void CAI_Rat::Load(CInifile* ini, const char* section)
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
	//vfAssignBones(ini,section);
	
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

BOOL CAI_Rat::Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	if (!inherited::Spawn(bLocal,server_id,o_pos,o_angle,P,flags))	return FALSE;
	
	INIT_SQUAD_AND_LEADER;

	/**/
	if (Leader == this)
		eCurrentState = aiRatFreeHunting;
	else
		eCurrentState = aiRatFollowLeader;
	
	//eCurrentState = aiRatFreeHunting;
	/**/
	
	return TRUE;
}

// rat update
void CAI_Rat::Update(DWORD DT)
{
	inherited::Update(DT);
}

void CAI_Rat::net_Export(NET_Packet* P)
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

void CAI_Rat::net_Import(NET_Packet* P)
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

void CAI_Rat::Exec_Movement	( float dt )
{
	AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
	if (m_bFiring) {
		if (m_bActionStarted) {
			m_bActionStarted = false;
			if (tSavedEnemy) {
				Fvector tAcceleration, tVelocity;
				tVelocity.sub(tSavedEnemy->Position(),vPosition);
				tVelocity.normalize_safe();
				tVelocity.mul(m_fJumpSpeed);
				Movement.SetVelocity(tVelocity);
				tAcceleration.set(0,0,0);
				Movement.SetPosition(vPosition);
				Movement.Calculate(tAcceleration,0,m_fJumpSpeed,dt > .1f ? .1f : dt,false);
				Movement.GetPosition(vPosition);
			}
		}
		else {
			float fY = ffGetY(*AI_Node,vPosition.x,vPosition.z);
			if (vPosition.y - fY > 0.01f) {
				Fvector tAcceleration;
				tAcceleration.set(0,m_fJumpSpeed,0);
				Movement.SetPosition(vPosition);
				Movement.Calculate	(tAcceleration,0,0,dt > .1f ? .1f : dt,false);
				Movement.GetPosition(vPosition);
			}
			else
				vPosition.set(vPosition.x,fY,vPosition.z);
		}
		UpdateTransform();
	}
}

void CAI_Rat::OnEvent(EVENT E, DWORD P1, DWORD P2)
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