////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_human.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation of humans
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"

CSE_ALifeTrader *CSE_ALifeSimulator::tpfGetNearestSuitableTrader(CSE_ALifeHumanAbstract *tpALifeHuman)
{
	float			fBestDistance = MAX_NODE_ESTIMATION_COST;
	CSE_ALifeTrader *tpBestTrader = 0;
	TRADER_P_IT		I = m_tpTraders.begin();
	TRADER_P_IT		E = m_tpTraders.end();
	Fvector			&tGlobalPoint = getAI().m_tpaGraph[tpALifeHuman->m_tGraphID].tGlobalPoint;
	for ( ; I != E; I++) {
		if ((*I)->m_tRank != tpALifeHuman->m_tRank)
			break;
		float		fCurDistance = getAI().m_tpaGraph[(*I)->m_tGraphID].tGlobalPoint.distance_to(tGlobalPoint);
		if (fCurDistance < fBestDistance) {
			fBestDistance = fCurDistance;
			tpBestTrader = *I;
		}
	}
	return			(tpBestTrader);
}

bool CSE_ALifeSimulator::bfChooseNextRoutePoint(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	bool				bOk = false;
	if (tpALifeHumanAbstract->m_tTaskState != eTaskStateSearchItem) {
		if (tpALifeHumanAbstract->m_tNextGraphID != tpALifeHumanAbstract->m_tGraphID) {
			_TIME_ID										tCurTime = tfGetGameTime();
			tpALifeHumanAbstract->m_fDistanceFromPoint	+= float(tCurTime - tpALifeHumanAbstract->m_tTimeID)/1000.f * tpALifeHumanAbstract->m_fCurSpeed;
			if (tpALifeHumanAbstract->m_fDistanceToPoint - tpALifeHumanAbstract->m_fDistanceFromPoint < EPS_L) {
				bOk = true;
				if ((tpALifeHumanAbstract->m_fDistanceFromPoint - tpALifeHumanAbstract->m_fDistanceToPoint > EPS_L) && (tpALifeHumanAbstract->m_fCurSpeed > EPS_L))
					tpALifeHumanAbstract->m_tTimeID			= tCurTime - _TIME_ID(iFloor((tpALifeHumanAbstract->m_fDistanceFromPoint - tpALifeHumanAbstract->m_fDistanceToPoint)*1000.f/tpALifeHumanAbstract->m_fCurSpeed));
				tpALifeHumanAbstract->m_fDistanceToPoint	= tpALifeHumanAbstract->m_fDistanceFromPoint	= 0.0f;
				tpALifeHumanAbstract->m_tPrevGraphID		= tpALifeHumanAbstract->m_tGraphID;
				vfChangeObjectGraphPoint(tpALifeHumanAbstract,tpALifeHumanAbstract->m_tGraphID,tpALifeHumanAbstract->m_tNextGraphID);
			}
		}
		else {
			if (++tpALifeHumanAbstract->m_dwCurNode < tpALifeHumanAbstract->m_tpaVertices.size()) {
				tpALifeHumanAbstract->m_tNextGraphID		= _GRAPH_ID(tpALifeHumanAbstract->m_tpaVertices[tpALifeHumanAbstract->m_dwCurNode]);
				tpALifeHumanAbstract->m_fDistanceToPoint	= getAI().ffGetDistanceBetweenGraphPoints(tpALifeHumanAbstract->m_tGraphID,tpALifeHumanAbstract->m_tNextGraphID);
				bOk = true;
			}
			else
				tpALifeHumanAbstract->m_fCurSpeed	= 0.f;
		}
	}
	else {
	}
	return				(bOk);
}

