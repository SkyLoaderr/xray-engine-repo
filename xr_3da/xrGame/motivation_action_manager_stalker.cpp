////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_stalker.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivation action manager class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "motivation_action_manager_stalker.h"
#include "stalker_motivations.h"
#include "stalker_actions.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_decision_space.h"
#include "script_game_object.h"

#ifdef DEBUG
extern EStalkerBehaviour	g_stalker_behaviour;
#endif

//#define GOAP_DEBUG

using namespace StalkerDecisionSpace;

CMotivationActionManagerStalker::CMotivationActionManagerStalker	()
{
	init					();
}

CMotivationActionManagerStalker::~CMotivationActionManagerStalker	()
{
}

void CMotivationActionManagerStalker::init				()
{
	m_death_planner			= 0;
	m_alife_planner			= 0;
	m_combat_planner		= 0;
	m_anomaly_planner		= 0;
#ifdef DEBUG
	m_stalker_behaviour		= g_stalker_behaviour;
#endif
}

#ifdef LOG_ACTION
LPCSTR CMotivationActionManagerStalker::action2string	(const _action_id_type &action_id)
{
	return					(inherited::action2string(action_id));
}

LPCSTR CMotivationActionManagerStalker::property2string	(const _condition_type &property_id)
{
	return					(inherited::property2string(property_id));
}

LPCSTR CMotivationActionManagerStalker::object_name		() const
{
	if (!m_object)
		return				("undefined");
	return					(*m_object->cName());
}
#endif

void CMotivationActionManagerStalker::Load				(LPCSTR section)
{
	inherited::Load			(section);
}

void CMotivationActionManagerStalker::reinit			(CAI_Stalker *object, bool clear_all)
{
	inherited::reinit		(object,clear_all);

	m_death_planner->CScriptActionPlanner::m_storage.clear			();
	m_alife_planner->CScriptActionPlanner::m_storage.clear			();
	m_combat_planner->CScriptActionPlanner::m_storage.clear			();
	m_anomaly_planner->CScriptActionPlanner::m_storage.clear		();

	m_death_planner->CScriptActionPlanner::m_storage.set_property	(eWorldPropertyDead,false);
	m_combat_planner->CScriptActionPlanner::m_storage.set_property	(eWorldPropertyEnemyAimed,false);
	m_combat_planner->CScriptActionPlanner::m_storage.set_property	(eWorldPropertyFireEnough,false);
	m_combat_planner->CScriptActionPlanner::m_storage.set_property	(eWorldPropertySafeToKill,false);
	m_anomaly_planner->CScriptActionPlanner::m_storage.set_property	(eWorldPropertyAnomaly,false);

//	{
//		OPERATOR_VECTOR::iterator	I = m_operators.begin();
//		OPERATOR_VECTOR::iterator	E = m_operators.end();
//		for ( ; I != E; ++I)
//			Msg				("Stalker operator %s : %x",(*I).get_operator()->m_action_name,smart_cast<void*>(&*(*I).get_operator()));
//	}
//	{
//		EVALUATOR_MAP::iterator		I = m_evaluators.begin();
//		EVALUATOR_MAP::iterator		E = m_evaluators.end();
//		for ( ; I != E; ++I)
//			Msg				("Stalker evaluator %d : %x",(*I).first,smart_cast<void*>(&*(*I).second));
//	}

#ifdef LOG_ACTION
	if (psAI_Flags.test(aiGOAP))
		m_use_log			= true;
	else
		m_use_log			= false;
#endif
}

void CMotivationActionManagerStalker::reload			(LPCSTR section)
{
	inherited::reload		(section);

	clear					();

	m_death_planner			= xr_new<CActionPlannerAction>(m_object,"death_planner");
	m_alife_planner			= xr_new<CActionPlannerAction>(m_object,"alife_planner");
	m_combat_planner		= xr_new<CActionPlannerAction>(m_object,"combat_planner");
	m_anomaly_planner		= xr_new<CActionPlannerAction>(m_object,"anomaly_planner");

	add_motivations			();
	add_evaluators			();
	add_actions				();
}

