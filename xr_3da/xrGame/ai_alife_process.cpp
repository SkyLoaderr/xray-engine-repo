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
	D_OBJECT_PAIR_IT					I = m_tpCurrentLevel->find(m_tNextFirstSwitchObjectID);;
//	u64									qwStartTime	= CPU::GetCycleCount();
//	u64									l_qwMaxProcessTime = m_qwMaxProcessTime/SWITCH_TIME_FACTOR;
	R_ASSERT2							(m_tpCurrentLevel,"There is no actor in the game!");
	for (int i=1; ; i++) {
		m_bSwitchChanged				= false;
		if ((*I).second->m_qwSwitchCounter == m_qwCycleCounter) {
#ifdef ALIFE_LOG
			Msg							("[LSS][OOS][%d : %d]",i, m_tpCurrentLevel->size());
#endif
			return;
		}

		_OBJECT_ID						l_tSaveObjectID = m_tNextFirstSwitchObjectID;
		(*I).second->m_qwSwitchCounter	= m_qwCycleCounter;
		ProcessOnlineOfflineSwitches	((*I).second);
		
		if (m_bSwitchChanged) {
			I							= m_tpCurrentLevel->find(m_tNextFirstSwitchObjectID);
			R_ASSERT2					(I != m_tpCurrentLevel->end(),"Cannot find specified object on the current level map");
		}
		I++;
		if (!m_bSwitchChanged || (l_tSaveObjectID == m_tNextFirstSwitchObjectID))
			if (I == m_tpCurrentLevel->end()) {
				// this map cannnot be empty, because at least actor must belong to it
				R_ASSERT2					(!m_tpCurrentLevel->empty(),"Impossible situation : current level contains no objects! (where is an actor, for example?)");
				I							= m_tpCurrentLevel->begin();
			}
		m_tNextFirstSwitchObjectID		= (*I).first;

//		if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= l_qwMaxProcessTime) {
//#ifdef ALIFE_LOG
//			Msg							("[LSS][OOS][%d : %d]",i, m_tpCurrentLevel->size());
//#endif
//			return;
//		}
	}
}

void CSE_ALifeSimulator::vfProcessUpdates()
{
	u64							qwStartTime	= CPU::GetCycleCount();
	u64							l_qwMaxProcessTime = m_qwMaxProcessTime - m_qwMaxProcessTime/SWITCH_TIME_FACTOR;
	if (m_tpScheduledObjects.size()) {
		SCHEDULE_P_PAIR_IT		I = m_tpScheduledObjects.find(m_tNextFirstProcessObjectID);
		R_ASSERT2				(I != m_tpScheduledObjects.end(),"Cannot find specified object on the current level map");
		for (int i=1; ; i++) {
			m_bUpdateChanged				= false;
			if ((*I).second->m_qwUpdateCounter == m_qwCycleCounter) {
	#ifdef ALIFE_LOG
				Msg							("[LSS][US][%d : %d]",i, m_tpScheduledObjects.size());
	#endif
				return;
			}

			_OBJECT_ID						l_tSaveObjectID = m_tNextFirstSwitchObjectID;
			
			(*I).second->m_qwUpdateCounter	= m_qwCycleCounter;
			(*I).second->Update				();

			if (m_bUpdateChanged) {
				I							= m_tpScheduledObjects.find(m_tNextFirstProcessObjectID);
				R_ASSERT2					(I != m_tpScheduledObjects.end(),"Cannot find specified object on the current level map");
			}
			I++;
			if (!m_bUpdateChanged || (l_tSaveObjectID == m_tNextFirstSwitchObjectID))
				if (I == m_tpScheduledObjects.end()) {
					I						= m_tpScheduledObjects.begin();
					if (I == m_tpScheduledObjects.end()) {
#ifdef ALIFE_LOG
						Msg							("[LSS][US][%d : %d]",i, m_tpScheduledObjects.size());
#endif
						return;
					}
				}
			m_tNextFirstProcessObjectID		= (*I).first;

			if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= l_qwMaxProcessTime) {
	#ifdef ALIFE_LOG
				Msg							("[LSS][US][%d : %d]",i, m_tpScheduledObjects.size());
	#endif
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
	
	m_qwCycleCounter++;
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
				break;
			}
			if (m_bFirstUpdate) {
				u64						l_qwSave = m_qwMaxProcessTime;
				m_qwMaxProcessTime		= u64(60000000)*CPU::cycles_per_microsec;
				m_tNextFirstSwitchObjectID = (*m_tpCurrentLevel->begin()).second->ID;
				vfProcessAllTheSwitches	();
				m_qwMaxProcessTime		= l_qwSave;
				m_bFirstUpdate			= false;
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