////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_misc.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation : miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"
#include "game_graph.h"

void CSE_ALifeSimulator::vfCheckForInteraction(CSE_ALifeSchedulable *tpALifeSchedulable)
{
	if (!tpALifeSchedulable->bfActive())
		return;
	CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeSchedulable);
	R_ASSERT2					(l_tpALifeDynamicObject,"Unknown schedulable object class");
	_GRAPH_ID					l_tGraphID = l_tpALifeDynamicObject->m_tGraphID;
	vfCheckForInteraction		(tpALifeSchedulable,l_tGraphID);
	CGameGraph::const_iterator	I, E;
	ai().game_graph().begin		(l_tGraphID,I,E);
	for ( ; I != E; ++I)
		vfCheckForInteraction	(tpALifeSchedulable,(*I).vertex_id());
}

void CSE_ALifeSimulator::vfCheckForInteraction(CSE_ALifeSchedulable *tpALifeSchedulable, _GRAPH_ID tGraphID)
{
	CSE_ALifeHumanAbstract		*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>(tpALifeSchedulable);
#pragma todo("Dima to Dima : Instead of copying map to map we have to implement more complex and efficient data processing while adding and removing objects to/from graph point")
	m_tpGraphPointObjects		= m_tpGraphObjects[tGraphID].tpObjects;

	D_OBJECT_PAIR_IT			I = m_tpGraphPointObjects.begin();
	D_OBJECT_PAIR_IT			E = m_tpGraphPointObjects.end();
	int							l_iGroupIndex;
	bool						l_bFirstTime = true;
	bool						l_bMutualDetection;
	for ( ; I != E; ++I) {
		if ((*I).first == tpALifeSchedulable->ID)
			continue;

		CSE_ALifeSchedulable	*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeSchedulable*>((*I).second);
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
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg("[LSS] %s started combat versus %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
				}
#endif
				ECombatResult			l_tCombatResult = eCombatResultRetreat12;
				bool					l_bDoNotContinue = false;
				for (int i=0; i<2*int(m_dwMaxCombatIterationCount); ++i) {
					if (eCombatActionAttack == tfChooseCombatAction(l_iGroupIndex)) {
#ifdef DEBUG
						if (psAI_Flags.test(aiALife)) {
							Msg("[LSS] %s choosed to attack %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
						}
#endif
						vfPerformAttackAction(l_iGroupIndex);

						l_bDoNotContinue = false;
					}
					else {
						if (l_bDoNotContinue)
							break;
#ifdef DEBUG
						if (psAI_Flags.test(aiALife)) {
							Msg("[LSS] %s choosed to retreat from %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
						}
#endif
						if (bfCheckIfRetreated(l_iGroupIndex)) {
#ifdef DEBUG
							if (psAI_Flags.test(aiALife)) {
								Msg("[LSS] %s did retreat from %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
							}
#endif
							l_tCombatResult	= l_iGroupIndex ? eCombatResultRetreat2 : eCombatResultRetreat1;
							break;
						}
						l_bDoNotContinue = true;
#ifdef DEBUG
						if (psAI_Flags.test(aiALife)) {
							Msg("[LSS] %s didn't retreat from %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
						}
#endif
					}

					l_iGroupIndex		^= 1;

					if (m_tpaCombatGroups[l_iGroupIndex].empty()) {
#ifdef DEBUG
						if (psAI_Flags.test(aiALife)) {
							Msg("[LSS] %s is dead",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace);
						}
#endif
						l_tCombatResult	= l_iGroupIndex ? eCombatResult1Kill2 : eCombatResult2Kill1;
						break;
					}
				}
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					if (eCombatResultRetreat12 == l_tCombatResult)
						Msg("[LSS] both combat groups decided not to continue combat");
				}
#endif
				vfFinishCombat			(l_tCombatResult);
				break;
			}
			case eMeetActionTypeInteract : {
				R_ASSERT2				(l_tpALifeHumanAbstract,"Non-human objects �annot communicate with each other");
				CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract2 = dynamic_cast<CSE_ALifeHumanAbstract*>(l_tpALifeSchedulable);
				R_ASSERT2				(l_tpALifeHumanAbstract2,"Non-human objects �annot communicate with each other");
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg					("[LSS] %s interacted with %s",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,m_tpaCombatObjects[l_iGroupIndex ^ 1]->s_name_replace);
				}
#endif
				vfPerformCommunication	();
				break;
			}
			case eMeetActionTypeIgnore : {
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg					("[LSS] %s refused from combat",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace);
				}
#endif
				continue;
			}
			default : NODEFAULT;
		}
		if (m_tpaCombatGroups[0].empty())
			break;
	}
}

ERelationType CSE_ALifeSimulator::tfGetRelationType(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract1, CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract2)
{
	if (tpALifeMonsterAbstract1->g_team() != tpALifeMonsterAbstract2->g_team())
		return(eRelationTypeEnemy);
	else
		return(eRelationTypeFriend);
}