void CMotivationActionManagerStalker::update			(u32 time_delta)
{
#ifdef LOG_ACTION
	if ((psAI_Flags.test(aiGOAP) && !m_use_log) || (!psAI_Flags.test(aiGOAP) && m_use_log)) {
		m_use_log			= !m_use_log;
		OPERATOR_VECTOR::iterator	I = m_operators.begin();
		OPERATOR_VECTOR::iterator	E = m_operators.end();
		for ( ; I != E; ++I)
			(*I).get_operator()->m_use_log = m_use_log;
	}
#endif
#ifdef DEBUG
	if (m_stalker_behaviour!=g_stalker_behaviour) {
		CSActionPlanner::m_actuality	= false;
	}
#endif
	inherited::update		();
	
#ifdef GOAP_DEBUG
	if (m_failed) {
		{
			Msg			("%d",evaluators().size());
			EVALUATOR_MAP::const_iterator	I = evaluators().begin();
			EVALUATOR_MAP::const_iterator	E = evaluators().end();
			for ( ; I != E; ++I)
				if ((*I).first != 5023)
					Msg		("%d,%d",(*I).first,(*I).second->evaluate() ? 1 : 0);
				else
					Msg		("%d,%d",(*I).first,0);
		}
		{
			Msg			("%d",target_state().conditions().size());
			xr_vector<COperatorCondition>::const_iterator	I = target_state().conditions().begin();
			xr_vector<COperatorCondition>::const_iterator	E = target_state().conditions().end();
			for ( ; I != E; ++I)
				Msg		("%d,%d",(*I).condition(),(*I).value() ? 1 : 0);
		}
		{
			Msg			("%d",operators().size());
			const_iterator	I = operators().begin();
			const_iterator	E = operators().end();
			for ( ; I != E; ++I) {
				Msg		("%d,%d",(*I).m_operator_id,(*I).m_operator->weight(target_state(),target_state()));
				{
					Msg		("%d",(*I).m_operator->conditions().conditions().size());
					xr_vector<COperatorCondition>::const_iterator	i = (*I).m_operator->conditions().conditions().begin();
					xr_vector<COperatorCondition>::const_iterator	e = (*I).m_operator->conditions().conditions().end();
					for ( ; i != e; ++i)
						Msg		("%d,%d",(*i).condition(),(*i).value() ? 1 : 0);
				}
				{
					Msg		("%d",(*I).m_operator->effects().conditions().size());
					xr_vector<COperatorCondition>::const_iterator	i = (*I).m_operator->effects().conditions().begin();
					xr_vector<COperatorCondition>::const_iterator	e = (*I).m_operator->effects().conditions().end();
					for ( ; i != e; ++i)
						Msg		("%d,%d",(*i).condition(),(*i).value() ? 1 : 0);
				}
			}
		}
	}
#endif

#ifdef DEBUG
	m_stalker_behaviour		= g_stalker_behaviour;
#endif
}

void CMotivationActionManagerStalker::add_motivations	()
{
	CWorldState				goal;
	
	// complex motivations
	add_motivation			(eMotivationGlobal,			xr_new<CMotivationGlobal>());

	// final motivations
	goal.clear				();
	add_condition			(goal,eWorldPropertyAlreadyDead,	true);
	add_motivation			(eMotivationDead,			xr_new<CStalkerMotivationAction>(goal));

	add_motivation			(eMotivationAlive,			xr_new<CMotivationAlive>());
	
	goal.clear				();
	add_condition			(goal,eWorldPropertyPuzzleSolved,	true);
	add_motivation			(eMotivationSolveZonePuzzle,xr_new<CStalkerMotivationAction>(goal));

	add_motivation			(eMotivationSquadCommand,	xr_new<CMotivationSquad>());

	goal.clear				();
	add_condition			(goal,eWorldPropertySquadGoal,		true);
	add_motivation			(eMotivationSquadGoal,		xr_new<CMotivationSquadGoal>(goal));

	goal.clear				();
	add_condition			(goal,eWorldPropertySquadAction,	true);
	add_motivation			(eMotivationSquadAction,	xr_new<CStalkerMotivationAction>(goal));

	// connect motivations
	add_connection			(eMotivationGlobal,			eMotivationDead);
	add_connection			(eMotivationGlobal,			eMotivationAlive);
	add_connection			(eMotivationAlive,			eMotivationSolveZonePuzzle);
	add_connection			(eMotivationAlive,			eMotivationSquadCommand);
	add_connection			(eMotivationSquadCommand,	eMotivationSquadGoal);
	add_connection			(eMotivationSquadCommand,	eMotivationSquadAction);
}

