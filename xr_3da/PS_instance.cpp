//----------------------------------------------------
// file: TempObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ps_instance.h"
#include "IGame_Level.h"

CPS_Instance::CPS_Instance			()
{
	g_pGameLevel->ps_active.push_back	(this);

	m_iLifeTime							= int_max;
	m_bAutoRemove						= TRUE;
}
//----------------------------------------------------
CPS_Instance::~CPS_Instance			()
{
	spatial_unregister					();
	shedule_unregister					();
}
//----------------------------------------------------
void CPS_Instance::shedule_Update	(u32 dt)
{
	m_iLifeTime					-= dt;

	// remove???
	if (m_bAutoRemove && m_iLifeTime<=0)
		g_pGameLevel->ps_destoy.push_back	(this);
}