bool CSE_ALifeSimulator::bfCheckForItems(CSE_ALifeHumanAbstract	*tpALifeHumanAbstract)
{
	CSE_ALifeHumanAbstract *tpALifeHuman = dynamic_cast<CSE_ALifeHumanAbstract *>(tpALifeHumanAbstract);
	if (tpALifeHuman)
		return(bfProcessItems(*tpALifeHuman,tpALifeHuman->m_tGraphID,tpALifeHuman->m_fMaxItemMass,1.f));//tpALifeHuman->m_tTaskState == eTaskStateSearching ? .25f : .05f));
	else {
		//		CSE_ALifeHumanGroup *tpALifeHumanGroup = dynamic_cast<CSE_ALifeHumanGroup *>(tpALifeHumanAbstract);
		//		VERIFY(tpALifeHumanGroup);
		//		HUMAN_PARAMS_P_IT	I = tpALifeHumanGroup->m_tpMembers.begin();
		//		HUMAN_PARAMS_P_IT	E = tpALifeHumanGroup->m_tpMembers.end();
		//		bool bOk = false;
		//		for ( ; I != E; I++)
		//			bOk = bOk || bfProcessItems(*(*I),tpALifeHumanGroup->m_tGraphID,tpALifeHumanGroup->m_fMaxItemMass,tpALifeHumanGroup->m_tTaskState == eTaskStateSearching ? .75f : .1f);
		//		return(bOk);
		return(false);
	}
}

bool CSE_ALifeSimulator::bfProcessItems(CSE_Abstract &CSE_Abstract, _GRAPH_ID tGraphID, float fMaxItemMass, float fProbability)
{
	CSE_ALifeTraderAbstract *tpALifeTraderParams = dynamic_cast<CSE_ALifeTraderAbstract*>(&CSE_Abstract);
	VERIFY(tpALifeTraderParams);
	//DYNAMIC_OBJECT_P_IT		I = m_tpGraphObjects[tGraphID].tpObjects.begin();
	//DYNAMIC_OBJECT_P_IT		E = m_tpGraphObjects[tGraphID].tpObjects.end();
	bool bOk = false;
	for (int I=0; I<(int)m_tpGraphObjects[tGraphID].tpObjects.size(); I++) {
		u16 wID = m_tpGraphObjects[tGraphID].tpObjects[I]->ID;
		CSE_ALifeDynamicObject *tpALifeDynamicObject = tpfGetObjectByID(wID);
		CSE_ALifeItem *tpALifeItem = dynamic_cast<CSE_ALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem && !tpALifeItem->m_bOnline) {
			// adding _new item to the item list
			if (tpALifeTraderParams->m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
				if (randF(1.0f) < fProbability) {
					vfAttachItem(CSE_Abstract,tpALifeItem,tGraphID);
					bOk = true;
					I--;
					continue;
				}
			}
			else {
				std::sort(CSE_Abstract.children.begin(),CSE_Abstract.children.end(),CSortItemPredicate(m_tObjectRegistry));
				float		fItemMass = tpALifeTraderParams->m_fCumulativeItemMass;
				u32			dwCount = (u32)CSE_Abstract.children.size();
				int			i;
				for ( i=(int)dwCount - 1; i>=0; i--) {
					CSE_ALifeItem *tpALifeItemIn = dynamic_cast<CSE_ALifeItem *>(tpfGetObjectByID(CSE_Abstract.children[i]));
					VERIFY(tpALifeItemIn);
					tpALifeTraderParams->m_fCumulativeItemMass -= tpALifeItemIn->m_fMass;
					if (float(tpALifeItemIn->m_dwCost)/tpALifeItemIn->m_fMass >= float(tpALifeItemIn->m_dwCost)/tpALifeItem->m_fMass)
						break;
					if (tpALifeTraderParams->m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass)
						break;
				}
				if (tpALifeTraderParams->m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
					for (int j=i + 1 ; j < (int)dwCount; j++)
						vfDetachItem(CSE_Abstract,tpALifeItem,tGraphID);
					vfAttachItem(CSE_Abstract,tpALifeItem,tGraphID);
					//return(true);
					bOk = true;
					I--;
					continue;
				}
				else
					tpALifeTraderParams->m_fCumulativeItemMass	= fItemMass;
			}
		}
	}
	return(bOk);
}