void CMotivationActionManagerStalker::add_evaluators		()
{
	add_evaluator			(eWorldPropertyAlreadyDead		,xr_new<CStalkerPropertyEvaluatorConst>				(false));
	add_evaluator			(eWorldPropertyPuzzleSolved		,xr_new<CStalkerPropertyEvaluatorConst>				(false));
	add_evaluator			(eWorldPropertySquadAction		,xr_new<CStalkerPropertyEvaluatorConst>				(false));
	add_evaluator			(eWorldPropertyAlive			,xr_new<CStalkerPropertyEvaluatorAlive>				());
	add_evaluator			(eWorldPropertyEnemy			,xr_new<CStalkerPropertyEvaluatorEnemies>			());
	add_evaluator			(eWorldPropertyAnomaly			,xr_new<CStalkerPropertyEvaluatorAnomaly>			());

	m_death_planner->add_evaluator	(eWorldPropertyAlreadyDead		,xr_new<CStalkerPropertyEvaluatorConst>				(false));
	m_death_planner->add_evaluator	(eWorldPropertyDead				,xr_new<CStalkerPropertyEvaluatorMember>			(&m_storage,eWorldPropertyDead,true));

	m_alife_planner->add_evaluator	(eWorldPropertyPuzzleSolved		,xr_new<CStalkerPropertyEvaluatorConst>				(false));
	m_alife_planner->add_evaluator	(eWorldPropertyALife			,xr_new<CStalkerPropertyEvaluatorALife>				());
	m_alife_planner->add_evaluator	(eWorldPropertyItems			,xr_new<CStalkerPropertyEvaluatorItems>				());

	m_combat_planner->add_evaluator	(eWorldPropertyEnemy			,xr_new<CStalkerPropertyEvaluatorEnemies>			());
	m_combat_planner->add_evaluator	(eWorldPropertyEnemyAimed		,xr_new<CStalkerPropertyEvaluatorMember>			(&m_storage,eWorldPropertyEnemyAimed,true,true));
	m_combat_planner->add_evaluator	(eWorldPropertyFireEnough		,xr_new<CStalkerPropertyEvaluatorMember>			(&m_storage,eWorldPropertyFireEnough,true,true));
	m_combat_planner->add_evaluator	(eWorldPropertySafeToKill		,xr_new<CStalkerPropertyEvaluatorMember>			(&m_storage,eWorldPropertySafeToKill,true,true));
	m_combat_planner->add_evaluator	(eWorldPropertySeeEnemy			,xr_new<CStalkerPropertyEvaluatorSeeEnemy>			());
	m_combat_planner->add_evaluator	(eWorldPropertyItemToKill		,xr_new<CStalkerPropertyEvaluatorItemToKill>		());
	m_combat_planner->add_evaluator	(eWorldPropertyItemCanKill		,xr_new<CStalkerPropertyEvaluatorItemCanKill>		());
	m_combat_planner->add_evaluator	(eWorldPropertyFoundItemToKill	,xr_new<CStalkerPropertyEvaluatorFoundItemToKill>	());
	m_combat_planner->add_evaluator	(eWorldPropertyFoundAmmo		,xr_new<CStalkerPropertyEvaluatorFoundAmmo>			());
	m_combat_planner->add_evaluator	(eWorldPropertyReadyToKill		,xr_new<CStalkerPropertyEvaluatorReadyToKill>		());
	m_combat_planner->add_evaluator	(eWorldPropertyKillDistance		,xr_new<CStalkerPropertyEvaluatorKillDistance>		());

	m_anomaly_planner->add_evaluator(eWorldPropertyInsideAnomaly	,xr_new<CStalkerPropertyEvaluatorInsideAnomaly>		());
	m_anomaly_planner->add_evaluator(eWorldPropertyAnomaly			,xr_new<CStalkerPropertyEvaluatorAnomaly>			());
}

