////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_human.cpp
//	Created 	: 24.07.2003
//  Modified 	: 24.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation of humans
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"

bool CSE_ALifeHumanAbstract::bfCheckIfTaskCompleted(CSE_ALifeSimulator *tpALife, OBJECT_IT &I)
{
	if (int(m_dwCurTaskID) < 0)
		return		(false);
	I				= children.begin();
	OBJECT_IT		E = children.end();
	CSE_ALifeTask	&tTask = *tpALife->tpfGetTaskByID(m_dwCurTaskID);
	for ( ; I != E; I++) {
		switch (tTask.m_tTaskType) {
			case eTaskTypeSearchForItemCL :
			case eTaskTypeSearchForItemCG : {
				if (!strcmp(tpALife->tpfGetObjectByID(*I)->s_name,tTask.m_caSection))
					return(true);
				break;
											}
			case eTaskTypeSearchForItemOL :
			case eTaskTypeSearchForItemOG : {
				if (tpALife->tpfGetObjectByID(*I)->ID == tTask.m_tObjectID)
					return(true);
				break;
											}
		}
	}
	return			(false);
}

bool CSE_ALifeHumanAbstract::bfCheckIfTaskCompleted	(CSE_ALifeSimulator	*tpALife)
{
	OBJECT_IT					I;
	return						(bfCheckIfTaskCompleted(tpALife,I));
}

void CSE_ALifeHumanAbstract::vfSetCurrentTask(CSE_ALifeSimulator	*tpALife, _TASK_ID &tTaskID)
{
	m_dwCurTaskID				= tpALife->tpfGetTaskByID(tTaskID)->m_tTaskID;
}

bool CSE_ALifeHumanAbstract::bfChooseNextRoutePoint(CSE_ALifeSimulator *tpALife)
{
	bool			bOk = false;
	if (m_tTaskState != eTaskStateSearchItem) {
		if (m_tNextGraphID != m_tGraphID) {
			_TIME_ID				tCurTime = tpALife->tfGetGameTime();
			m_fDistanceFromPoint	+= float(tCurTime - m_tTimeID)/1000.f*m_fCurSpeed;
			if (m_fDistanceToPoint - m_fDistanceFromPoint < EPS_L) {
				bOk = true;
				if ((m_fDistanceFromPoint - m_fDistanceToPoint > EPS_L) && (m_fCurSpeed > EPS_L))
					m_tTimeID		= tCurTime - _TIME_ID(iFloor((m_fDistanceFromPoint - m_fDistanceToPoint)*1000.f/m_fCurSpeed));
				m_fDistanceToPoint	= m_fDistanceFromPoint	= 0.0f;
				m_tPrevGraphID		= m_tGraphID;
				tpALife->vfChangeObjectGraphPoint(this,m_tGraphID,m_tNextGraphID);
			}
		}
		else {
			if (++m_dwCurNode < m_tpPath.size()) {
				m_tNextGraphID		= _GRAPH_ID(m_tpPath[m_dwCurNode]);
				m_fDistanceToPoint	= getAI().ffGetDistanceBetweenGraphPoints(m_tGraphID,m_tNextGraphID);
				bOk = true;
			}
			else
				m_fCurSpeed	= 0.f;
		}
	}
	else {
	}
	return			(bOk);
}

bool CSE_ALifeHumanAbstract::bfProcessItems(CSE_ALifeSimulator *tpALife)
{
	bool						bOk = false;
	for (int I=0; I<(int)tpALife->m_tpGraphObjects[m_tGraphID].tpObjects.size(); I++) {
		u16						wID = tpALife->m_tpGraphObjects[m_tGraphID].tpObjects[I]->ID;
		CSE_ALifeDynamicObject	*tpALifeDynamicObject = tpALife->tpfGetObjectByID(wID);
		CSE_ALifeItem			*tpALifeItem = dynamic_cast<CSE_ALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem && !tpALifeItem->m_bOnline) {
			// adding _new item to the item list
			if (m_fCumulativeItemMass + tpALifeItem->m_fMass < m_fMaxItemMass) {
				if (tpALife->randF(1.0f) < m_fProbability) {
					tpALife->vfAttachItem(*this,tpALifeItem,m_tGraphID);
					bOk = true;
					I--;
					continue;
				}
			}
			else {
				std::sort(children.begin(),children.end(),CSortItemPredicate(tpALife->m_tObjectRegistry));
				float			fItemMass = m_fCumulativeItemMass;
				u32				dwCount = (u32)children.size();
				int				i;
				for ( i=(int)dwCount - 1; i>=0; i--) {
					CSE_ALifeItem	*tpALifeItemIn = dynamic_cast<CSE_ALifeItem *>(tpALife->tpfGetObjectByID(children[i]));
					R_ASSERT	(tpALifeItemIn);
					m_fCumulativeItemMass -= tpALifeItemIn->m_fMass;
					if (float(tpALifeItemIn->m_dwCost)/tpALifeItemIn->m_fMass >= float(tpALifeItemIn->m_dwCost)/tpALifeItem->m_fMass)
						break;
					if (m_fCumulativeItemMass + tpALifeItem->m_fMass < m_fMaxItemMass)
						break;
				}
				if (m_fCumulativeItemMass + tpALifeItem->m_fMass < m_fMaxItemMass) {
					for (int j=i + 1 ; j < (int)dwCount; j++)
						tpALife->vfDetachItem(*this,tpALifeItem,m_tGraphID);
					tpALife->vfAttachItem(*this,tpALifeItem,m_tGraphID);
					bOk = true;
					I--;
					continue;
				}
				else
					m_fCumulativeItemMass	= fItemMass;
			}
		}
	}
	return			(bOk);
}

void CSE_ALifeHumanAbstract::vfCheckForDeletedEvents(CSE_ALifeSimulator	*tpALife)
{
	PERSONAL_EVENT_P_IT I = std::remove_if(m_tpEvents.begin(),m_tpEvents.end(),CRemovePersonalEventPredicate(tpALife->m_tEventRegistry));
	m_tpEvents.erase(I,m_tpEvents.end());
}

void CSE_ALifeHumanAbstract::vfChooseHumanTask(CSE_ALifeSimulator *tpALife)
{
	OBJECT_IT					I = m_tpKnownCustomers.begin();
	OBJECT_IT					E = m_tpKnownCustomers.end();
	for ( ; I != E; I++) {
		OBJECT_TASK_PAIR_IT		J = tpALife->m_tTaskCrossMap.find(*I);
		R_ASSERT2				(J != tpALife->m_tTaskCrossMap.end(),"Can't find a specified customer in the Task registry!");
		
		TASK_SET_IT				i = (*J).second.begin();
		TASK_SET_IT				e = (*J).second.end();
		for ( ; i != e; i++)
			if (!tpALife->tpfGetTaskByID(*i)->m_dwTryCount)
				break;
		
		if (i != e) {
			vfSetCurrentTask	(tpALife,*i);
			break;
		}
	}
}

void CSE_ALifeSimulator::vfCommunicateWithCustomer(CSE_ALifeHumanAbstract *tpALifeHumanAbstract, CSE_ALifeTraderAbstract *tpTraderAbstract)
{
	// update items
	if (tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID,true)) {
		OBJECT_IT								I;
		if (tpALifeHumanAbstract->bfCheckIfTaskCompleted(this, I)) {
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