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

void CAI_ALife::Update(u32 dt)
{
	CSheduled::Update					(dt);
	
	if (!m_bLoaded)
		return;
	
//	switch (m_tZoneState) {
//		case eZoneStateSurge : {
//			vfGenerateArtefacts			();
//			TRADER_P_IT					I = m_tpTraders.begin();
//			TRADER_P_IT					E = m_tpTraders.end();
//			for ( ; I != E; I++) {
//				vfSellArtefacts			(**I);
//				vfUpdateArtefactOrders	(**I);
//				vfGiveMilitariesBribe	(**I);
//				vfBuySupplies			(**I);
//				vfAssignPrices			(**I);
//			}
//			vfBallanceCreatures			();
//			vfUpdateCreatures			();
//			Save						();
//			m_tTimeAfterSurge			= 0;
//			m_tZoneState				= eZoneStateAfterSurge;
//			break;
//		}
//		case eZoneStateAfterSurge : {
//			u64							qwStartTime	= CPU::GetCycleCount();
//			if (m_tpScheduledObjects.size()) {
//				int						i=0;
//				do {
//					i++;
//					m_dwObjectsBeingProcessed = ((m_dwObjectsBeingProcessed + 1) % m_tpScheduledObjects.size());
//					vfProcessNPC		(m_tpScheduledObjects[m_dwObjectsBeingProcessed]);
//				}
//				while (((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i < m_qwMaxProcessTime) && (i < (int)m_tpScheduledObjects.size()));
//			}
//			break;
//		}
//		default : NODEFAULT;
//	}
	
	OBJECT_PAIR_IT					I = m_tObjectRegistry.begin();
	OBJECT_PAIR_IT					E = m_tObjectRegistry.end();
	CObject							*tpObject = Level().Objects.net_Find(m_tpActor->ID);
	if (!tpObject)
		return;
	for ( ;I != E; I++)
		ProcessOnlineOfflineSwitches(tpObject, (*I).second);
}

void CAI_ALife::vfProcessNPC(CALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	CALifeHumanAbstract *tpALifeHumanAbstract = dynamic_cast<CALifeHumanAbstract *>(tpALifeMonsterAbstract);
	if (tpALifeHumanAbstract) {
		CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeMonsterAbstract);
		if (tpALifeHuman)
			vfUpdateHuman(tpALifeHuman);
		else
			vfUpdateHumanGroup(dynamic_cast<CALifeHumanGroup *>(tpALifeMonsterAbstract));
	}
	else
		vfUpdateMonster(tpALifeMonsterAbstract);
	tpALifeMonsterAbstract->m_tTimeID = tfGetGameTime();
}

void CAI_ALife::vfUpdateMonster(CALifeMonsterAbstract *tpALifeMonsterAbstract)
{
	vfChooseNextRoutePoint	(tpALifeMonsterAbstract);
	vfCheckForTheBattle		(tpALifeMonsterAbstract);
}

void CAI_ALife::vfUpdateHumanGroup(CALifeHumanGroup *tpALifeHumanGroup)
{
}