void CMotivationActionManagerStalker::add_actions			()
{
	CAction					*action;
	CActionPlannerAction	*planner;

	planner					= m_death_planner;
	add_condition			(planner,eWorldPropertyAlive,		false);
	add_condition			(planner,eWorldPropertyAlreadyDead,	false);
	add_effect				(planner,eWorldPropertyAlreadyDead,	true);
	add_operator			(eWorldOperatorDeathPlanner,planner);

	planner					= m_alife_planner;
	add_condition			(planner,eWorldPropertyAlive,		true);
	add_condition			(planner,eWorldPropertyEnemy,		false);
	add_condition			(planner,eWorldPropertyAnomaly,		false);
	add_condition			(planner,eWorldPropertyPuzzleSolved,false);
	add_effect				(planner,eWorldPropertyPuzzleSolved,true);
	add_operator			(eWorldOperatorALifePlanner,planner);

	planner					= m_combat_planner;
	add_condition			(planner,eWorldPropertyAlive,		true);
	add_condition			(planner,eWorldPropertyEnemy,		true);
	add_effect				(planner,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorCombatPlanner,planner);

	planner					= m_anomaly_planner;
	add_condition			(planner,eWorldPropertyAlive,		true);
	add_condition			(planner,eWorldPropertyEnemy,		false);
	add_condition			(planner,eWorldPropertyAnomaly,		true);
	add_effect				(planner,eWorldPropertyAnomaly,		false);
	add_operator			(eWorldOperatorAnomalyPlanner,planner);

	action					= xr_new<CStalkerActionSquad>		(m_object,"squad_action");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertySquadAction,	false);
	add_effect				(action,eWorldPropertySquadAction,	true);
	add_operator			(eWorldOperatorSquadAction,			action);

	action					= xr_new<CStalkerActionDead>		(m_object,"dead");
	add_condition			(action,eWorldPropertyDead,			false);
	add_effect				(action,eWorldPropertyDead,			true);
	m_death_planner->add_operator	(eWorldOperatorDead,				action);

	action					= xr_new<CAction>					(m_object,"already_dead");
	add_condition			(action,eWorldPropertyDead,			true);
	add_effect				(action,eWorldPropertyAlreadyDead,	true);
	m_death_planner->add_operator			(eWorldOperatorAlreadyDead,			action);

	action					= xr_new<CStalkerActionFreeNoALife>	(m_object,"free_alife");
	add_condition			(action,eWorldPropertyALife,		true);
	add_condition			(action,eWorldPropertyItems,		false);
	add_condition			(action,eWorldPropertyPuzzleSolved,	false);
	add_effect				(action,eWorldPropertyPuzzleSolved,	true);
	m_alife_planner->add_operator			(eWorldOperatorFreeALife,			action);

	action					= xr_new<CStalkerActionFreeNoALife>	(m_object,"free_no_alife");
	add_condition			(action,eWorldPropertyALife,		false);
	add_condition			(action,eWorldPropertyItems,		false);
	add_condition			(action,eWorldPropertyPuzzleSolved,	false);
	add_effect				(action,eWorldPropertyPuzzleSolved,	true);
	m_alife_planner->add_operator			(eWorldOperatorFreeNoALife,			action);

	action					= xr_new<CStalkerActionGatherItems>	(m_object,"gather_items");
	add_condition			(action,eWorldPropertyItems,		true);
	add_effect				(action,eWorldPropertyItems,		false);
	m_alife_planner->add_operator			(eWorldOperatorGatherItems,			action);

	action					= xr_new<CStalkerActionGetKillDistance>	(m_object,"get_kill_distance");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	false);
	add_effect				(action,eWorldPropertyKillDistance,	true);
	m_combat_planner->add_operator			(eWorldOperatorGetKillDistance,		action);

