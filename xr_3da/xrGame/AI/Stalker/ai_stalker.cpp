////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

CAI_Stalker::CAI_Stalker()
{
	m_tStateList.clear();
	while (m_tStateStack.size())
		m_tStateStack.pop();
}

CAI_Stalker::~CAI_Stalker()
{
	Msg("FSM report for %s :",cName());
	for (int i=0; i<(int)m_tStateStack.size(); i++)
		Msg("%3d %6d",m_tStateList[i].eState,m_tStateList[i].dwTime);
	Msg("Total updates : %d",m_dwUpdateCount);
	xr_delete(Weapons);
}

// when soldier is dead
void CAI_Stalker::Die()
{
	vfAddStateToList(m_eCurrentState = eStalkerStateDie);

	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
}

void CAI_Stalker::OnDeviceCreate()
{ 
	inherited::OnDeviceCreate();
	CStalkerAnimations::Load(PKinematics(pVisual));
	Weapons->Init		(pSettings->ReadSTRING(cNameSect(),"bone_torso_weapon"),pSettings->ReadSTRING(cNameSect(),"bone_head_weapon"));
}

void CAI_Stalker::Load	(LPCSTR section)
{ 
	inherited::Load		(section);
	Weapons				= xr_new<CWeaponList> (this);
	m_tSelectorFreeHunting.Load(section);
}

BOOL CAI_Stalker::net_Spawn	(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);
	xrSE_Human		*tpHuman = (xrSE_Human*)(DC);
	R_ASSERT		(tpHuman);
	cNameVisual_set	(tpHuman->caModel);
	fHealth			= tpHuman->fHealth;

	m_tStateStack.push	(m_eCurrentState = eStalkerStateLookingOver);
	vfAddStateToList	(m_eCurrentState);
	return				(TRUE);
}

void CAI_Stalker::net_Export(NET_Packet& P)
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
	P.w_float				(N.fHealth);
}

void CAI_Stalker::net_Import(NET_Packet& P)
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
	P.r_float				(N.fHealth);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible				(TRUE);
	setEnabled				(TRUE);
}