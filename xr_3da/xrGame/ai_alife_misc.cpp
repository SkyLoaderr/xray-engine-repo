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

void CSE_ALifeSimulator::vfFillBattleGroup(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract, MONSTER_P_VECTOR &tpGroupVector)
{
	CSE_ALifeAbstractGroup	*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(tpALifeMonsterAbstract);
	if (l_tpALifeAbstractGroup) {
		OBJECT_IT			I = l_tpALifeAbstractGroup->m_tpMembers.begin();
		OBJECT_IT			E = l_tpALifeAbstractGroup->m_tpMembers.end();
		for ( ; I != E; I++) {
			CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpfGetObjectByID(*I));
			R_ASSERT2		(l_tpALifeMonsterAbstract,"Invalid group member!");
			tpGroupVector.push_back(l_tpALifeMonsterAbstract);
		}
	}
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
			if (bfCheckForBattle(tpALifeMonsterAbstract,l_tpALifeMonsterAbstract,l_iGroupIndex)) {
				vfFillBattleGroup		(tpALifeMonsterAbstract,m_tpGroupVector[0]);
				vfFillBattleGroup		(l_tpALifeMonsterAbstract,m_tpGroupVector[1]);
				EBattleResult			l_tBattleResult = eBattleResultRetreat12;
				for (int i=0; i<2*int(m_dwMaxBattleIterationCount); i++) {
					if (tfChooseBattleAction(l_iGroupIndex) == eBattleActionAttack)
						vfPerformAttackAction(l_iGroupIndex);
					else
						if (bfCheckIfRetreated(l_iGroupIndex)) {
							l_tBattleResult	= l_iGroupIndex ? eBattleResultRetreat2 : eBattleResultRetreat1;
							break;
						}
					
					l_iGroupIndex	^= 1;
					vfUpdateGroupMembers(l_iGroupIndex);
					if (m_tpGroupVector[l_iGroupIndex].empty()) {
						l_tBattleResult	= l_iGroupIndex ? eBattleResult2Kill1 : eBattleResult1Kill2;
						break;
					}
				}
				vfFinishBattle			(l_tBattleResult);
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

void CSE_ALifeSimulator::vfUpdateGroupMembers(int iGroupIndex)
{
	MONSTER_P_VECTOR			&l_tpGroupVector = m_tpGroupVector[iGroupIndex];
	for (int i=0, n=l_tpGroupVector.size(); i<n; i++)
		if (l_tpGroupVector[i]->fHealth <= 0) {
			l_tpGroupVector.erase(l_tpGroupVector.begin() + i);
			i--;
			n--;
		}
}

bool CSE_ALifeSimulator::bfCheckForBattle(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract1, CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract2, int &iGroupIndex)
{
#pragma todo("Dima to Dima: Append relationships and monster noticability")
#pragma todo("Dima to Dima: Monster can't notice another monster if it is asleep")
	iGroupIndex					= 0;
	if ((tpALifeMonsterAbstract1->g_team() != tpALifeMonsterAbstract2->g_team()) && randI(2))
		return(true);
	else
		return(false);
}

EBattleAction CSE_ALifeSimulator::tfChooseBattleAction(int iGroupIndex)
{
#pragma todo("Dima to Dima: Append correct battle actions choice")
	return					(eBattleActionAttack);
}

void CSE_ALifeSimulator::vfPerformAttackAction(int iGroupIndex)
{
#pragma todo("Dima to Dima: Append attack actions")
}

bool CSE_ALifeSimulator::bfCheckIfRetreated(int iGroupIndex)
{
#pragma todo("Dima to Dima: Append retreat check")
	return(false);
}

void CSE_ALifeSimulator::vfPerformCommunication(CSE_ALifeHumanAbstract *tpALifeHumanAbstract1, CSE_ALifeHumanAbstract *tpALifeHumanAbstract2)
{
#pragma todo("Dima to Dima: Append communication")
}

void CSE_ALifeSimulator::vfFinishBattle(EBattleResult tBattleResult)
{
#pragma todo("Dima to Dima: Append finish battle actions")
}