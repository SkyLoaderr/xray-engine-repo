////////////////////////////////////////////////////////////////////////////
//	Module 		: state_free_no_alife.cpp
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Free state when no simulation started (for test purposes only!)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_free_no_alife.h"
#include "ai/stalker/ai_stalker.h"

CStateFreeNoAlife::CStateFreeNoAlife	(CAI_Stalker *object) : CStateBase(object)
{
	Init							();
}

CStateFreeNoAlife::~CStateFreeNoAlife	()
{
}

void CStateFreeNoAlife::Init			()
{
}

void CStateFreeNoAlife::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateFreeNoAlife::reinit			()
{
	inherited::reinit				();
}

void CStateFreeNoAlife::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateFreeNoAlife::initialize		()
{
	inherited::initialize			();
	m_object->set_selection_type	(CMovementManager::eSelectionTypeRandomBranching);
}

void CStateFreeNoAlife::execute			()
{
	m_object->play					(eStalkerSoundHumming,60000,10000);
	m_object->vfSetParameters		(0,0,0,MonsterSpace::eObjectActionIdle,CMovementManager::ePathTypeGamePath,CMovementManager::eDetailPathTypeSmooth,eBodyStateStand,eMovementTypeWalk,eMentalStateFree,eLookTypePathDirection);
}

void CStateFreeNoAlife::finalize		()
{
	inherited::finalize				();
}