void CAI_ALife::vfUpdateHuman(CALifeHuman *tpALifeHuman)
{
	switch (tpALifeHuman->m_tTaskState) {
		case eTaskStateNoTask : {
			if (!tpALifeHuman->m_tpTaskIDs.size()) {
				CALifeTrader *tpTrader = tpfGetNearestSuitableTrader(tpALifeHuman);
				Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,tpTrader->m_tGraphID,tpALifeHuman->m_tpaVertices);
				tpALifeHuman->m_dwCurNode = 0;
				tpALifeHuman->m_tTaskState = eTaskStateGoToTrader;
				tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tCustomerID = _OBJECT_ID(-1);
			}
			else
				tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
			break;
		}
		case eTaskStateGoToTrader : {
			if ((tpALifeHuman->m_dwCurNode >= (tpALifeHuman->m_tpaVertices.size() - 1)) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
				tpALifeHuman->m_tpaVertices.clear();
				tpALifeHuman->m_dwCurNode = u32(-1);
				if (tpALifeHuman->m_tpTaskIDs.size()) {
					OBJECT_PAIR_IT I = m_tObjectRegistry.find(tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tCustomerID);
					CALifeTrader *tpTrader = 0;
					if (I != m_tObjectRegistry.end())
						tpTrader = dynamic_cast<CALifeTrader *>(I->second);
					if (tpTrader)
						vfCommunicateWithTrader(tpALifeHuman,tpTrader);
					else
						vfCommunicateWithTrader(tpALifeHuman,tpfGetNearestSuitableTrader(tpALifeHuman));
				}
				else
					vfCommunicateWithTrader(tpALifeHuman,tpfGetNearestSuitableTrader(tpALifeHuman));
				tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
			}
			break;
		}
		case eTaskStateChooseTask : {
			tpALifeHuman->m_dwCurTask = 0;
			_GRAPH_ID tGraphID = _GRAPH_ID(-1);
			switch (tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tTaskType) {
				case eTaskTypeSearchForItemCG :
				case eTaskTypeSearchForItemOG : {
					tGraphID = tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tGraphID;
					break;
				}
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemOL : {
					VERIFY(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size());
					tpALifeHuman->m_baVisitedVertices.resize(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size());
					tpALifeHuman->m_baVisitedVertices.assign(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size(),false);
					tGraphID = m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID][tpALifeHuman->m_dwCurTaskLocation = 0];
					tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
					break;
				}
				default : NODEFAULT;
			};
			tpALifeHuman->m_tTaskState = eTaskStateGoing;
			Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,tGraphID,tpALifeHuman->m_tpaVertices);
			tpALifeHuman->m_dwCurNode = 0;
			break;
		}
		case eTaskStateGoing : {
			if ((tpALifeHuman->m_dwCurNode + 1 >= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
				if (bfCheckIfTaskCompleted(tpALifeHuman)) {
					Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tObjectRegistry[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tCustomerID]->m_tGraphID,tpALifeHuman->m_tpaVertices);
					tpALifeHuman->m_dwCurNode = 0;
					tpALifeHuman->m_tTaskState = eTaskStateGoToTrader;
				}
				else {
					switch (tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tTaskType) {
						case eTaskTypeSearchForItemCG :
						case eTaskTypeSearchForItemOG : {
							if ((tpALifeHuman->m_dwCurNode + 1>= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID))
								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
							break;
						}
						case eTaskTypeSearchForItemCL :
						case eTaskTypeSearchForItemOL : {
							for (tpALifeHuman->m_dwCurTaskLocation++; (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) && (tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation]); tpALifeHuman->m_dwCurTaskLocation++);
							if (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) {
								tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
								Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID][tpALifeHuman->m_dwCurTaskLocation],tpALifeHuman->m_tpaVertices);
								tpALifeHuman->m_dwCurNode = 0;
							}
							else
								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
							break;
						}
					};
					break;
				}
			}
			break;
		}
		case eTaskStateSearching : {
			if ((tpALifeHuman->m_dwCurNode + 1 >= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
				if (bfCheckIfTaskCompleted(tpALifeHuman)) {
					Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tObjectRegistry[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tCustomerID]->m_tGraphID,tpALifeHuman->m_tpaVertices);
					tpALifeHuman->m_dwCurNode = 0;
					tpALifeHuman->m_tTaskState = eTaskStateGoToTrader;
				}
				else {
					switch (tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tTaskType) {
						case eTaskTypeSearchForItemCG :
						case eTaskTypeSearchForItemOG : {
							if ((tpALifeHuman->m_dwCurNode + 1>= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID))
								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
							break;
						}
						case eTaskTypeSearchForItemCL :
						case eTaskTypeSearchForItemOL : {
							for (tpALifeHuman->m_dwCurTaskLocation++; (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) && (tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation]); tpALifeHuman->m_dwCurTaskLocation++);
							if (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) {
								tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
								Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID][tpALifeHuman->m_dwCurTaskLocation],tpALifeHuman->m_tpaVertices);
								tpALifeHuman->m_dwCurNode = 0;
							}
							else
								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
							break;
						}
					};
					break;
				}
			}
			break;
		}
		default : NODEFAULT;
	};
	vfChooseNextRoutePoint	(tpALifeHuman);
	vfCheckForTheBattle		(tpALifeHuman);
	vfCheckForDeletedEvents	(tpALifeHuman);
}