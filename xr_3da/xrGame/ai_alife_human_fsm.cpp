////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_human_fsm.cpp
//	Created 	: 24.07.2003
//  Modified 	: 24.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life FSM of humans
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

#define OLD_BEHAVIOUR

void CSE_ALifeHumanAbstract::Update			(CSE_ALifeSimulator *tpALife)
{
	if (fHealth <= 0)
		return;
#ifdef OLD_BEHAVOIUR
	inherited2::Update		(tpALife);
	tpALife->vfCheckForTheBattle(this);
	bfProcessItems			(tpALife);
#else
	R_ASSERT3			(!m_bOnline,"Can't update online object ",s_name_replace);
	bool				bOk;
	do {
		switch (m_tTaskState) {
			case eTaskStateChooseTask : {
				vfChooseTask(tpALife);
				break;
										}
			case eTaskStateHealthCare : {
				vfHealthCare(tpALife);
				break;
										}
			case eTaskStateBuySupplies : {
				vfBuySupplies(tpALife);
				break;
										 }
			case eTaskStateGoToCustomer : {
				vfGoToCustomer(tpALife);
				break;
										  }
			case eTaskStateBringToCustomer : {
				vfBringToCustomer(tpALife);
				break;
											 }
			case eTaskStateGoToSOS : {
				vfGoToSOS(tpALife);
				break;
									 }
			case eTaskStateSendSOS : {
				vfSendSOS(tpALife);
				break;
									 }
			case eTaskStateAccomplishTask : {
				vfAccomplishTask(tpALife);
				break;
											}
			case eTaskStateSearchItem : {
				vfSearchObject	(tpALife);
				break;
										}
			default				: NODEFAULT;
		}
		bOk						= bfChooseNextRoutePoint(tpALife);
		tpALife->vfCheckForTheBattle(this);
		bfProcessItems			(tpALife);
		vfCheckForDeletedEvents	(tpALife);
	}
	while (bOk && (tpALife->m_tpActor->o_Position.distance_to(o_Position) > tpALife->m_fOnlineDistance));
#endif
	m_tTimeID					= tpALife->tfGetGameTime();
}

void CSE_ALifeHumanAbstract::vfChooseTask(CSE_ALifeSimulator *tpALife)
{
	if (!HealthIsGood()) {
		if (!CanTreat()) {
			if (!EnoughMoneyToTreat() || !EnoughTimeToTreat()) {
				m_tTaskState = eTaskStateSendSOS;
				return;
			}
			else {
				m_tTaskState = eTaskStateBuySupplies;
				return;
			}
		}
		else {
			m_tTaskState = eTaskStateHealthCare;
			return;
		}
	}
	else {
		if (!EnoughEquipmentToGo()) {
			if (DistanceToTraderIsDanger() || !EnoughMoneyToEquip()) {
				m_tTaskState = eTaskStateSendSOS;
				return;
			}
			else {
				m_tTaskState = eTaskStateBuySupplies;
				return;
			}
		}
		else {
			_GRAPH_ID				tGraphID = _GRAPH_ID(-1);
			vfChooseHumanTask		(tpALife);
			CSE_ALifeTask			*l_tpTask = tpALife->tpfGetTaskByID(m_dwCurTaskID);
			switch (l_tpTask->m_tTaskType) {
				case eTaskTypeSearchForItemCG :
				case eTaskTypeSearchForItemOG : {
					tGraphID		= l_tpTask->m_tGraphID;
					break;
				}
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemOL : {
#pragma todo("Dima to Dima : add graph point type item search")
					//m_baVisitedVertices.assign(tpALife->m_tpTerrain[l_tpTask->m_tLocationID].size(),false);
					//tGraphID		= tpALife->m_tpTerrain[l_tpTask->m_tLocationID][m_dwCurTaskLocation = 0];
					//m_baVisitedVertices[m_dwCurTaskLocation] = true;
					break;
				}
				default				: NODEFAULT;
			};
			m_tTaskState			= eTaskStateAccomplishTask;
			tpALife->ffFindMinimalPath(m_tGraphID,tGraphID,m_tpaVertices);
			m_dwCurNode				= 0;
			m_tNextGraphID			= m_tGraphID;
			m_fCurSpeed				= m_fGoingSpeed;
		}
	}
}

