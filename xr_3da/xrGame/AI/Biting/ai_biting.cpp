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
}

void CAI_Biting::Die()
{
	inherited::Die( );

	// ...

}

void CAI_Biting::Load(LPCSTR section)
{
	// load parameters from ".ini" file
	inherited::Load(section);
	

//	g_vfLoadSounds (m_tpSoundTest,pSettings->r_string(section,"sound_test"),100);

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

/*BOOL CAI_Biting::net_Spawn (LPVOID DC) 
{
	//////////////////////////////////////////////////////////////////////////
	
	xrSE_Rat						*tpSE_Rat = (xrSE_Rat *)DC;
	// model
	cNameVisual_set					(tpSE_Rat->caModel);
	if (!inherited::net_Spawn(DC))
		return(FALSE);
	// personal characteristics



	if (!inherited::net_Spawn(DC))
		return(FALSE);
	return true;
}

void CAI_Biting::net_Export(NET_Packet& P) 
{
}

void CAI_Biting::net_Import(NET_Packet& P)
{
}

void CAI_Biting::Update(u32 dt)
{
}

void CAI_Biting::UpdateCL()
{
}

//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////

*/

