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
	if (!m_bLoaded)
		return;
	
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
				ALIFE_ENTITY_P_PAIR_IT	B = m_tpCurrentLevel->begin();
				ALIFE_ENTITY_P_PAIR_IT	E = m_tpCurrentLevel->end();
				ALIFE_ENTITY_P_PAIR_IT	I;
				for (I = B ; I != E; I++)
					vfFurlObjectOffline((*I).second);
				return;
			}
			u64							qwStartTime	= CPU::GetCycleCount();

			// processing online/offline switches
			VERIFY						(m_tpCurrentLevel);
			ALIFE_ENTITY_P_PAIR_IT		B = m_tpCurrentLevel->begin();
			ALIFE_ENTITY_P_PAIR_IT		E = m_tpCurrentLevel->end();
			ALIFE_ENTITY_P_PAIR_IT		M = m_tpCurrentLevel->find(m_tNextFirstSwitchObjectID), I;
			int i=1;
			for (I = M ; I != E; I++, i++) {
				ProcessOnlineOfflineSwitches((*I).second);
				if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= m_qwMaxProcessTime) {
					m_tNextFirstSwitchObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
#ifdef DEBUG_LOG
					Msg("Not enough time (0)[%d : %d] !",E - B, I - M);
#endif
					return;
				}
			}
			for (I = B; I != M; I++, i++) {
				ProcessOnlineOfflineSwitches((*I).second);
				if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= m_qwMaxProcessTime) {
					m_tNextFirstSwitchObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
#ifdef DEBUG_LOG
					Msg("Not enough time (1)[%d : %d] !",E - B, E - M + I - B);
#endif
					return;
				}
			}
			
			if (CPU::GetCycleCount() - qwStartTime >= m_qwMaxProcessTime) {
#ifdef DEBUG_LOG
				Msg("Not enough time (2)[%d : %d] !",E - B,E - B);
#endif
				return;
			}
			
			u64							qwMaxProcessTime = m_qwMaxProcessTime - qwStartTime;
			qwStartTime					= CPU::GetCycleCount();
			
			// updating objects being scheduled
#ifdef DEBUG_LOG
			Msg("Enough time (0) !");
#endif
			if (m_tpScheduledObjects.size()) {
				ALIFE_MONSTER_P_PAIR_IT	B = m_tpScheduledObjects.begin();
				ALIFE_MONSTER_P_PAIR_IT	E = m_tpScheduledObjects.end();
				ALIFE_MONSTER_P_PAIR_IT	M = m_tpScheduledObjects.find(m_tNextFirstProcessObjectID), I;
				int i=1;
				for (I = M ; I != E; I++, i++) {
					vfProcessNPC		((*I).second);
					if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= qwMaxProcessTime) {
						m_tNextFirstProcessObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
						return;
					}
				}
				for (I = B; I != M; I++, i++) {
					vfProcessNPC		((*I).second);
					if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i >= qwMaxProcessTime) {
						m_tNextFirstProcessObjectID = (++I == E) ? (*B).second->ID : (*I).second->ID;
						return;
					}
				}
			}
			break;
		}
		default : NODEFAULT;
	}
}

void CSE_ALifeSimulator::vfProcessNPC(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	CSE_ALifeHumanAbstract *tpHuman = dynamic_cast<CSE_ALifeHumanAbstract *>(tpALifeMonsterAbstract);
	if (tpHuman)
		vfUpdateHuman(tpHuman);
	else
		vfUpdateMonster(tpALifeMonsterAbstract);
	tpALifeMonsterAbstract->m_tTimeID = tfGetGameTime();
}

void CSE_ALifeSimulator::vfUpdateMonster(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract)
{
	vfChooseNextRoutePoint	(tpALifeMonsterAbstract);
	vfCheckForTheBattle		(tpALifeMonsterAbstract);
}