bool CSE_ALifeSimulator::bfCheckIfTaskCompleted(CSE_Abstract &CSE_Abstract, CSE_ALifeHumanAbstract *tpALifeHumanAbstract, OBJECT_IT &I)
{
	if (int(tpALifeHumanAbstract->m_dwCurTaskID) < 0)
		return(false);
	I				= CSE_Abstract.children.begin();
	OBJECT_IT		E = CSE_Abstract.children.end();
	CSE_ALifeTask	&tTask = *tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID);
	for ( ; I != E; I++) {
		switch (tTask.m_tTaskType) {
			case eTaskTypeSearchForItemCL :
			case eTaskTypeSearchForItemCG : {
				if (!strcmp(tpfGetObjectByID(*I)->s_name,tTask.m_caSection))
					return(true);
				break;
											}
			case eTaskTypeSearchForItemOL :
			case eTaskTypeSearchForItemOG : {
				if (tpfGetObjectByID(*I)->ID == tTask.m_tObjectID)
					return(true);
				break;
											}
		}
	}
	return(false);
}

void CSE_ALifeSimulator::vfCheckForDeletedEvents(CSE_ALifeHumanAbstract	*tpALifeHuman)
{
	PERSONAL_EVENT_P_IT I = std::remove_if(tpALifeHuman->m_tpEvents.begin(),tpALifeHuman->m_tpEvents.end(),CRemovePersonalEventPredicate(m_tEventRegistry));
	tpALifeHuman->m_tpEvents.erase(I,tpALifeHuman->m_tpEvents.end());
}

void CSE_ALifeSimulator::vfChooseHumanTask(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	OBJECT_IT					I = tpALifeHumanAbstract->m_tpKnownCustomers.begin();
	OBJECT_IT					E = tpALifeHumanAbstract->m_tpKnownCustomers.end();
	for ( ; I != E; I++) {
		OBJECT_TASK_PAIR_IT		J = m_tTaskCrossMap.find(*I);
		R_ASSERT2				(J != m_tTaskCrossMap.end(),"Can't find a specified customer in the Task registry!");
		
		TASK_SET_IT				i = (*J).second.begin();
		TASK_SET_IT				e = (*J).second.end();
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

void CSE_ALifeSimulator::vfCommunicateWithCustomer(CSE_ALifeHumanAbstract *tpALifeHumanAbstract, CSE_ALifeTraderAbstract *tpTraderAbstract)
{
	// update items
	if (tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID,true)) {
		OBJECT_IT								I;
		if (bfCheckIfTaskCompleted(tpALifeHumanAbstract, I)) {
			D_OBJECT_PAIR_IT						J = m_tObjectRegistry.find(*I);
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
	R_ASSERT3			(!tpALifeHumanAbstract->m_bOnline,"Can't update online object ",tpALifeHumanAbstract->s_name_replace);
	bool				bOk;
	do {
		switch (tpALifeHumanAbstract->m_tTaskState) {
			case eTaskStateChooseTask : {
				vfChooseTask(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateHealthCare : {
				vfHealthCare(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateBuySupplies : {
				vfBuySupplies(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateGoToCustomer : {
				vfGoToCustomer(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateBringToCustomer : {
				vfBringToCustomer(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateGoToSOS : {
				vfGoToSOS(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateSendSOS : {
				vfSendSOS(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateAccomplishTask : {
				vfAccomplishTask(tpALifeHumanAbstract);
				break;
			}
			case eTaskStateSearchItem : {
				vfSearchObject	(tpALifeHumanAbstract);
				break;
			}
			default				: NODEFAULT;
		}
		bOk						= bfChooseNextRoutePoint(tpALifeHumanAbstract);
		vfChooseNextRoutePoint	(tpALifeHumanAbstract);
		vfCheckForTheBattle		(tpALifeHumanAbstract);
		bfCheckForItems			(tpALifeHumanAbstract);
		//vfCheckForDeletedEvents	(tpALifeHumanAbstract);
	}
	while (bOk && (m_tpActor->o_Position.distance_to(tpALifeHumanAbstract->o_Position) > m_fOnlineDistance));
	tpALifeHumanAbstract->m_tTimeID	= tfGetGameTime();
}