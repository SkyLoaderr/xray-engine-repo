////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_communication_manager.cpp
//	Created 	: 03.09.2003
//  Modified 	: 14.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife communication manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_interaction_manager.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_graph_registry.h"
#include "alife_news_registry.h"
#include "alife_time_manager.h"

using namespace ALife;

CALifeInteractionManager::CALifeInteractionManager	(xrServer*server, LPCSTR section) : 
	CALifeCombatManager			(server,section),
	CALifeCommunicationManager	(server,section),
	CALifeSimulatorBase			(server,section)
{
	m_inventory_slot_count		= pSettings->r_u32("inventory","slots");
	m_temp_weapons.resize		(m_inventory_slot_count);
	m_temp_marks.assign			(u16(-1),false);
}

CALifeInteractionManager::~CALifeInteractionManager()
{
}

void CALifeInteractionManager::check_for_interaction(CSE_ALifeSchedulable *tpALifeSchedulable)
{
	if (!tpALifeSchedulable->bfActive())
		return;
	CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeSchedulable);
	R_ASSERT2					(l_tpALifeDynamicObject,"Unknown schedulable object class");
	_GRAPH_ID					l_tGraphID = l_tpALifeDynamicObject->m_tGraphID;
	check_for_interaction		(tpALifeSchedulable,l_tGraphID);
	CGameGraph::const_iterator	I, E;
	ai().game_graph().begin		(l_tGraphID,I,E);
	for ( ; I != E; ++I)
		check_for_interaction	(tpALifeSchedulable,(*I).vertex_id());
}

class CCheckForInteractionPredicate {
public:
	CALifeInteractionManager	*manager;
	mutable CSE_ALifeSchedulable*tpALifeSchedulable;
	mutable _GRAPH_ID			tGraphID;
	mutable int						l_iGroupIndex;
	mutable bool					l_bMutualDetection;
	mutable CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract;

	IC	CCheckForInteractionPredicate(CALifeInteractionManager *manager, CSE_ALifeSchedulable *tpALifeSchedulable, _GRAPH_ID tGraphID) :
		manager(manager),
		tpALifeSchedulable(tpALifeSchedulable),
		tGraphID(tGraphID)
	{
		l_tpALifeHumanAbstract	= dynamic_cast<CSE_ALifeHumanAbstract*>(tpALifeSchedulable);
		manager->vfFillCombatGroup	(tpALifeSchedulable,0);
	}

	IC	bool operator()	(CALifeGraphRegistry::OBJECT_REGISTRY::_iterator &I, u64 counter, bool) const
	{
		if (counter == (*I).second->m_switch_counter)
			return				(false);
		return					(!manager->m_tpaCombatGroups[0].empty());
	}

