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
	dwSavedEnemyNodeID = DWORD(-1);
	dwLostEnemyTime = 0;
	r_spine_speed = r_torso_speed = q_look.o_look_speed = PI_DIV_2*2;
	m_tpCurrentGlobalAnimation = 0;
	m_tpCurrentGlobalBlend = 0;
	m_bActionStarted = false;
	m_bMobility = true;
	m_tpEventSay = Engine.Event.Handler_Attach("level.entity.say",this);
	m_tpEventAssignPath = Engine.Event.Handler_Attach("level.entity.path.assign",this);
	m_bFiring = false;
	tpaDynamicSounds.clear();
	m_fSensetivity = 0.f;
	m_iSoundIndex = -1;
	tpaDynamicSounds.clear();
	m_dwLastUpdate = 0;
	m_dwLastVoiceTalk = 0;
	m_tpSoundBeingPlayed = 0;
	m_dwLastSoundRefresh = 0;
	m_dwLastRangeSearch  = 0;
	m_tpPath = 0;
	// test parameters
	m_tGoalDir.set			(10.0f*(Random.randF()-Random.randF()),10.0f*(Random.randF()-Random.randF()),10.0f*(Random.randF()-Random.randF()));
	m_tCurrentDir.set		(0,0,1);
	m_tHPB.set				(0,0,0);
	m_fDHeading				= 0;
	m_fGoalChangeDelta		= 10.f;
	m_fGoalChangeTime		= 0.f;
	m_fASpeed				= .2f;
	m_fMinHeight			= 0.f;
	m_tVarGoal.set			(10.0,0.0,20.0);
	m_dwStandLookTime		= 5000;
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
	eCurrentState = aiRatDie;
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	
	pSounds->PlayAtPos(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
}

void CAI_Rat::vfLoadSelectors(LPCSTR section)
{
	SelectorAttack.Load(section);
	SelectorFreeHunting.Load(section);
}

void CAI_Rat::Load(LPCSTR section)
{ 
	// load parameters from ".ini" file
	inherited::Load	(section);
	
	// initialize start position
	Fvector			P = vPosition;
	P.x				+= ::Random.randF();
	P.z				+= ::Random.randF();
	
	vfLoadSounds();
	vfLoadAnimations();
	vfLoadSelectors(section);
	
	// visibility
	m_dwMovementIdleTime = pSettings->ReadINT(section,"MovementIdleTime");
	m_fMaxInvisibleSpeed = pSettings->ReadFLOAT(section,"MaxInvisibleSpeed");
	m_fMaxViewableSpeed = pSettings->ReadFLOAT(section,"MaxViewableSpeed");
	m_fMovementSpeedWeight = pSettings->ReadFLOAT(section,"MovementSpeedWeight");
	m_fDistanceWeight = pSettings->ReadFLOAT(section,"DistanceWeight");
	m_fSpeedWeight = pSettings->ReadFLOAT(section,"SpeedWeight");
	m_fCrouchVisibilityMultiplier = pSettings->ReadFLOAT(section,"CrouchVisibilityMultiplier");
	m_fLieVisibilityMultiplier = pSettings->ReadFLOAT(section,"LieVisibilityMultiplier");
	m_fVisibilityThreshold = pSettings->ReadFLOAT(section,"VisibilityThreshold");
	m_fLateralMultiplier = pSettings->ReadFLOAT(section,"LateralMultiplier");
	m_fShadowWeight = pSettings->ReadFLOAT(section,"ShadowWeight");
	
	m_dwMaxDynamicSoundsCount = pSettings->ReadINT(section,"DynamicSoundsCount");

	m_fMinVoiceIinterval = pSettings->ReadFLOAT(section,"MinVoiceIinterval");
	m_fMaxVoiceIinterval = pSettings->ReadFLOAT(section,"MaxVoiceIinterval");
	m_fVoiceRefreshRate	 = pSettings->ReadFLOAT(section,"VoiceRefreshRate");
	//fire
	m_fHitPower			= (float)pSettings->ReadINT(section,"HitPower");
	m_dwHitInterval		= pSettings->ReadINT(section,"HitInterval");

	m_fSpeed			= m_fMaxSpeed;
}

BOOL CAI_Rat::net_Spawn	(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))	return FALSE;
	
	m_tOldPosition.set(vPosition);
	m_tSpawnPosition.set(vPosition);
	m_tSafeSpawnPosition.set(m_tSpawnPosition);
	INIT_SQUAD_AND_LEADER;

	tStateStack.push(eCurrentState = aiRatFreeHunting);
	
	return TRUE;
}

void CAI_Rat::Update(DWORD DT)
{
	inherited::Update(DT);
}

void CAI_Rat::net_Export(NET_Packet& P)
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);
}

void CAI_Rat::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;
	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible				(TRUE);
	setEnabled				(TRUE);
}

void CAI_Rat::Exec_Movement	( float dt )
{
	if (fabsf(vPosition.x) > 10000.f) {
		if (ps_Size() > 1)
			vPosition = ps_Element(ps_Size() - 2).vPosition;
		Msg("%s",cName());
	}
	AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
	if (m_bFiring) {
		if (m_bActionStarted) {
			m_bActionStarted = false;
			if (tSavedEnemy) {
				m_fJumpSpeed = 7.f;
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
						
						if (((int)team != g_Team()) || ((int)squad != g_Squad()) || ((int)group != g_Group()))
							return;
						
						for (int komas=0; komas<3; buf2++)
							if (*buf2 == ',')
								komas++;
					}
				}
				
				m_tpPath = &(Level().m_PatrolPaths[buf2]);
			}
		}
}