//	action					= xr_new<CStalkerActionGetEnemy>	(m_object,"get_enemy");
//	add_condition			(action,eWorldPropertyReadyToKill,	true);
//	add_condition			(action,eWorldPropertyEnemy,		false);
//	add_effect				(action,eWorldPropertyEnemy,		true);
//	m_combat_planner->add_operator			(eWorldOperatorGetEnemy,			action);

	action					= xr_new<CStalkerActionGetEnemySeen>(m_object,"get_enemy_seen");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_effect				(action,eWorldPropertySeeEnemy,		true);
	m_combat_planner->add_operator			(eWorldOperatorGetEnemySeen,		action);

	action					= xr_new<CStalkerActionGetItemToKill>	(m_object,"get_item_to_kill");
	add_condition			(action,eWorldPropertyFoundItemToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,		false);
	add_effect				(action,eWorldPropertyItemToKill,		true);
	add_effect				(action,eWorldPropertyItemCanKill,		true);
	m_combat_planner->add_operator			(eWorldOperatorGetItemToKill,			action);

	action					= xr_new<CStalkerActionFindItemToKill>	(m_object,"find_item_to_kill");
	add_condition			(action,eWorldPropertyItemToKill,		false);
	add_condition			(action,eWorldPropertyFoundItemToKill,	false);
	add_effect				(action,eWorldPropertyFoundItemToKill,	true);
	m_combat_planner->add_operator			(eWorldOperatorFindItemToKill,			action);

	action					= xr_new<CStalkerActionMakeItemKilling>	(m_object,"make_item_killing");
	add_condition			(action,eWorldPropertyFoundAmmo,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	false);
	add_effect				(action,eWorldPropertyItemCanKill,	true);
	m_combat_planner->add_operator			(eWorldOperatorMakeItemKilling,		action);

	action					= xr_new<CStalkerActionFindAmmo>	(m_object,"find_ammo");
	add_condition			(action,eWorldPropertyFoundItemToKill,	true);
	add_condition			(action,eWorldPropertyFoundAmmo,		false);
	add_effect				(action,eWorldPropertyFoundAmmo,		true);
	m_combat_planner->add_operator			(eWorldOperatorFindAmmo,				action);

	action					= xr_new<CStalkerActionGetReadyToKillVeryAggressive>	(m_object,"get_ready_to_kill_very_aggressive");
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	m_combat_planner->add_operator			(eWorldOperatorGetReadyToKillVeryAggressive,action);

	action					= xr_new<CStalkerActionKillEnemyVeryAggressive>	(m_object,"kill_enemy_very_aggressive");
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	m_combat_planner->add_operator			(eWorldOperatorKillEnemyVeryAggressive,	action);

	action					= xr_new<CStalkerActionGetReadyToKillAggressive>(m_object,"get_ready_to_kill_aggressive");
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	m_combat_planner->add_operator			(eWorldOperatorGetReadyToKillAggressive,action);

	action					= xr_new<CStalkerActionKillEnemyAggressive>	(m_object,"kill_enemy_aggressive");
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyEnemyAimed,	true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	m_combat_planner->add_operator			(eWorldOperatorKillEnemyAggressive,	action);

	action					= xr_new<CStalkerActionGetReadyToKillAvoid>(m_object,"get_ready_to_kill_avoid");
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	m_combat_planner->add_operator			(eWorldOperatorGetReadyToKillAvoid,action);

	action					= xr_new<CStalkerActionKillEnemyAvoid>	(m_object,"kill_enemy_avoid");
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	m_combat_planner->add_operator			(eWorldOperatorKillEnemyAvoid,		action);

	action					= xr_new<CStalkerActionRetreatFromEnemy>	(m_object,"retreat_from_enemy");
	add_effect				(action,eWorldPropertyEnemy,		false);
	m_combat_planner->add_operator			(eWorldOperatorRetreatFromEnemy,	action);

	action					= xr_new<CStalkerActionCamp>(m_object,"camping");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_effect				(action,eWorldPropertySeeEnemy,		true);
	m_combat_planner->add_operator			(eWorldOperatorCamping,				action);

	action					= xr_new<CStalkerActionGetEnemySeenModerate>(m_object,"get_enemy_seen_moderate");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_effect				(action,eWorldPropertySeeEnemy,		true);
	m_combat_planner->add_operator			(eWorldOperatorGetEnemySeenModerate,action);

	action					= xr_new<CStalkerActionGetReadyToKillModerate>(m_object,"get_ready_to_kill_moderate");
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	m_combat_planner->add_operator			(eWorldOperatorGetReadyToKillModerate,action);

	action					= xr_new<CStalkerActionKillEnemyModerate>	(m_object,"kill_enemy_moderate");
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyEnemyAimed,	true);
	add_condition			(action,eWorldPropertySafeToKill,	true);
	add_condition			(action,eWorldPropertyFireEnough,	false);
	add_effect				(action,eWorldPropertyEnemy,		false);
	m_combat_planner->add_operator			(eWorldOperatorKillEnemyModerate,	action);

	action					= xr_new<CStalkerActionAimEnemy>	(m_object,"aim_enemy");
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertySafeToKill,	true);
	add_condition			(action,eWorldPropertyFireEnough,	false);
	add_effect				(action,eWorldPropertyEnemyAimed,	true);
	m_combat_planner->add_operator			(eWorldOperatorAimEnemy,			action);

	action					= xr_new<CStalkerActionKillEnemyLostModerate>	(m_object,"kill_enemy_moderate_lost");
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertySafeToKill,	false);
	add_effect				(action,eWorldPropertyEnemy,		false);
	m_combat_planner->add_operator			(eWorldOperatorKillEnemyLostModerate,action);

	action					= xr_new<CStalkerActionTakeCover>	(m_object,"take_cover");
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyFireEnough,	true);
	add_effect				(action,eWorldPropertyFireEnough,	false);
	m_combat_planner->add_operator			(eWorldOperatorTakeCover,			action);

	action					= xr_new<CStalkerActionGetOutOfAnomaly>	(m_object,"get_out_of_anomaly");
	add_condition			(action,eWorldPropertyInsideAnomaly,true);
	add_effect				(action,eWorldPropertyInsideAnomaly,false);
	m_anomaly_planner->add_operator			(eWorldOperatorGetOutOfAnomaly,		action);

	action					= xr_new<CStalkerActionDetectAnomaly>	(m_object,"detect_anomaly");
	add_condition			(action,eWorldPropertyInsideAnomaly,false);
	add_condition			(action,eWorldPropertyAnomaly,true);
	add_effect				(action,eWorldPropertyAnomaly,false);
	m_anomaly_planner->add_operator			(eWorldOperatorDetectAnomaly,		action);
}
