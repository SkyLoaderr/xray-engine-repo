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
#include "agent_manager_motivation_planner.h"

#define SECTION				"agent_manager"
//#define IMPORTANT_BUILD

CAgentManager::CAgentManager		()
{
	shedule.t_min				= pSettings->r_s32	(SECTION,"schedule_min");
	shedule.t_max				= pSettings->r_s32	(SECTION,"schedule_max");
	shedule_register			();
	m_brain						= xr_new<CAgentManagerMotivationPlanner>();
	brain().setup				(this);
}

CAgentManager::~CAgentManager		()
{
	shedule_unregister			();
	xr_delete					(m_brain);
	VERIFY						(m_members.empty());
}

float CAgentManager::shedule_Scale	()
{
	return						(.5f);
}

void CAgentManager::shedule_Update	(u32 time_delta)
{
	ISheduled::shedule_Update	(time_delta);
	reset_memory_masks			();
	remove_old_danger_covers	();
	brain().update				();
}

BOOL CAgentManager::shedule_Ready	()
{
	return						(!m_members.empty());
}

void CAgentManager::add				(CEntity *member)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(member);
	if (!stalker || !stalker->g_Alive())
		return;

	VERIFY2						(sizeof(squad_mask_type)*8 > members().size(),"Too many stalkers in squad!");

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
#ifndef IMPORTANT_BUILD
	VERIFY						(I == m_members.end());
#else
	if (I != m_members.end())
		return;
#endif
	
	m_members.push_back			(CMemberOrder(stalker));
}

void CAgentManager::remove			(CEntity *member, bool no_assert)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(member);
	if (!stalker)
		return;

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
	if (I == m_members.end()) {
#ifndef IMPORTANT_BUILD
		R_ASSERT				(no_assert);
#endif
		return;
	}
	m_members.erase				(I);
}

void CAgentManager::remove_links	(CObject *object)
{
	xr_vector<u16>::iterator		I = std::find(m_grenades_to_remove.begin(),m_grenades_to_remove.end(),object->ID());
	if (I != m_grenades_to_remove.end())
		m_grenades_to_remove.erase	(I);
}