	IC	void operator()	(CALifeGraphRegistry::OBJECT_REGISTRY::_iterator &I, u64 counter) const
	{
		(*I).second->m_switch_counter = counter;

		VERIFY					(!manager->m_tpaCombatGroups[0].empty());

		if ((*I).first == tpALifeSchedulable->base()->ID)
			return;

		CSE_ALifeSchedulable	*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeSchedulable*>((*I).second);
		if (!l_tpALifeSchedulable)
			return;

		if (!manager->bfCheckForInteraction(tpALifeSchedulable,l_tpALifeSchedulable,l_iGroupIndex,l_bMutualDetection))
			return;

		manager->vfFillCombatGroup		(l_tpALifeSchedulable,1);

		CSE_ALifeTraderAbstract	*trader_abstract0 = dynamic_cast<CSE_ALifeTraderAbstract*>(tpALifeSchedulable);
		CSE_ALifeTraderAbstract	*trader_abstract1 = dynamic_cast<CSE_ALifeTraderAbstract*>(l_tpALifeSchedulable);
		bool					add_news = trader_abstract0 || trader_abstract1;
		CALifeNews				news;
		switch (manager->m_tpaCombatObjects[l_iGroupIndex]->tfGetActionType(manager->m_tpaCombatObjects[l_iGroupIndex ^ 1],l_iGroupIndex,l_bMutualDetection)) {
			case eMeetActionTypeAttack : {
				if (add_news) {
					CSE_ALifeDynamicObject	*dynamic_object = dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeSchedulable);
					VERIFY					(dynamic_object);
					news.m_game_time		= manager->time_manager().game_time();
					news.m_game_vertex_id	= dynamic_object->m_tGraphID;
					news.m_object_id[0]		= tpALifeSchedulable->base()->ID;
					news.m_object_id[1]		= l_tpALifeSchedulable->base()->ID;
					news.m_class_id			= 0;
				}
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg("[LSS] %s started combat versus %s",manager->m_tpaCombatObjects[l_iGroupIndex]->base()->s_name_replace,manager->m_tpaCombatObjects[l_iGroupIndex ^ 1]->base()->s_name_replace);
				}
#endif
				ECombatResult	l_tCombatResult = eCombatResultRetreat12;
				bool					l_bDoNotContinue = false;
				for (int i=0; i<2*int(manager->m_dwMaxCombatIterationCount); ++i) {
					if (eCombatActionAttack == manager->choose_combat_action(l_iGroupIndex)) {
#ifdef DEBUG
						if (psAI_Flags.test(aiALife)) {
							Msg("[LSS] %s choosed to attack %s",manager->m_tpaCombatObjects[l_iGroupIndex]->base()->s_name_replace,manager->m_tpaCombatObjects[l_iGroupIndex ^ 1]->base()->s_name_replace);
						}
#endif
						manager->vfPerformAttackAction(l_iGroupIndex);

						l_bDoNotContinue = false;
					}
					else {
						if (l_bDoNotContinue)
							break;
#ifdef DEBUG
						if (psAI_Flags.test(aiALife)) {
							Msg("[LSS] %s choosed to retreat from %s",manager->m_tpaCombatObjects[l_iGroupIndex]->base()->s_name_replace,manager->m_tpaCombatObjects[l_iGroupIndex ^ 1]->base()->s_name_replace);
						}
#endif
						if (manager->bfCheckIfRetreated(l_iGroupIndex)) {
#ifdef DEBUG
							if (psAI_Flags.test(aiALife)) {
								Msg("[LSS] %s did retreat from %s",manager->m_tpaCombatObjects[l_iGroupIndex]->base()->s_name_replace,manager->m_tpaCombatObjects[l_iGroupIndex ^ 1]->base()->s_name_replace);
							}
#endif
							l_tCombatResult	= l_iGroupIndex ? eCombatResultRetreat2 : eCombatResultRetreat1;
							break;
						}
						l_bDoNotContinue = true;
#ifdef DEBUG
						if (psAI_Flags.test(aiALife)) {
							Msg("[LSS] %s didn't retreat from %s",manager->m_tpaCombatObjects[l_iGroupIndex]->base()->s_name_replace,manager->m_tpaCombatObjects[l_iGroupIndex ^ 1]->base()->s_name_replace);
						}
#endif
					}

					l_iGroupIndex		^= 1;

					if (manager->m_tpaCombatGroups[l_iGroupIndex].empty()) {
#ifdef DEBUG
						if (psAI_Flags.test(aiALife)) {
							Msg("[LSS] %s is dead",manager->m_tpaCombatObjects[l_iGroupIndex]->base()->s_name_replace);
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
				if (add_news) {
					switch (l_tCombatResult) {
						case eCombatResultRetreat1 : {
							news.m_news_type	= eNewsTypeRetreat;
							break;
						}
						case eCombatResultRetreat2 : {
							news.m_news_type	= eNewsTypeRetreat;
							news.m_object_id[0]	= l_tpALifeSchedulable->base()->ID;
							news.m_object_id[1]	= tpALifeSchedulable->base()->ID;
							break;
						}
						case eCombatResultRetreat12 : {
							news.m_news_type	= eNewsTypeRetreatBoth;
							break;
						}
						case eCombatResult1Kill2 : {
							news.m_news_type	= eNewsTypeKill;
							break;
						}
						case eCombatResult2Kill1 : {
							news.m_news_type	= eNewsTypeKill;
							news.m_object_id[0]	= l_tpALifeSchedulable->base()->ID;
							news.m_object_id[1]	= tpALifeSchedulable->base()->ID;
							break;
						}
						case eCombatResultBothKilled : {
							news.m_news_type	= eNewsTypeKillBoth;
							break;
						}
						default			: NODEFAULT;
					}
					manager->news().add	(news);
				}
				manager->vfFinishCombat	(l_tCombatResult);
				break;
			}
			case eMeetActionTypeInteract : {
				R_ASSERT2				(l_tpALifeHumanAbstract,"Non-human objects ñannot communicate with each other");
				CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract2 = dynamic_cast<CSE_ALifeHumanAbstract*>(l_tpALifeSchedulable);
				R_ASSERT2				(l_tpALifeHumanAbstract2,"Non-human objects ñannot communicate with each other");
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg					("[LSS] %s interacted with %s",manager->m_tpaCombatObjects[l_iGroupIndex]->base()->s_name_replace,manager->m_tpaCombatObjects[l_iGroupIndex ^ 1]->base()->s_name_replace);
				}
#endif
				manager->vfPerformCommunication	();
				break;
			}
			case eMeetActionTypeIgnore : {
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg					("[LSS] %s refused from combat",manager->m_tpaCombatObjects[l_iGroupIndex]->base()->s_name_replace);
				}
#endif
				return;
			}
			default : NODEFAULT;
		}
	}
};

void CALifeInteractionManager::check_for_interaction(CSE_ALifeSchedulable *tpALifeSchedulable, _GRAPH_ID tGraphID)
{
	graph().iterate_objects(
		tGraphID,
		CCheckForInteractionPredicate(
			this,
			tpALifeSchedulable,
			tGraphID
		)
	);
}
