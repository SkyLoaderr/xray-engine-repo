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

void CSE_ALifeSimulator::vfCheckForTheInteraction(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract)
{
	_GRAPH_ID					l_tGraphID = tpALifeMonsterAbstract->m_tGraphID;
	vfCheckForTheInteraction	(tpALifeMonsterAbstract,l_tGraphID);
	CSE_ALifeGraph::SGraphEdge	*I = (CSE_ALifeGraph::SGraphEdge *)((u8 *)getAI().m_tpaGraph + getAI().m_tpaGraph[l_tGraphID].dwEdgeOffset);
	CSE_ALifeGraph::SGraphEdge	*E = I + (u32)getAI().m_tpaGraph[l_tGraphID].tNeighbourCount;
	for ( ; I != E; I++)
		vfCheckForTheInteraction(tpALifeMonsterAbstract,(_GRAPH_ID)(*I).dwVertexNumber);
}

void CSE_ALifeSimulator::vfCheckForTheInteraction(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract, _GRAPH_ID tGraphID)
{
	CSE_ALifeHumanAbstract		*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>(tpALifeMonsterAbstract);
	D_OBJECT_P_IT				I = m_tpGraphObjects[tGraphID].tpObjects.begin();
	D_OBJECT_P_IT				E = m_tpGraphObjects[tGraphID].tpObjects.end();
	int							l_iGroupIndex;
	for ( ; I != E; I++) {
		CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(*I);
		if (l_tpALifeMonsterAbstract) {
			if (bfCheckForCombat(tpALifeMonsterAbstract,l_tpALifeMonsterAbstract,l_iGroupIndex)) {
				vfFillCombatGroup		(tpALifeMonsterAbstract,m_tpaCombatGroups[0]);
				vfFillCombatGroup		(l_tpALifeMonsterAbstract,m_tpaCombatGroups[1]);
				ECombatResult			l_tCombatResult = eCombatResultRetreat12;
				for (int i=0; i<2*int(m_dwMaxCombatIterationCount); i++) {
					if (tfChooseCombatAction(l_iGroupIndex) == eCombatActionAttack)
						vfPerformAttackAction(l_iGroupIndex);
					else
						if (bfCheckIfRetreated(l_iGroupIndex)) {
							l_tCombatResult	= l_iGroupIndex ? eCombatResultRetreat2 : eCombatResultRetreat1;
							break;
						}
					
					l_iGroupIndex	^= 1;
					if (m_tpaCombatGroups[l_iGroupIndex].empty()) {
						l_tCombatResult	= l_iGroupIndex ? eCombatResult2Kill1 : eCombatResult1Kill2;
						break;
					}
				}
				vfFinishCombat			(l_tCombatResult);
			}
			else {
				if (l_tpALifeHumanAbstract)	{
					CSE_ALifeHumanAbstract		*l_tpALifeHumanAbstract2 = dynamic_cast<CSE_ALifeHumanAbstract*>(l_tpALifeMonsterAbstract);
					if (l_tpALifeHumanAbstract2)
						vfPerformCommunication(l_tpALifeHumanAbstract,l_tpALifeHumanAbstract2);
				}
			}
		}
	}
}

void CSE_ALifeSimulator::vfPerformCommunication(CSE_ALifeHumanAbstract *tpALifeHumanAbstract1, CSE_ALifeHumanAbstract *tpALifeHumanAbstract2)
{
#pragma todo("Dima to Dima: Append communication")
}