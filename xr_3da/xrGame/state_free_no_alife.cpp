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
	m_object->set_selection_type	(CMovementManager::eSelectionTypeRandomBranching);
}

void CStateFreeNoAlife::execute			()
{
#ifndef NO_AI
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
	m_object->CSightManager::update				(eLookTypePathDirection);
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#endif
//	m_object->set_level_dest_vertex(dynamic_cast<CAI_ObjectLocation*>(Level().CurrentEntity())->level_vertex_id());
//	m_object->CStalkerMovementManager::update	(
//		0,
//		0,
//		&Level().CurrentEntity()->Position(),
//		0,
//		CMovementManager::ePathTypeLevelPath,
//		CMovementManager::eDetailPathTypeSmooth,
//		eBodyStateStand,
//		eMovementTypeWalk,
//		eMentalStateFree
//	);
//	m_object->CSightManager::update				(eLookTypePoint,&Level().CurrentEntity()->Position());
//	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
}

void CStateFreeNoAlife::finalize		()
{
	inherited::finalize				();
}
