////////////////////////////////////////////////////////////////////////////
//	Module 		: state_hidden_enemy_check.cpp
//	Created 	: 10.02.2004
//  Modified 	: 10.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Hidden enemy check state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_hidden_enemy_check.h"
#include "ai/stalker/ai_stalker.h"

using namespace MonsterSpace;

CStateHiddenEnemyCheck::CStateHiddenEnemyCheck	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateHiddenEnemyCheck::~CStateHiddenEnemyCheck	()
{
}

void CStateHiddenEnemyCheck::Init			()
{
}

void CStateHiddenEnemyCheck::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateHiddenEnemyCheck::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateHiddenEnemyCheck::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateHiddenEnemyCheck::initialize		()
{
	inherited::initialize			();
	set_inertia_time				(::Random.randI(10000,30000));
}

void CStateHiddenEnemyCheck::execute			()
{
	m_object->CStalkerMovementManager::update	(
		0,
		0,
		0,
		0,
		CMovementManager::ePathTypeNoPath,
		CMovementManager::eDetailPathTypeSmooth,
		eBodyStateStand,
		eMovementTypeStand,
		eMentalStateDanger
	);
}

void CStateHiddenEnemyCheck::finalize		()
{
	inherited::finalize				();
}
