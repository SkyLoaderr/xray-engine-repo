////////////////////////////////////////////////////////////////////////////
//	Module 		: state_anomaly_check.cpp
//	Created 	: 10.02.2004
//  Modified 	: 10.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Anomaly check state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_anomaly_check.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

using namespace MonsterSpace;

CStateAnomalyCheck::CStateAnomalyCheck	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateAnomalyCheck::~CStateAnomalyCheck	()
{
}

void CStateAnomalyCheck::Init			()
{
}

void CStateAnomalyCheck::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateAnomalyCheck::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateAnomalyCheck::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateAnomalyCheck::initialize		()
{
	inherited::initialize			();
	set_inertia_time				(::Random.randI(5000,10000));
}

void CStateAnomalyCheck::execute			()
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
	m_object->CObjectHandler::set_dest_state(eObjectActionFire1,m_object->inventory().m_slots[5].m_pIItem);
}

void CStateAnomalyCheck::finalize		()
{
	inherited::finalize				();
}
