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
#include "inventory.h"
#include "inventory_item.h"

using namespace MonsterSpace;

CStateFreeNoAlife::CStateFreeNoAlife	(LPCSTR state_name) : inherited(state_name)
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
	set_inertia_time				(::Random.randI(60,120)*1000);
	m_object->set_sound_mask		(eStalkerSoundMaskAnySound);
	m_object->set_selection_type	(CMovementManager::eSelectionTypeRandomBranching);
}

void CStateFreeNoAlife::execute			()
{
	m_object->play								(eStalkerSoundHumming,60000,10000);
	m_object->CStalkerMovementManager::update	(
		0,
		0,
		0,
		0,
		CMovementManager::ePathTypeGamePath,
		CMovementManager::eDetailPathTypeSmooth,
		eBodyStateStand,
		eMovementTypeWalk,
		eMentalStateFree
	);
	m_object->CSightManager::update				(eLookTypeSearch);
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
}

void CStateFreeNoAlife::finalize		()
{
	inherited::finalize				();
}
