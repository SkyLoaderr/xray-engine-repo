//----------------------------------------------------
// file: TempObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ps_instance.h"
#include "IGame_Level.h"

CPS_Instance::CPS_Instance			()
{
	g_pGameLevel->ps_active.insert			(this);

	m_iLifeTime								= int_max;
	m_bAutoRemove							= TRUE;
}

//----------------------------------------------------
CPS_Instance::~CPS_Instance			()
{
	xr_set<CPS_Instance*>::iterator it		= g_pGameLevel->ps_active.find(this);
	VERIFY									(it!=g_pGameLevel->ps_active.end());
	g_pGameLevel->ps_active.erase			(it);

	spatial_unregister						();
	shedule_unregister						();
}
//----------------------------------------------------
void CPS_Instance::shedule_Update	(u32 dt)
{
	ISheduled::shedule_Update		(dt);
	m_iLifeTime						-= dt;

	// remove???
	if (m_bAutoRemove && m_iLifeTime<=0)
		PSI_destroy					();
}
//----------------------------------------------------
void CPS_Instance::PSI_destroy		()
{
	m_iLifeTime						= 0;
	g_pGameLevel->ps_destroy.push_back	(this);
}
//----------------------------------------------------
