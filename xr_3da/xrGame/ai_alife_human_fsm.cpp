////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_human_fsm.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life FSM of humans
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

void CSE_ALifeSimulator::vfAccomplishTask(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	// build path and wait until we go to the end of it
	if ((tpALifeHumanAbstract->m_dwCurNode + 1 >= (tpALifeHumanAbstract->m_tpaVertices.size())) && (tpALifeHumanAbstract->m_tGraphID == tpALifeHumanAbstract->m_tNextGraphID)) {
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
}

void CSE_ALifeSimulator::vfGoToCustomer(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	// go to customer to get something to accomplish task
	if ((tpALifeHumanAbstract->m_dwCurNode >= (tpALifeHumanAbstract->m_tpaVertices.size() - 1)) && (tpALifeHumanAbstract->m_tGraphID == tpALifeHumanAbstract->m_tNextGraphID)) {
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
	tpALifeHumanAbstract->m_tTaskState = eTaskStateAccomplishTask;
	ffFindMinimalPath		(tpALifeHumanAbstract->m_tGraphID,tGraphID,tpALifeHumanAbstract->m_tpaVertices);
	tpALifeHumanAbstract->m_dwCurNode = 0;
	tpALifeHumanAbstract->m_tNextGraphID = tpALifeHumanAbstract->m_tGraphID;
	tpALifeHumanAbstract->m_fCurSpeed	= tpALifeHumanAbstract->m_fGoingSpeed;
}

void CSE_ALifeSimulator::vfHealthCare(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	// if health is low -> use medikit
	// if hunhry -> eat or drink vodka
	// if irradiating -> use antirad or drink vodka 
	// if tired -> sleep, take a rest
	tpALifeHumanAbstract->m_tTaskState = eTaskStateChooseTask;
}

void CSE_ALifeSimulator::vfBuySupplies(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	// choose an appropriate trader and go to him to buy supplies
}

void CSE_ALifeSimulator::vfBringToCustomer(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	// go to customer to sell found artefacts
}

void CSE_ALifeSimulator::vfGoToSOS(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	// go to person who sent SOS to save or kill
}

void CSE_ALifeSimulator::vfSendSOS(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	// send SOS by myself
}

void CSE_ALifeSimulator::vfSearchObject(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
}

