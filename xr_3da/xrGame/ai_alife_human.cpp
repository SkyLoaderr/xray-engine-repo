////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_human.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation of humans
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

void CSE_ALifeSimulator::vfChooseHumanTask(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	OBJECT_IT					I = tpALifeHumanAbstract->m_tpKnownCustomers.begin();
	OBJECT_IT					E = tpALifeHumanAbstract->m_tpKnownCustomers.end();
	for ( ; I != E; I++) {
		OBJECT_TASK_PAIR_IT		J = m_tTaskCrossMap.find(*I);
		R_ASSERT2				(J != m_tTaskCrossMap.end(),"Can't find a specified customer in the Task registry!");
		
		TASK_SET_PAIR_IT		i = (*J).second.begin();
		TASK_SET_PAIR_IT		e = (*J).second.end();
		for ( ; i != e; i++) {
			if (!tpfGetTaskByID(*i)->m_dwTryCount)
				break;
		}
		
		if (i != e) {
			vfSetCurrentTask	(tpALifeHumanAbstract,*i);
			break;
		}
	}
}

void CSE_ALifeSimulator::vfChooseTask(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	_GRAPH_ID				tGraphID = _GRAPH_ID(-1);
	vfChooseHumanTask		(tpALifeHumanAbstract);
	CSE_ALifeTask			*l_tpTask = tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID);
	switch (l_tpTask->m_tTaskType) {
		case eTaskTypeSearchForItemCG :
		case eTaskTypeSearchForItemOG : {
			tGraphID		= l_tpTask->m_tGraphID;
			break;
		}
		case eTaskTypeSearchForItemCL :
		case eTaskTypeSearchForItemOL : {
			//VERIFY		(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size());
			//tpALifeHuman->m_baVisitedVertices.resize(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size());
			//tpALifeHuman->m_baVisitedVertices.assign(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size(),false);
			//tGraphID		= m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID][tpALifeHuman->m_dwCurTaskLocation = 0];
			//tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
			break;
		}
		default				: NODEFAULT;
	};
	tpALifeHumanAbstract->m_tTaskState = eTaskStateAccomplishing;
	ffFindMinimalPath		(tpALifeHumanAbstract->m_tGraphID,tGraphID,tpALifeHumanAbstract->m_tpaVertices);
	tpALifeHumanAbstract->m_dwCurNode = 0;
	tpALifeHumanAbstract->m_tNextGraphID = tpALifeHumanAbstract->m_tGraphID;
	tpALifeHumanAbstract->m_fCurSpeed	= tpALifeHumanAbstract->m_fGoingSpeed;
}

void CSE_ALifeSimulator::vfAccomplishTask(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	if (bfCheckIfTaskCompleted(tpALifeHumanAbstract)) {
		ffFindMinimalPath	(tpALifeHumanAbstract->m_tGraphID,tpfGetObjectByID(tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID)->m_tCustomerID)->m_tGraphID,tpALifeHumanAbstract->m_tpaVertices);
		tpALifeHumanAbstract->m_dwCurNode = 0;
		tpALifeHumanAbstract->m_tTaskState = eTaskStateGoToCustomer;
	}
	else {
		switch (tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID)->m_tTaskType) {
			case eTaskTypeSearchForItemCG :
			case eTaskTypeSearchForItemOG : {
				if ((tpALifeHumanAbstract->m_dwCurNode + 1>= (tpALifeHumanAbstract->m_tpaVertices.size())) && (tpALifeHumanAbstract->m_tGraphID == tpALifeHumanAbstract->m_tNextGraphID)) {
					tpfGetTaskByID		(tpALifeHumanAbstract->m_dwCurTaskID)->m_dwTryCount++;
					tpALifeHumanAbstract->m_tTaskState = eTaskStateChooseTask;
				}
				break;
			}
			case eTaskTypeSearchForItemCL :
			case eTaskTypeSearchForItemOL : {
				tpfGetTaskByID		(tpALifeHumanAbstract->m_dwCurTaskID)->m_dwTryCount++;
				tpALifeHumanAbstract->m_tTaskState = eTaskStateChooseTask;
//				for (tpALifeHuman->m_dwCurTaskLocation++; (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) && (tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation]); tpALifeHuman->m_dwCurTaskLocation++);
//					if (tpALifeHuman->m_dwCurTaskLocation < m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size()) {
//						tpALifeHuman->m_baVisitedVertices[tpALifeHuman->m_dwCurTaskLocation] = true;
//						ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID][tpALifeHuman->m_dwCurTaskLocation],tpALifeHuman->m_tpaVertices);
//						tpALifeHuman->m_dwCurNode = 0;
//					}
//					else
//						tpALifeHuman->m_tTaskState = eTaskStateChooseTask;
			}
		}
	}
}

