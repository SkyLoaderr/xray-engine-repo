////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_chimera.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all monsters of class "Chimera"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_chimera.h"

using namespace AI_Chimera;

CAI_Chimera::CAI_Chimera()
{
	Init();
	Movement.AllocateCharacterObject(CPHMovementControl::CharacterType::ai_stalker);

	stateRest			= xr_new<CChimeraRest>		(this);
	stateAttack			= xr_new<CChimeraAttack>	(this);
	stateEat			= xr_new<CChimeraEat>		(this);
	stateHide			= xr_new<CChimeraHide>		(this);
	stateDetour			= xr_new<CChimeraDetour>	(this);
	statePanic			= xr_new<CChimeraPanic>	(this);
	CurrentState		= stateRest;
}

CAI_Chimera::~CAI_Chimera()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
}

void CAI_Chimera::Init()
{
	// initializing class members
	m_tpCurrentGlobalAnimation		= 0;
	m_tCurGP						= _GRAPH_ID(-1);
	m_tNextGP						= _GRAPH_ID(-1);
	m_dwTimeToChange				= 0;
	
	m_dwLastRangeSearch				= 0;
	m_fGoingSpeed					= 0.f;

	// ������������� ���������� ��������
	m_tAnim							= DEFAULT_ANIM;

	m_tPathType						= ePathTypeStraight;

	m_fAttackSuccessProbability0	= .8f;
	m_fAttackSuccessProbability1	= .6f;
	m_fAttackSuccessProbability2	= .4f;
	m_fAttackSuccessProbability3	= .2f;

	m_dwPointCheckLastTime			= 0;
	m_dwPointCheckInterval			= 1500;
	
	m_dwActionIndex					= 0;

	InitMemory(5000,10000);
	Motion.Init();

}

void CAI_Chimera::Die()
{
	inherited::Die( );

	DeinitMemory();

	Fvector	dir;
	AI_Path.Direction(dir);
	
	bShowDeath = true;
	SelectAnimation(XFORM().k,dir,AI_Path.fSpeed);
}

void CAI_Chimera::Load(LPCSTR section)
{
	// load parameters from ".ltx" file
	inherited::Load		(section);
	
	// ������ ���� ����� ����� � �����������
	m_tpaTerrain.clear				();
	LPCSTR							S = pSettings->r_string(section,"terrain");
	u32								N = _GetItemCount(S);
	R_ASSERT						(((N % (LOCATION_TYPE_COUNT + 2)) == 0) && (N));
	STerrainPlace					tTerrainPlace;
	tTerrainPlace.tMask.resize		(LOCATION_TYPE_COUNT);
	m_tpaTerrain.reserve			(32);
	string16						I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; j++, i++)
			tTerrainPlace.tMask[j]	= _LOCATION_ID(atoi(_GetItem(S,i,I)));
		tTerrainPlace.dwMinTime		= atoi(_GetItem(S,i++,I))*1000;
		tTerrainPlace.dwMaxTime		= atoi(_GetItem(S,i++,I))*1000;
		m_tpaTerrain.push_back		(tTerrainPlace);
	}
	
	m_fGoingSpeed					= pSettings->r_float	(section, "going_speed");

	m_tSelectorFreeHunting.Load		(section,"selector_free_hunting");
	m_tSelectorRetreat.Load			(section,"selector_retreat");
	m_tSelectorCover.Load			(section,"selector_cover");

	// loading frustum parameters
	eye_fov							= pSettings->r_float(section,"EyeFov");
	eye_range						= pSettings->r_float(section,"EyeRange");
	m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	m_bCannibalism					= pSettings->r_bool  (section,"Cannibalism");
	m_bEatMemberCorpses				= pSettings->r_bool  (section,"EatMemberCorpses");
	m_dwEatCorpseInterval			= pSettings->r_s32   (section,"EatCorpseInterval");

	m_dwHealth						= pSettings->r_u32   (section,"Health");
	m_fHitPower						= pSettings->r_float (section,"HitPower");
	// temp
	//m_fHitPower						= 1.f;
	fHealth							= (float)m_dwHealth;

}

BOOL CAI_Chimera::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterBiting						*l_tpSE_Biting	= dynamic_cast<CSE_ALifeMonsterBiting*>(e);
	
	cNameVisual_set					(l_tpSE_Biting->get_visual());
	
	m_tNextGP						= m_tCurGP = getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
	
	// loading animations
	CChimeraAnimations::Load			(PKinematics(Visual()));

#ifndef NO_PHYSICS_IN_AI_MOVE
	Movement.CreateCharacter();
	Movement.SetPhysicsRefObject(this);
#endif
	Movement.SetPosition	(Position());
	Movement.SetVelocity	(0,0,0);

	return(TRUE);
}

void CAI_Chimera::net_Destroy()
{
	inherited::net_Destroy();
	Init();
	Movement.DestroyCharacter();
}

void CAI_Chimera::net_Export(NET_Packet& P) 
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_float_q16			(fHealth,-1000,1000);
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);

	P.w						(&m_tNextGP,				sizeof(m_tNextGP));
	P.w						(&m_tCurGP,					sizeof(m_tCurGP));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	float					f1;
	f1						= Position().distance_to		(getAI().m_tpaGraph[m_tCurGP].tLocalPoint);
	P.w						(&f1,						sizeof(f1));
	f1						= Position().distance_to		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
	P.w						(&f1,						sizeof(f1));
}

void CAI_Chimera::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;
	P.r_float_q16			(fHealth,-1000,1000);
	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);

	P.r						(&m_tNextGP,				sizeof(m_tNextGP));
	P.r						(&m_tCurGP,					sizeof(m_tCurGP));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible				(TRUE);
	setEnabled				(TRUE);
}

void CAI_Chimera::Exec_Movement		(float dt)
{
	AI_Path.Calculate				(this,Position(),Position(),m_fCurSpeed,dt);
}


//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////

void CAI_Chimera::UpdateCL()
{
	inherited::UpdateCL();

}
