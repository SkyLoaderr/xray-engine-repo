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

#ifdef DEBUG
extern EStalkerBehaviour	g_stalker_behaviour;
#endif

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
#endif

void CMotivationActionManagerStalker::Load				(LPCSTR section)
{
	inherited::Load			(section);
}

void CMotivationActionManagerStalker::reinit			(CAI_Stalker *object, bool clear_all)
{
	inherited::reinit		(object,clear_all);

	m_storage.clear			();
	m_storage.set_property	(eWorldPropertyDead,false);
	m_storage.set_property	(eWorldPropertyEnemyAimed,false);

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
	inherited::update		(time_delta);
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

	goal.clear				();
	add_condition			(goal,eWorldPropertyPuzzleSolved,	true);
	add_motivation			(eMotivationAlive,			xr_new<CStalkerMotivationAction>(goal));

	// connect motivations
	add_connection			(eMotivationGlobal,			eMotivationDead);
	add_connection			(eMotivationGlobal,			eMotivationAlive);
}

void CMotivationActionManagerStalker::add_evaluators		()
{
	add_evaluator			(eWorldPropertyAlreadyDead		,xr_new<CStalkerPropertyEvaluatorConst>				(false));
	add_evaluator			(eWorldPropertyPuzzleSolved		,xr_new<CStalkerPropertyEvaluatorConst>				(false));

	add_evaluator			(eWorldPropertyDead				,xr_new<CStalkerPropertyEvaluatorMember>			(&m_storage,eWorldPropertyDead,true));
	
	add_evaluator			(eWorldPropertyALife			,xr_new<CStalkerPropertyEvaluatorALife>				());
	add_evaluator			(eWorldPropertyAlive			,xr_new<CStalkerPropertyEvaluatorAlive>				());
	add_evaluator			(eWorldPropertyItems			,xr_new<CStalkerPropertyEvaluatorItems>				());
	add_evaluator			(eWorldPropertyEnemy			,xr_new<CStalkerPropertyEvaluatorEnemies>			());
	add_evaluator			(eWorldPropertySeeEnemy			,xr_new<CStalkerPropertyEvaluatorSeeEnemy>			());

	add_evaluator			(eWorldPropertyItemToKill		,xr_new<CStalkerPropertyEvaluatorItemToKill>		());
	add_evaluator			(eWorldPropertyItemCanKill		,xr_new<CStalkerPropertyEvaluatorItemCanKill>		());
	add_evaluator			(eWorldPropertyFoundItemToKill	,xr_new<CStalkerPropertyEvaluatorFoundItemToKill>	());
	add_evaluator			(eWorldPropertyFoundAmmo		,xr_new<CStalkerPropertyEvaluatorFoundAmmo>			());
	add_evaluator			(eWorldPropertyReadyToKill		,xr_new<CStalkerPropertyEvaluatorReadyToKill>		());
	add_evaluator			(eWorldPropertyKillDistance		,xr_new<CStalkerPropertyEvaluatorKillDistance>		());
	add_evaluator			(eWorldPropertyEnemyAimed		,xr_new<CStalkerPropertyEvaluatorMember>			(&m_storage,eWorldPropertyEnemyAimed,true,true));
}

