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

//#define WRITE_TO_LOG

void CAI_ALife::Update(u32 dt)
{
	CSheduled::Update(dt);
	if (!m_bLoaded)
		return;
#ifdef WRITE_TO_LOG
	Msg("* %7.2fs",Level().timeServer()/1000.f);
#endif
	u64	qwStartTime = CPU::GetCycleCount();
	if (m_tpScheduledObjects.size()) {
		int i=0;
		do {
			i++;
			m_dwObjectsBeingProcessed = ((m_dwObjectsBeingProcessed + 1) % m_tpScheduledObjects.size());
			vfProcessNPC(m_tpScheduledObjects[m_dwObjectsBeingProcessed]);
		}
		while (((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i < m_qwMaxProcessTime) && (i < (int)m_tpScheduledObjects.size()));
	}
#ifdef WRITE_TO_LOG
	u64 t2x = CPU::GetCycleCount() - qwStartTime;
	Msg("* %.3f microseconds",CPU::cycles2microsec*t2x);
#endif
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
				tpALifeHuman->m_tCurTask.m_tCustomerID = _OBJECT_ID(-1);
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
					OBJECT_PAIR_IT I = m_tObjectRegistry.find(tpALifeHuman->m_tCurTask.m_tCustomerID);
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
			CALifeTask *tpTask = m_tTaskRegistry[tpALifeHuman->m_tpTaskIDs[0]];
			tpALifeHuman->m_tCurTask = *tpTask;;
			_GRAPH_ID tGraphID = _GRAPH_ID(-1);
			switch (tpALifeHuman->m_tCurTask.m_tTaskType) {
				case eTaskTypeSearchForItemCG :
				case eTaskTypeSearchForItemOG : {
					tGraphID = tpALifeHuman->m_tCurTask.m_tGraphID;
					break;
				}
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemOL : {
					VERIFY(m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID].size());
					tpALifeHuman->m_baVisitedVertices.resize(m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID].size());
					tpALifeHuman->m_baVisitedVertices.assign(m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID].size(),false);
					tGraphID = m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID][tpALifeHuman->m_dwCurTaskLocation = 0];
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
					Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tObjectRegistry[tpALifeHuman->m_tCurTask.m_tCustomerID]->m_tGraphID,tpALifeHuman->m_tpaVertices);
					tpALifeHuman->m_dwCurNode = 0;
					tpALifeHuman->m_tTaskState = eTaskStateGoToTrader;
				}
				else {
					switch (tpALifeHuman->m_tCurTask.m_tTaskType) {
						case eTaskTypeSearchForItemCG :
						case eTaskTypeSearchForItemOG : {
							if ((tpALifeHuman->m_dwCurNode + 1>= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID))
								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
							break;
						}
						case eTaskTypeSearchForItemCL :
						case eTaskTypeSearchForItemOL : {
							for (tpALifeHuman->m_dwCurTaskLocation++; (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID].size()) && (tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation]); tpALifeHuman->m_dwCurTaskLocation++);
							if (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID].size()) {
								tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
								Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID][tpALifeHuman->m_dwCurTaskLocation],tpALifeHuman->m_tpaVertices);
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
					Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tObjectRegistry[tpALifeHuman->m_tCurTask.m_tCustomerID]->m_tGraphID,tpALifeHuman->m_tpaVertices);
					tpALifeHuman->m_dwCurNode = 0;
					tpALifeHuman->m_tTaskState = eTaskStateGoToTrader;
				}
				else {
					switch (tpALifeHuman->m_tCurTask.m_tTaskType) {
						case eTaskTypeSearchForItemCG :
						case eTaskTypeSearchForItemOG : {
							if ((tpALifeHuman->m_dwCurNode + 1>= (tpALifeHuman->m_tpaVertices.size())) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID))
								tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
							break;
						}
						case eTaskTypeSearchForItemCL :
						case eTaskTypeSearchForItemOL : {
							for (tpALifeHuman->m_dwCurTaskLocation++; (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID].size()) && (tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation]); tpALifeHuman->m_dwCurTaskLocation++);
							if (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID].size()) {
								tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
								Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tpTerrain[tpALifeHuman->m_tCurTask.m_tLocationID][tpALifeHuman->m_dwCurTaskLocation],tpALifeHuman->m_tpaVertices);
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