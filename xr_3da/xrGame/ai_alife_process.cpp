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
			u64							qwStartTime	= CPU::GetCycleCount();
			if (m_bFirstUpdate) {
				D_OBJECT_P_PAIR_IT		B = m_tpCurrentLevel->begin(), I = B;
				D_OBJECT_P_PAIR_IT		E = m_tpCurrentLevel->end();
				for ( ; I != E; I++)
					ProcessOnlineOfflineSwitches((*I).second);
				m_tNextFirstSwitchObjectID = (*B).second->ID;
				m_bFirstUpdate				= false;
			}
			else {
				// processing online/offline switches
				R_ASSERT2					(m_tpCurrentLevel,"There is no actor in the game!");
				D_OBJECT_P_PAIR_IT		B = m_tpCurrentLevel->begin();
				D_OBJECT_P_PAIR_IT		E = m_tpCurrentLevel->end();
				D_OBJECT_P_PAIR_IT		M = m_tpCurrentLevel->find(m_tNextFirstSwitchObjectID), I;
				int i=1;
				for (I = M ; I != E; I++, i++) {
					ProcessOnlineOfflineSwitches((*I).second);
					if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= m_qwMaxProcessTime) {
						m_tNextFirstSwitchObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
#ifdef DEBUG_LOG
						Msg("Not enough time (0)[%d : %d] !",i - 1, m_tpCurrentLevel->size() - i);
#endif
						Device.Statistic.TEST3.End();
						return;
					}
				}
				for (I = B; I != M; I++, i++) {
					ProcessOnlineOfflineSwitches((*I).second);
					if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= m_qwMaxProcessTime) {
						m_tNextFirstSwitchObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
#ifdef DEBUG_LOG
						Msg("Not enough time (1)[%d : %d] !",i - 1, m_tpCurrentLevel->size() - i);
#endif
						Device.Statistic.TEST3.End();
						return;
					}
				}
				if (CPU::GetCycleCount() - qwStartTime >= m_qwMaxProcessTime) {
#ifdef DEBUG_LOG
					Msg("Not enough time (2)[%d : %d] !",m_tpCurrentLevel->size(),m_tpCurrentLevel->size());
#endif
					Device.Statistic.TEST3.End();
					return;
				}
			}
			u64							qwMaxProcessTime = m_qwMaxProcessTime - qwStartTime;
			qwStartTime					= CPU::GetCycleCount();
			
			// updating objects being scheduled
#ifdef DEBUG_LOG
			Msg("Enough time (0) !");
#endif
			if (m_tpScheduledObjects.size()) {
				SCHEDULE_P_PAIR_IT		B = m_tpScheduledObjects.begin();
				SCHEDULE_P_PAIR_IT		E = m_tpScheduledObjects.end();
				SCHEDULE_P_PAIR_IT		M = m_tpScheduledObjects.find(m_tNextFirstProcessObjectID), I;
				int i=1;
				for (I = M ; I != E; I++, i++) {
					(*I).second->Update();
					if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= qwMaxProcessTime) {
						m_tNextFirstProcessObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
						Device.Statistic.TEST3.End();
						return;
					}
				}
				for (I = B; I != M; I++, i++) {
					(*I).second->Update();
					if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= qwMaxProcessTime) {
						m_tNextFirstProcessObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
						Device.Statistic.TEST3.End();
						return;
					}
				}
			}
			break;
		}
		default : NODEFAULT;
	}
	Device.Statistic.TEST3.End();
}