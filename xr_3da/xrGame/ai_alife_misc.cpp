////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_misc.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"

void CSE_ALifeSimulator::vfCheckForInteraction(CSE_ALifeSchedulable *tpALifeSchedulable)
{
	if (!tpALifeSchedulable->bfActive())
		return;
	CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeSchedulable);
	R_ASSERT2					(l_tpALifeDynamicObject,"Unknown schedulable object class");
	_GRAPH_ID					l_tGraphID = l_tpALifeDynamicObject->m_tGraphID;
	vfCheckForInteraction		(tpALifeSchedulable,l_tGraphID);
	CSE_ALifeGraph::SGraphEdge	*I = (CSE_ALifeGraph::SGraphEdge *)((u8 *)getAI().m_tpaGraph + getAI().m_tpaGraph[l_tGraphID].dwEdgeOffset);
	CSE_ALifeGraph::SGraphEdge	*E = I + (u32)getAI().m_tpaGraph[l_tGraphID].tNeighbourCount;
	for ( ; I != E; I++)
		vfCheckForInteraction	(tpALifeSchedulable,(_GRAPH_ID)(*I).dwVertexNumber);
}

void CSE_ALifeSimulator::vfCheckForInteraction(CSE_ALifeSchedulable *tpALifeSchedulable, _GRAPH_ID tGraphID)
{
	CSE_ALifeHumanAbstract		*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>(tpALifeSchedulable);
	D_OBJECT_P_IT				I = m_tpGraphObjects[tGraphID].tpObjects.begin();
	D_OBJECT_P_IT				E = m_tpGraphObjects[tGraphID].tpObjects.end();
	int							l_iGroupIndex;
	bool						l_bFirstTime = true;
	bool						l_bMutualDetection;
	for ( ; I != E; I++) {
		if ((*I)->ID == tpALifeSchedulable->ID)
			continue;

		CSE_ALifeSchedulable	*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeSchedulable*>(*I);
		if (!l_tpALifeSchedulable)
			continue;

		if (!bfCheckForInteraction(tpALifeSchedulable,l_tpALifeSchedulable,l_iGroupIndex,l_bMutualDetection))
			continue;

		if (l_bFirstTime) {
			vfFillCombatGroup	(tpALifeSchedulable,0);
			l_bFirstTime		= false;
		}
		
		vfFillCombatGroup		(l_tpALifeSchedulable,1);

		switch (m_tpaCombatObjects[l_iGroupIndex]->tfGetActionType(m_tpaCombatObjects[l_iGroupIndex ^ 1],l_iGroupIndex,l_bMutualDetection)) {
			case eMeetActionTypeAttack : {
#ifdef OFFLINE_LOG
				Msg("[LSS] %s started combat versus %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
#endif
				ECombatResult			l_tCombatResult = eCombatResultRetreat12;
				for (int i=0; i<2*int(m_dwMaxCombatIterationCount); i++) {
					if (tfChooseCombatAction(l_iGroupIndex) == eCombatActionAttack) {
#ifdef OFFLINE_LOG
						Msg("[LSS] %s choosed to attack %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
#endif
						vfPerformAttackAction(l_iGroupIndex);
					}
					else {
#ifdef OFFLINE_LOG
						Msg("[LSS] %s choosed to retreat from %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
#endif
						if (bfCheckIfRetreated(l_iGroupIndex)) {
#ifdef OFFLINE_LOG
							Msg("[LSS] %s did retreat from %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
#endif
							l_tCombatResult	= l_iGroupIndex ? eCombatResultRetreat2 : eCombatResultRetreat1;
							break;
						}
#ifdef OFFLINE_LOG
						Msg("[LSS] %s didn't retreat from %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
#endif
					}

					l_iGroupIndex		^= 1;

					if (m_tpaCombatGroups[l_iGroupIndex].empty()) {
#ifdef OFFLINE_LOG
						Msg("[LSS] %s is dead",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace);
#endif
						l_tCombatResult	= l_iGroupIndex ? eCombatResult2Kill1 : eCombatResult1Kill2;
						break;
					}
				}
				vfFinishCombat			(l_tCombatResult);
				break;
			}
			case eMeetActionTypeInteract : {
				R_ASSERT2				(l_tpALifeHumanAbstract,"Non-human objects ñannot communicate with each other");
				CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract2 = dynamic_cast<CSE_ALifeHumanAbstract*>(l_tpALifeSchedulable);
				R_ASSERT2				(l_tpALifeHumanAbstract2,"Non-human objects ñannot communicate with each other");
#ifdef OFFLINE_LOG
				Msg						("[LSS] %s interacted with ",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
#endif
				vfPerformCommunication	(l_tpALifeHumanAbstract,l_tpALifeHumanAbstract2);
				break;
			}
			case eMeetActionTypeIgnore : {
#ifdef OFFLINE_LOG
				Msg("[LSS] %s refused from combat",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace);
#endif
				continue;
			}
			default : NODEFAULT;
		}
		if (m_tpaCombatGroups[0].empty())
			break;
	}
}

void CSE_ALifeSimulator::vfPerformCommunication(CSE_ALifeHumanAbstract *tpALifeHumanAbstract1, CSE_ALifeHumanAbstract *tpALifeHumanAbstract2)
{
#pragma todo("Dima to Dima: Append communication")
}

void CSE_ALifeSimulator::vfCommunicateWithCustomer(CSE_ALifeHumanAbstract *tpALifeHumanAbstract, CSE_ALifeTraderAbstract *tpTraderAbstract)
{
	// update items
	if (tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID,true)) {
		OBJECT_IT								I;
		if (tpALifeHumanAbstract->bfCheckIfTaskCompleted(I)) {
			D_OBJECT_PAIR_IT					J = m_tObjectRegistry.find(*I);
			R_ASSERT2							(J != m_tObjectRegistry.end(), "Specified object hasn't been found in the Object registry!");
			CSE_ALifeItem						*tpALifeItem = dynamic_cast<CSE_ALifeItem *>((*J).second);
			if (tpTraderAbstract->m_dwMoney >= tpALifeItem->m_dwCost) {
				vfDetachItem					(*tpALifeHumanAbstract,tpALifeItem,tpALifeHumanAbstract->m_tGraphID);
				vfAttachItem					(*tpTraderAbstract,tpALifeItem,tpALifeHumanAbstract->m_tGraphID);
				// paying/receiving money
				tpTraderAbstract->m_dwMoney		-= tpALifeItem->m_dwCost;
				tpALifeHumanAbstract->m_dwMoney += tpALifeItem->m_dwCost;
			}
		}
	}
	// update events
#pragma todo("Dima to Dima: Update events")
}

ERelationType CSE_ALifeSimulator::tfGetRelationType(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract1, CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract2)
{
	if (tpALifeMonsterAbstract1->g_team() != tpALifeMonsterAbstract2->g_team())
		return(eRelationTypeEnemy);
	else
		return(eRelationTypeFriend);
}