void CSE_ALifeSimulator::vfGoToCustomer(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	tpALifeHumanAbstract->m_tpaVertices.clear();
	tpALifeHumanAbstract->m_dwCurNode = 0;
	if (int(tpALifeHumanAbstract->m_dwCurTaskID) > 0) {
		CSE_ALifeTask			*l_tpTask = tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID);
		CSE_ALifeTraderAbstract	*l_tpTraderAbstract = dynamic_cast<CSE_ALifeTraderAbstract*>(tpfGetObjectByID(l_tpTask->m_tCustomerID));
		if (l_tpTraderAbstract)
			vfCommunicateWithCustomer(tpALifeHumanAbstract,l_tpTraderAbstract);
	}
	tpALifeHumanAbstract->m_tTaskState = eTaskStateChooseTask;
}

void CSE_ALifeSimulator::vfSearchObject(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
}

void CSE_ALifeSimulator::vfCommunicateWithCustomer(CSE_ALifeHumanAbstract *tpALifeHumanAbstract, CSE_ALifeTraderAbstract *tpTraderAbstract)
{
	// update items
	if (tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID,true)) {
		OBJECT_IT								I;
		if (bfCheckIfTaskCompleted(tpALifeHumanAbstract, I)) {
			OBJECT_PAIR_IT						J = m_tObjectRegistry.find(*I);
			R_ASSERT2							(J != m_tObjectRegistry.end(), "Specified object hasn't been found in the Object registry!");
			CSE_ALifeItem						*tpALifeItem = dynamic_cast<CSE_ALifeItem *>((*J).second);
			if (tpTraderAbstract->m_dwMoney >= tpALifeItem->m_dwCost) {
				// changing item parent
				tpTraderAbstract->children.push_back(*I);
				tpALifeHumanAbstract->children.erase(I);
				tpALifeItem->ID_Parent			= tpTraderAbstract->ID;
				// changing cumulative mass
				tpTraderAbstract->m_fCumulativeItemMass += tpALifeItem->m_fMass;
				tpALifeHumanAbstract->m_fCumulativeItemMass -= tpALifeItem->m_fMass;
				// paying/receiving money
				tpTraderAbstract->m_dwMoney		-= tpALifeItem->m_dwCost;
				tpALifeHumanAbstract->m_dwMoney += tpALifeItem->m_dwCost;
			}
		}
	}
	
	// update events
	
	// update tasks
}

void CSE_ALifeSimulator::vfUpdateHuman(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	R_ASSERT3			(tpALifeHumanAbstract->fHealth > 0,"Can't update death object ",tpALifeHumanAbstract->s_name_replace);
	R_ASSERT3			(!tpALifeHumanAbstract->m_bOnline,"Can't update online object ",tpALifeHumanAbstract->s_name_replace);
	bool				bOk;
	do {
		switch (tpALifeHumanAbstract->m_tTaskState) {
			case eTaskStateChooseTask : {
				vfChooseTask(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateAccomplishing : {
				if ((tpALifeHumanAbstract->m_dwCurNode + 1 >= (tpALifeHumanAbstract->m_tpaVertices.size())) && (tpALifeHumanAbstract->m_tGraphID == tpALifeHumanAbstract->m_tNextGraphID))
					vfAccomplishTask(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateGoToCustomer : {
				if ((tpALifeHumanAbstract->m_dwCurNode >= (tpALifeHumanAbstract->m_tpaVertices.size() - 1)) && (tpALifeHumanAbstract->m_tGraphID == tpALifeHumanAbstract->m_tNextGraphID))
					vfGoToCustomer(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateSearching : {
				vfSearchObject(tpALifeHumanAbstract);
				break;
			}
			default				: NODEFAULT;
		}

		bOk						= bfChooseNextRoutePoint(tpALifeHumanAbstract);
		vfCheckForTheBattle		(tpALifeHumanAbstract);
		bfCheckForItems			(tpALifeHumanAbstract);
//		vfCheckForDeletedEvents	(tpALifeHumanAbstract);
	}
	while (bOk);
}