void CSE_ALifeHumanAbstract::vfHealthCare(CSE_ALifeSimulator *tpALife)
{
	// if health is low -> use medikit
	// if hunhry -> eat or drink vodka
	// if irradiating -> use antirad or drink vodka 
	// if tired -> sleep, take a rest
	m_tTaskState = eTaskStateChooseTask;
}

void CSE_ALifeHumanAbstract::vfBuySupplies(CSE_ALifeSimulator *tpALife)
{
	// choose an appropriate trader and go to him to buy supplies
	m_tTaskState = eTaskStateChooseTask;
}

void CSE_ALifeHumanAbstract::vfGoToCustomer(CSE_ALifeSimulator *tpALife)
{
	// go to customer to get something to accomplish task
	if ((m_dwCurNode >= (m_tpaVertices.size() - 1)) && (m_tGraphID == m_tNextGraphID)) {
		m_tpaVertices.clear();
		m_dwCurNode = 0;
		if (int(m_dwCurTaskID) > 0) {
			CSE_ALifeTask			*l_tpTask = tpALife->tpfGetTaskByID(m_dwCurTaskID);
			CSE_ALifeTraderAbstract	*l_tpTraderAbstract = dynamic_cast<CSE_ALifeTraderAbstract*>(tpALife->tpfGetObjectByID(l_tpTask->m_tCustomerID));
			if (l_tpTraderAbstract)
				tpALife->vfCommunicateWithCustomer(this,l_tpTraderAbstract);
		}
		m_tTaskState = eTaskStateChooseTask;
	}
}

void CSE_ALifeHumanAbstract::vfBringToCustomer(CSE_ALifeSimulator *tpALife)
{
	// go to customer to sell found artefacts
}

void CSE_ALifeHumanAbstract::vfGoToSOS(CSE_ALifeSimulator *tpALife)
{
	// go to person who sent SOS to save or kill
}

void CSE_ALifeHumanAbstract::vfSendSOS(CSE_ALifeSimulator *tpALife)
{
	// send SOS by myself
}

void CSE_ALifeHumanAbstract::vfAccomplishTask(CSE_ALifeSimulator *tpALife)
{
	// build path and wait until we go to the end of it
	if ((m_dwCurNode + 1 >= (m_tpaVertices.size())) && (m_tGraphID == m_tNextGraphID)) {
		if (bfCheckIfTaskCompleted(tpALife)) {
			tpALife->ffFindMinimalPath	(m_tGraphID,tpALife->tpfGetObjectByID(tpALife->tpfGetTaskByID(m_dwCurTaskID)->m_tCustomerID)->m_tGraphID,m_tpaVertices);
			m_dwCurNode		= 0;
			m_tTaskState	= eTaskStateGoToCustomer;
		}
		else {
			switch (tpALife->tpfGetTaskByID(m_dwCurTaskID)->m_tTaskType) {
				case eTaskTypeSearchForItemCG :
				case eTaskTypeSearchForItemOG : {
					if ((m_dwCurNode + 1>= (m_tpaVertices.size())) && (m_tGraphID == m_tNextGraphID)) {
						tpALife->tpfGetTaskByID	(m_dwCurTaskID)->m_dwTryCount++;
						m_tTaskState = eTaskStateChooseTask;
					}
					break;
				}
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemOL : {
	//				tpALife->tpfGetTaskByID(m_dwCurTaskID)->m_dwTryCount++;
	//				m_tTaskState		= eTaskStateChooseTask;
	//				for (m_dwCurTaskLocation++; (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) && (m_baVisitedVertices[m_dwCurTaskLocation]); m_dwCurTaskLocation++);
	//				if (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) {
	//					m_baVisitedVertices[m_dwCurTaskLocation] = true;
	//					tpAlife->ffFindMinimalPath(m_tGraphID,tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID][m_dwCurTaskLocation],m_tpaVertices);
	//					m_dwCurNode = 0;
	//				}
	//				else
	//					m_tTaskState	= eTaskStateChooseTask;
					break;
				}
				default :				NODEFAULT;
			}
		}
	}
}

void CSE_ALifeHumanAbstract::vfSearchObject(CSE_ALifeSimulator *tpALife)
{
}