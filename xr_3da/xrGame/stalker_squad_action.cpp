////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_squad_action.cpp
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker squad action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "stalker_squad_action.h"
#include "ai/stalker/ai_stalker.h"
#include "agent_manager.h"
#include "script_game_object.h"

CStalkerActionSquad::CStalkerActionSquad	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionSquad::initialize		()
{
	inherited::initialize	();
	VERIFY					(m_object->agent_manager().member(m_object).order_type() == AgentManager::eOrderTypeAction);
	m_object->CSSetupManager::clear();
	m_object->CSSetupManager::add_action(0,xr_new<CSetupAction>(m_object->agent_manager().member(m_object).action()));
}

void CStalkerActionSquad::execute			()
{
	VERIFY					(m_object->agent_manager().member(m_object).order_type() == AgentManager::eOrderTypeAction);
	m_object->CSSetupManager::action(0).update(m_object->agent_manager().member(m_object).action());
}

void CStalkerActionSquad::finalize			()
{
	inherited::finalize		();
	m_object->CSSetupManager::clear();
}
