////////////////////////////////////////////////////////////////////////////
//	Module 		: state_base.cpp
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Base state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_base.h"

CStateBase::CStateBase			(CAI_Stalker *object)
{
	Init				();
	VERIFY				(object);
	m_object			= object;
}

CStateBase::~CStateBase			()
{
}

void CStateBase::Init			()
{
}

void CStateBase::Load			(LPCSTR section)
{
}

void CStateBase::reinit			()
{
	VERIFY				(m_object);
	m_inertia_time		= 0;
}

void CStateBase::reload			(LPCSTR section)
{
}

void CStateBase::initialize		()
{
	m_start_level_time	= Level().timeServer();
	m_start_game_time	= Level().GetGameTime();
}

void CStateBase::execute		()
{
}

void CStateBase::finalize		()
{
	m_inertia_time		= 0;
}