void CSE_ALifeSimulator::vfUpdateHuman(CSE_ALifeHumanAbstract *tpALifeHuman)
{
//	switch (tpALifeHuman->m_tTaskState) {
//		case eTaskStateNoTask : {
//			if (!tpALifeHuman->m_tpTaskIDs.size()) {
//				CSE_ALifeTrader *tpTrader = tpfGetNearestSuitableTrader(tpALifeHuman);
//				ffFindMinimalPath(tpALifeHuman->m_tGraphID,tpTrader->m_tGraphID,tpALifeHuman->m_tpaVertices);
//				tpALifeHuman->m_dwCurNode = 0;
//				tpALifeHuman->m_tTaskState = eTaskStateGoToTrader;
//				tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tCustomerID = _OBJECT_ID(-1);
//			}
//			else
//				tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
//			break;
//		}
//		case eTaskStateGoToTrader : {
//			if ((tpALifeHuman->m_dwCurNode >= (tpALifeHuman->m_tpaVertices.size() - 1)) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
//				tpALifeHuman->m_tpaVertices.clear();
//				tpALifeHuman->m_dwCurNode = u32(-1);
//				if (tpALifeHuman->m_tpTaskIDs.size()) {
//					OBJECT_PAIR_IT I = m_tObjectRegistry.find(tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tCustomerID);
//					CSE_ALifeTrader *tpTrader = 0;
//					if (I != m_tObjectRegistry.end())
//						tpTrader = dynamic_cast<CSE_ALifeTrader *>(I->second);
//					if (tpTrader)
//						vfCommunicateWithTrader(tpALifeHuman,tpTrader);
//					else
//						vfCommunicateWithTrader(tpALifeHuman,tpfGetNearestSuitableTrader(tpALifeHuman));
//				}
//				else
//					vfCommunicateWithTrader(tpALifeHuman,tpfGetNearestSuitableTrader(tpALifeHuman));
//				tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
//			}
//			break;
//		}
//		case eTaskStateChooseTask : {
//			tpALifeHuman->m_dwCurTask = 0;
//			_GRAPH_ID tGraphID = _GRAPH_ID(-1);
//			switch (tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tTaskType) {
//				case eTaskTypeSearchForItemCG :
//				case eTaskTypeSearchForItemOG : {
//					tGraphID = tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tGraphID;
//					break;
//				}
//				case eTaskTypeSearchForItemCL :
//				case eTaskTypeSearchForItemOL : {
//					//VERIFY(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size());
////					tpALifeHuman->m_baVisitedVertices.resize(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size());
////					tpALifeHuman->m_baVisitedVertices.assign(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size(),false);
////					tGraphID = m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID][tpALifeHuman->m_dwCurTaskLocation = 0];
////					tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
//					break;
//				}
//				default : NODEFAULT;
//			};
//			tpALifeHuman->m_tTaskState = eTaskStateGoing;
//			ffFindMinimalPath(tpALifeHuman->m_tGraphID,tGraphID,tpALifeHuman->m_tpaVertices);
//			tpALifeHuman->m_dwCurNode = 0;
//			break;
//		}
//		case eTaskStateGoing : {
//			if ((tpALifeHuman->m_dwCurNode + 1 >= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
//				if (bfCheckIfTaskCompleted(tpALifeHuman)) {
//					ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tObjectRegistry[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tCustomerID]->m_tGraphID,tpALifeHuman->m_tpaVertices);
//					tpALifeHuman->m_dwCurNode = 0;
//					tpALifeHuman->m_tTaskState = eTaskStateGoToTrader;
//				}
//				else {
//					switch (tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tTaskType) {
//						case eTaskTypeSearchForItemCG :
//						case eTaskTypeSearchForItemOG : {
//							if ((tpALifeHuman->m_dwCurNode + 1>= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID))
//								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
//							break;
//						}
//						case eTaskTypeSearchForItemCL :
//						case eTaskTypeSearchForItemOL : {
////							for (tpALifeHuman->m_dwCurTaskLocation++; (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) && (tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation]); tpALifeHuman->m_dwCurTaskLocation++);
////							if (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) {
////								tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
////								ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID][tpALifeHuman->m_dwCurTaskLocation],tpALifeHuman->m_tpaVertices);
////								tpALifeHuman->m_dwCurNode = 0;
////							}
////							else
////								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
//							break;
//						}
//					};
//					break;
//				}
//			}
//			break;
//		}
//		case eTaskStateSearching : {
//			if ((tpALifeHuman->m_dwCurNode + 1 >= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
//				if (bfCheckIfTaskCompleted(tpALifeHuman)) {
//					ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tObjectRegistry[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tCustomerID]->m_tGraphID,tpALifeHuman->m_tpaVertices);
//					tpALifeHuman->m_dwCurNode = 0;
//					tpALifeHuman->m_tTaskState = eTaskStateGoToTrader;
//				}
//				else {
//					switch (tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tTaskType) {
//						case eTaskTypeSearchForItemCG :
//						case eTaskTypeSearchForItemOG : {
//							if ((tpALifeHuman->m_dwCurNode + 1>= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID))
//								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
//							break;
//						}
//						case eTaskTypeSearchForItemCL :
//						case eTaskTypeSearchForItemOL : {
////							for (tpALifeHuman->m_dwCurTaskLocation++; (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) && (tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation]); tpALifeHuman->m_dwCurTaskLocation++);
////							if (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) {
////								tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
////								ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID][tpALifeHuman->m_dwCurTaskLocation],tpALifeHuman->m_tpaVertices);
////								tpALifeHuman->m_dwCurNode = 0;
////							}
////							else
////								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
//							break;
//						}
//					};
//					break;
//				}
//			}
//			break;
//		}
//		default : NODEFAULT;
//	};
	VERIFY(!tpALifeHuman->m_bOnline);
	vfChooseNextRoutePoint	(tpALifeHuman);
	vfCheckForTheBattle		(tpALifeHuman);
	bfCheckForItems			(tpALifeHuman);
//	vfCheckForDeletedEvents	(tpALifeHuman);
}