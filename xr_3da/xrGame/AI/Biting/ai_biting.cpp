////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"

CAI_Biting::CAI_Biting()
{
	Init();
}

CAI_Biting::~CAI_Biting()
{
}

void CAI_Biting::Init()
{
	// initializing class members
	m_tpCurrentGlobalAnimation	= 0;
	m_tCurGP					= _GRAPH_ID(-1);
	m_tNextGP					= _GRAPH_ID(-1);
	m_fGoingSpeed				= 0.f;
}

void CAI_Biting::Die()
{
	inherited::Die( );
}

void CAI_Biting::Load(LPCSTR section)
{
	// load parameters from ".ltx" file
	inherited::Load		(section);
	
	// loading sounds
//	g_vfLoadSounds		(m_tpSoundDie,pSettings->r_string(section,"sound_death"),100);
//	g_vfLoadSounds		(m_tpSoundHit,pSettings->r_string(section,"sound_hit"),100);

	// Load params from section

	// personal properties
/*	m_dwEyeFOV				= pSettings->r_u32   (section,"EyeFov");
	m_dwEyeRange			= pSettings->r_u32   (section,"EyeRange");
	m_dwHealth				= pSettings->r_u32   (section,"Health");
	m_fMinSpeed				= pSettings->r_float   (section,"MinSpeed");
	m_fMaxSpeed				= pSettings->r_float   (section,"MaxSpeed");
	m_fAttackSpeed			= pSettings->r_float   (section,"AttackSpeed");
	m_dwMaxPersuitRadius	= pSettings->r_u32   (section,"MaxPersuitRadius");	
	m_dwMaxHomeRadius		= pSettings->r_u32   (section,"MaxHomeRadius");	

	// morale
	m_dwMoraleSuccessAttackQuant	= pSettings->r_s32   (section,"MoraleSuccessAttackQuant");
	m_dwMoraleDeathQuant			= pSettings->r_s32   (section,"MoraleDeathQuant");
	m_dwMoraleFearQuant				= pSettings->r_s32   (section,"MoraleFearQuant");
	m_dwMoraleRestoreQuant			= pSettings->r_s32   (section,"MoraleRestoreQuant");
	m_dwMoraleRestoreTimeInterval	= pSettings->r_u32   (section,"MoraleRestoreTimeInterval");
	m_dwMoraleMinValue				= pSettings->r_u32   (section,"MoraleMinValue");
	m_dwMoraleMaxValue				= pSettings->r_u32   (section,"MoraleMaxValue");
	m_dwMoraleNormalValue			= pSettings->r_u32   (section,"MoraleNormalValue");

	// attack
	m_fHitPower						= pSettings->r_float   (section,"HitPower");	
	m_dwHitInterval					= pSettings->r_u32   (section,"HitInterval");	
	m_fAttackDistance				= pSettings->r_float   (section,"AttackDistance");	
	m_dwAttackAngle					= pSettings->r_s32   (section,"AttackAngle");	
	m_fAttackSuccessProbability		= pSettings->r_float   (section,"AttackSuccessProbability");	

	m_dwActiveScheduleMin			= shedule_Min;
	m_dwActiveScheduleMax			= shedule_Max;

	*/
	// Prepare terrain
	// ...
}

BOOL CAI_Biting::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	xrSE_Biting						*l_tpSE_Biting = (xrSE_Biting*)DC;
	
	cNameVisual_set					(l_tpSE_Biting->caModel);
	
	// loading animations
	CBitingAnimations::Load			(PKinematics(pVisual));
	
	return(TRUE);
}

void CAI_Biting::net_Destroy()
{
	Init();
}

void CAI_Biting::net_Export(NET_Packet& P) 
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
	f1						= vPosition.distance_to		(getAI().m_tpaGraph[m_tCurGP].tLocalPoint);
	P.w						(&f1,						sizeof(f1));
	f1						= vPosition.distance_to		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
	P.w						(&f1,						sizeof(f1));
}

void CAI_Biting::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;
	P.r_float_q16		(fHealth,-1000,1000);
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


//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////

void CAI_Biting::HitSignal(float amount, Fvector &vLocalDir, CObject *who, s16 element)
{
}