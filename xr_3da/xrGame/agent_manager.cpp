////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_manager.h"
#include "ai/stalker/ai_stalker.h"

CAgentManager::CAgentManager		()
{
	shedule.t_min				= pSettings->r_s32	("squad_manager","schedule_min");
	shedule.t_max				= pSettings->r_s32	("squad_manager","schedule_max");
	shedule_register			();
}

CAgentManager::~CAgentManager		()
{
	VERIFY						(m_members.empty());
	shedule_unregister			();
}

float CAgentManager::shedule_Scale	()
{
	return						(.5f);
}

void CAgentManager::shedule_Update	(u32 time_delta)
{
	inherited::shedule_Update	(time_delta);
	update						(time_delta);
}

BOOL CAgentManager::shedule_Ready	()
{
	return						(!m_members.empty());
}

void CAgentManager::add				(CEntity *member)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(member);
	if (!stalker)
		return;
	iterator					I = std::find(m_members.begin(),m_members.end(), stalker);
	VERIFY						(I == m_members.end());
	m_members.push_back			(stalker);
}

void CAgentManager::remove			(CEntity *member)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(member);
	if (!stalker)
		return;
	iterator					I = std::find(m_members.begin(),m_members.end(), stalker);
	VERIFY						(I != m_members.end());
	m_members.erase				(I);
}

void CAgentManager::update			(u32 time_delta)
{
}
