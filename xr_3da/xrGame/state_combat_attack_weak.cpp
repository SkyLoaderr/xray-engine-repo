////////////////////////////////////////////////////////////////////////////
//	Module 		: state_combat_attack_weak.cpp
//	Created 	: 24.01.2004
//  Modified 	: 24.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Combat state attacking weak enemy
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_combat_attack_weak.h"
#include "ai/stalker/ai_stalker.h"

CStateAttackWeak::CStateAttackWeak	()
{
	Init							();
}

CStateAttackWeak::~CStateAttackWeak	()
{
}

void CStateAttackWeak::Init			()
{
}

void CStateAttackWeak::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateAttackWeak::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateAttackWeak::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateAttackWeak::initialize		()
{
	inherited::initialize			();
	m_object->set_selection_type	(CMovementManager::eSelectionTypeRandomBranching);
}

void CStateAttackWeak::execute			()
{
//	m_object->play					(eStalkerSoundHumming,60000,10000);
//	m_object->vfSetParameters		(0,0,0,MonsterSpace::eObjectActionStrap,CMovementManager::ePathTypeGamePath,CMovementManager::eDetailPathTypeSmooth,eBodyStateStand,eMovementTypeWalk,eMentalStateFree,eLookTypePathDirection);

//	m_object->CStalkerMovementManager::update	(0,0,0,
//		CMovementManager::ePathTypeGamePath,
//		CMovementManager::eDetailPathTypeSmooth,
//		eBodyStateStand,
//		eMovementTypeStand,
//		eMentalStateDanger
	//	);
	//	m_object->CSightManager::update				(eLookTypeCurrentDirection);
	m_object->CObjectHandler::set_dest_state	(eObjectActionFire1,m_object->best_weapon());

	m_object->CStalkerMovementManager::update	(0,0,0,0,
		CMovementManager::ePathTypeGamePath,
		CMovementManager::eDetailPathTypeSmooth,
		eBodyStateStand,
		eMovementTypeWalk,
		eMentalStateDanger
		);
//	m_object->CSightManager::update				(eLookTypeCurrentDirection);
	m_object->CSightManager::update				(eLookTypePathDirection);
//	m_object->CObjectHandler::set_dest_state	(eObjectActionIdle);
	m_object->play								(eStalkerSoundHumming,60000,10000);
}

void CStateAttackWeak::finalize		()
{
	inherited::finalize				();
}
