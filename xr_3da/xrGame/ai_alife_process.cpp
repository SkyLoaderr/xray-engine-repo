////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_process.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation processing
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "a_star.h"

#define SWITCH_TIME_FACTOR u64(10)

void CSE_ALifeSimulator::vfProcessAllTheSwitches()
{
	// processing online/offline switches
	u64						qwStartTime	= CPU::GetCycleCount();
	u64						l_qwMaxProcessTime = m_qwMaxProcessTime/SWITCH_TIME_FACTOR;
	R_ASSERT2				(m_tpCurrentLevel,"There is no actor in the game!");
	D_OBJECT_P_PAIR_IT		B = m_tpCurrentLevel->begin();
	D_OBJECT_P_PAIR_IT		E = m_tpCurrentLevel->end();
	D_OBJECT_P_PAIR_IT		M = m_tpCurrentLevel->find(m_tNextFirstSwitchObjectID), I;
	R_ASSERT				(M != E);
	int i=1;
	for (I = M ; I != E; I++, i++) {
		ProcessOnlineOfflineSwitches((*I).second);
		if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= l_qwMaxProcessTime) {
			m_tNextFirstSwitchObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
#ifdef ALIFE_LOG
			Msg("[LSS][OOS0][%d : %d]",i, m_tpCurrentLevel->size() - i);
#endif
			return;
		}
	}
	for (I = B; I != M; I++, i++) {
		ProcessOnlineOfflineSwitches((*I).second);
		if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= l_qwMaxProcessTime) {
			m_tNextFirstSwitchObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
#ifdef ALIFE_LOG
			Msg("[LSS][OOS1][%d : %d]",i, m_tpCurrentLevel->size() - i);
#endif
			return;
		}
	}
}

void CSE_ALifeSimulator::vfProcessUpdates()
{
	u64							qwStartTime	= CPU::GetCycleCount();
	u64							l_qwMaxProcessTime = m_qwMaxProcessTime - m_qwMaxProcessTime/SWITCH_TIME_FACTOR;
	if (m_tpScheduledObjects.size()) {
		SCHEDULE_P_PAIR_IT		B = m_tpScheduledObjects.begin();
		SCHEDULE_P_PAIR_IT		E = m_tpScheduledObjects.end();
		SCHEDULE_P_PAIR_IT		M = m_tpScheduledObjects.find(m_tNextFirstProcessObjectID), I;
		int i=1;
		for (I = M ; I != E; I++, i++) {
			(*I).second->Update();
			if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= l_qwMaxProcessTime) {
				m_tNextFirstProcessObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
#ifdef ALIFE_LOG
				Msg("[LSS][U0][%d : %d]",i, m_tpScheduledObjects.size() - i);
#endif
				Device.Statistic.TEST3.End();
				return;
			}
		}
		for (I = B; I != M; I++, i++) {
			(*I).second->Update();
			if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= l_qwMaxProcessTime) {
				m_tNextFirstProcessObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
#ifdef ALIFE_LOG
				Msg("[LSS][U1][%d : %d]",i, m_tpScheduledObjects.size() - i);
#endif
				Device.Statistic.TEST3.End();
				return;
			}
		}
	}
}

void CSE_ALifeSimulator::shedule_Update			(u32 dt)
{
	Device.Statistic.TEST3.Begin		();
	if (!m_bLoaded) {
		Device.Statistic.TEST3.End		();
		return;
	}
	
	vfInitAI_ALifeMembers				();

	switch (m_tZoneState) {
		case eZoneStateSurge : {
			vfPerformSurge				();
			Save						(m_caSaveName);
			m_tLastSurgeTime			= tfGetGameTime();
			m_tNextSurgeTime			= m_tLastSurgeTime + 7*24*3600*1000; // a week in milliseconds
			m_tZoneState				= eZoneStateAfterSurge;
			break;
		}
		case eZoneStateAfterSurge : {
			if (tfGetGameTime() > m_tNextSurgeTime) {
				m_tZoneState			= eZoneStateSurge;
				D_OBJECT_P_PAIR_IT		B = m_tpCurrentLevel->begin();
				D_OBJECT_P_PAIR_IT		E = m_tpCurrentLevel->end();
				D_OBJECT_P_PAIR_IT		I;
				for (I = B ; I != E; I++)
					vfFurlObjectOffline((*I).second);
				Device.Statistic.TEST3.End();
				return;
			}
			if (m_bFirstUpdate) {
				D_OBJECT_P_PAIR_IT		B = m_tpCurrentLevel->begin(), I = B;
				D_OBJECT_P_PAIR_IT		E = m_tpCurrentLevel->end();
				for ( ; I != E; I++)
					ProcessOnlineOfflineSwitches((*I).second);
				m_tNextFirstSwitchObjectID = (*B).second->ID;
				m_bFirstUpdate				= false;
			}
			else
				vfProcessAllTheSwitches	();
			vfProcessUpdates();
			break;
		}
		default : NODEFAULT;
	}
	Device.Statistic.TEST3.End();
}