void CMotivationActionManagerStalker::add_actions			()
{
	CAction					*action;

	action					= xr_new<CStalkerActionDead>		(m_object,"dead");
	add_condition			(action,eWorldPropertyAlive,		false);
	add_condition			(action,eWorldPropertyDead,			false);
	add_effect				(action,eWorldPropertyDead,			true);
	add_operator			(eWorldOperatorDead,				action);

	action					= xr_new<CAction>					(m_object,"already_dead");
	add_condition			(action,eWorldPropertyDead,			true);
	add_effect				(action,eWorldPropertyAlreadyDead,	true);
	add_operator			(eWorldOperatorAlreadyDead,			action);

	action					= xr_new<CStalkerActionFreeNoALife>	(m_object,"free_alife");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyALife,		true);
	add_condition			(action,eWorldPropertyPuzzleSolved,	false);
	add_effect				(action,eWorldPropertyPuzzleSolved,	true);
	add_operator			(eWorldOperatorFreeALife,			action);

	action					= xr_new<CStalkerActionFreeNoALife>	(m_object,"free_no_alife");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyALife,		false);
	add_condition			(action,eWorldPropertyItems,		false);
	add_condition			(action,eWorldPropertyEnemy,		false);
	add_condition			(action,eWorldPropertyPuzzleSolved,	false);
	add_effect				(action,eWorldPropertyPuzzleSolved,	true);
	add_operator			(eWorldOperatorFreeNoALife,			action);

	action					= xr_new<CStalkerActionGatherItems>	(m_object,"gather_items");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyItems,		true);
	add_condition			(action,eWorldPropertyEnemy,		false);
	add_effect				(action,eWorldPropertyItems,		false);
	add_operator			(eWorldOperatorGatherItems,			action);

	action					= xr_new<CStalkerActionGetKillDistance>	(m_object,"get_kill_distance");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	false);
	add_effect				(action,eWorldPropertyKillDistance,	true);
	add_operator			(eWorldOperatorGetKillDistance,		action);

	action					= xr_new<CStalkerActionGetEnemy>	(m_object,"get_enemy");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyEnemy,		false);
	add_effect				(action,eWorldPropertyEnemy,		true);
	add_operator			(eWorldOperatorGetEnemy,			action);

	action					= xr_new<CStalkerActionGetEnemySeen>(m_object,"get_enemy_seen");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_effect				(action,eWorldPropertySeeEnemy,		true);
	add_operator			(eWorldOperatorGetEnemySeen,		action);

	action					= xr_new<CStalkerActionCamp>(m_object,"camping");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_effect				(action,eWorldPropertySeeEnemy,		true);
	add_operator			(eWorldOperatorCamping,				action);

	action					= xr_new<CStalkerActionGetItemToKill>	(m_object,"get_item_to_kill");
	add_condition			(action,eWorldPropertyAlive,			true);
	add_condition			(action,eWorldPropertyFoundItemToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,		false);
	add_effect				(action,eWorldPropertyItemToKill,		true);
	add_effect				(action,eWorldPropertyItemCanKill,		true);
	add_operator			(eWorldOperatorGetItemToKill,			action);

	action					= xr_new<CStalkerActionFindItemToKill>	(m_object,"find_item_to_kill");
	add_condition			(action,eWorldPropertyAlive,			true);
	add_condition			(action,eWorldPropertyItemToKill,		false);
	add_condition			(action,eWorldPropertyFoundItemToKill,	false);
	add_effect				(action,eWorldPropertyFoundItemToKill,	true);
	add_operator			(eWorldOperatorFindItemToKill,			action);

	action					= xr_new<CStalkerActionMakeItemKilling>	(m_object,"make_item_killing");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyFoundAmmo,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	false);
	add_effect				(action,eWorldPropertyItemCanKill,	true);
	add_operator			(eWorldOperatorMakeItemKilling,		action);

	action					= xr_new<CStalkerActionFindAmmo>	(m_object,"find_ammo");
	add_condition			(action,eWorldPropertyAlive,			true);
	add_condition			(action,eWorldPropertyFoundItemToKill,	true);
	add_condition			(action,eWorldPropertyFoundAmmo,		false);
	add_effect				(action,eWorldPropertyFoundAmmo,		true);
	add_operator			(eWorldOperatorFindAmmo,				action);

	action					= xr_new<CStalkerActionGetReadyToKillVeryAggressive>	(m_object,"get_ready_to_kill_very_aggressive");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	add_operator			(eWorldOperatorGetReadyToKillVeryAggressive,action);

	action					= xr_new<CStalkerActionKillEnemyVeryAggressive>	(m_object,"kill_enemy_very_aggressive");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorKillEnemyVeryAggressive,	action);

	action					= xr_new<CStalkerActionGetReadyToKillAggressive>(m_object,"get_ready_to_kill_aggressive");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	add_operator			(eWorldOperatorGetReadyToKillAggressive,action);

	action					= xr_new<CStalkerActionKillEnemyAggressive>	(m_object,"kill_enemy_aggressive");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyEnemyAimed,	true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorKillEnemyAggressive,	action);

	action					= xr_new<CStalkerActionAimEnemy>	(m_object,"aim_enemy");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_effect				(action,eWorldPropertyEnemyAimed,	true);
	add_operator			(eWorldOperatorAimEnemy,			action);

	action					= xr_new<CStalkerActionGetReadyToKillModerate>(m_object,"get_ready_to_kill_moderate");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	add_operator			(eWorldOperatorGetReadyToKillModerate,action);

	action					= xr_new<CStalkerActionKillEnemyModerate>	(m_object,"kill_enemy_moderate");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorKillEnemyModerate,	action);

	action					= xr_new<CStalkerActionGetReadyToKillAvoid>(m_object,"get_ready_to_kill_avoid");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	add_operator			(eWorldOperatorGetReadyToKillAvoid,action);

	action					= xr_new<CStalkerActionKillEnemyAvoid>	(m_object,"kill_enemy_avoid");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyKillDistance,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorKillEnemyAvoid,		action);

	action					= xr_new<CStalkerActionRetreatFromEnemy>	(m_object,"retreat_from_enemy");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorRetreatFromEnemy,	action);
}
