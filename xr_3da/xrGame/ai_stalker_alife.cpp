////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife.cpp
//	Created 	: 15.10.2004
//  Modified 	: 15.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai/stalker/ai_stalker.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_task_registry.h"
#include "alife_task.h"
#include "alife_space.h"

void CAI_Stalker::select_alife_task			()
{
	ALife::OBJECT_IT			I = m_tpKnownCustomers.begin();
	ALife::OBJECT_IT			E = m_tpKnownCustomers.end();
	for ( ; I != E; ++I) {
		ALife::OBJECT_TASK_MAP::const_iterator	J = ai().alife().tasks().cross().find(*I);
		R_ASSERT2		(ai().alife().tasks().cross().end() != J,"Can't find a specified customer in the Task registry!\nPossibly, there is no traders at all or there is no anomalous zones.");

		u32					l_dwMinTryCount = u32(-1);
		ALife::_TASK_ID		l_tBestTaskID = ALife::_TASK_ID(-1);
		ALife::TASK_SET::const_iterator	i = (*J).second.begin();
		ALife::TASK_SET::const_iterator	e = (*J).second.end();
		for ( ; i != e; ++i) {
			CALifeTask		*l_tpTask = ai().alife().tasks().task(*i);
			if (!l_tpTask->m_dwTryCount) {
				l_tBestTaskID = l_tpTask->m_tTaskID;
				break;
			}
			else
				if (l_tpTask->m_dwTryCount < l_dwMinTryCount) {
					l_dwMinTryCount = l_tpTask->m_dwTryCount;
					l_tBestTaskID = l_tpTask->m_tTaskID;
				}
		}

		if (ALife::_TASK_ID(-1) != l_tBestTaskID)
			m_current_alife_task	= ai().alife().tasks().task(l_tBestTaskID);
	}
}

CALifeTask &CAI_Stalker::current_alife_task	()
{
	if (!m_current_alife_task)
		select_alife_task	();
	VERIFY					(m_current_alife_task);
	return					(*m_current_alife_task);
}
