////////////////////////////////////////////////////////////////////////////
//	Module 		: state_watch_over.cpp
//	Created 	: 10.02.2004
//  Modified 	: 10.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Watch over state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_watch_over.h"
#include "ai/stalker/ai_stalker.h"

using namespace MonsterSpace;

CStateWatchOver::CStateWatchOver	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateWatchOver::~CStateWatchOver	()
{
}

void CStateWatchOver::Init			()
{
}

void CStateWatchOver::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateWatchOver::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateWatchOver::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateWatchOver::initialize		()
{
	inherited::initialize			();
	m_object->set_sound_mask		(eStalkerSoundMaskHumming);
	set_inertia_time				(::Random.randI(5000,10000));
}

void CStateWatchOver::execute			()
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

void CStateWatchOver::finalize		()
{
	inherited::finalize				();
}
