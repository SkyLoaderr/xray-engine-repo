//----------------------------------------------------
// file: TempObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ps_instance.h"

CPS_Instance::CPS_Instance			()
{
	m_iLifeTime					= int_max;
	m_bAutoRemove				= TRUE;
}
//----------------------------------------------------

CPS_Instance::~CPS_Instance			()
{
	Render->model_Delete		(renderable.visual);
}
//----------------------------------------------------
void CPS_Instance::shedule_Update	(u32 dt)
{
	m_iLifeTime					-= dt;
}
