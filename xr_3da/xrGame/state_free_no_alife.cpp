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

using namespace MonsterSpace;

CStateFreeNoAlife::CStateFreeNoAlife	()
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

void CStateFreeNoAlife::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
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
//	m_object->play					(eStalkerSoundHumming,60000,10000);
//	m_object->vfSetParameters		(0,0,0,MonsterSpace::eObjectActionStrap,CMovementManager::ePathTypeGamePath,CMovementManager::eDetailPathTypeSmooth,eBodyStateStand,eMovementTypeWalk,eMentalStateFree,eLookTypePathDirection);
	m_object->CStalkerMovementManager::update	(0,0,0,
		CMovementManager::ePathTypeGamePath,
		CMovementManager::eDetailPathTypeSmooth,
		eBodyStateStand,
		eMovementTypeStand,
//		eMentalStateFree
		eMentalStateDanger
	);
	m_object->CSightManager::update				(eLookTypeCurrentDirection);
	m_object->CObjectHandler::set_dest_state	(eObjectActionFire1,m_object->get_best_weapon());
}

void CStateFreeNoAlife::finalize		()
{
	inherited::finalize				();
}
