////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_planner.cpp
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_combat_planner.h"
#include "stalker_combat_actions.h"
#include "stalker_decision_space.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

using namespace StalkerDecisionSpace;

CStalkerCombatPlanner::CStalkerCombatPlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited									(object,action_name)
{
}

CStalkerCombatPlanner::~CStalkerCombatPlanner	()
{
}

void CStalkerCombatPlanner::reinit				(CAI_Stalker *object, CPropertyStorage *storage, bool clear_all)
{
	inherited::reinit		(object,storage,clear_all);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyEnemyAimed,false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyFireEnough,false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertySafeToKill,false);
}

void CStalkerCombatPlanner::reload			(LPCSTR section)
{
	inherited::reload		(section);
	clear					();
	add_evaluators			();
	add_actions				();
}

void CStalkerCombatPlanner::add_evaluators		()
{
	add_evaluator			(eWorldPropertyEnemy			,xr_new<CStalkerPropertyEvaluatorEnemies>			());
	add_evaluator			(eWorldPropertyEnemyAimed		,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyEnemyAimed,true,true));
	add_evaluator			(eWorldPropertyFireEnough		,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyFireEnough,true,true));
	add_evaluator			(eWorldPropertySafeToKill		,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertySafeToKill,true,true));
	add_evaluator			(eWorldPropertySeeEnemy			,xr_new<CStalkerPropertyEvaluatorSeeEnemy>			());
	add_evaluator			(eWorldPropertyItemToKill		,xr_new<CStalkerPropertyEvaluatorItemToKill>		());
	add_evaluator			(eWorldPropertyItemCanKill		,xr_new<CStalkerPropertyEvaluatorItemCanKill>		());
	add_evaluator			(eWorldPropertyFoundItemToKill	,xr_new<CStalkerPropertyEvaluatorFoundItemToKill>	());
	add_evaluator			(eWorldPropertyFoundAmmo		,xr_new<CStalkerPropertyEvaluatorFoundAmmo>			());
	add_evaluator			(eWorldPropertyReadyToKill		,xr_new<CStalkerPropertyEvaluatorReadyToKill>		());
	add_evaluator			(eWorldPropertyPanic			,xr_new<CStalkerPropertyEvaluatorPanic>				());
}

void CStalkerCombatPlanner::add_actions			()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionGetItemToKill>	(m_object,"get_item_to_kill");
	add_condition			(action,eWorldPropertyFoundItemToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,		false);
	add_effect				(action,eWorldPropertyItemToKill,		true);
	add_effect				(action,eWorldPropertyItemCanKill,		true);
	add_operator			(eWorldOperatorGetItemToKill,			action);

	action					= xr_new<CStalkerActionMakeItemKilling>	(m_object,"make_item_killing");
	add_condition			(action,eWorldPropertyFoundAmmo,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	false);
	add_effect				(action,eWorldPropertyItemCanKill,	true);
	add_operator			(eWorldOperatorMakeItemKilling,		action);

	action					= xr_new<CStalkerActionRetreatFromEnemy>	(m_object,"retreat_from_enemy");
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorRetreatFromEnemy,	action);

	action					= xr_new<CStalkerActionCamp>(m_object,"camping");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_effect				(action,eWorldPropertySeeEnemy,		true);
	add_operator			(eWorldOperatorCamping,				action);

	action					= xr_new<CStalkerActionGetEnemySeenModerate>(m_object,"get_enemy_seen_moderate");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_effect				(action,eWorldPropertySeeEnemy,		true);
	add_operator			(eWorldOperatorGetEnemySeenModerate,action);

	action					= xr_new<CStalkerActionGetReadyToKillModerate>(m_object,"get_ready_to_kill_moderate");
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	add_operator			(eWorldOperatorGetReadyToKillModerate,action);

	action					= xr_new<CStalkerActionKillEnemyModerate>	(m_object,"kill_enemy_moderate");
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyEnemyAimed,	true);
	add_condition			(action,eWorldPropertySafeToKill,	true);
	add_condition			(action,eWorldPropertyFireEnough,	false);
	add_condition			(action,eWorldPropertyPanic,		false);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorKillEnemyModerate,	action);

	action					= xr_new<CStalkerActionAimEnemy>	(m_object,"aim_enemy");
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertySafeToKill,	true);
	add_condition			(action,eWorldPropertyFireEnough,	false);
	add_effect				(action,eWorldPropertyEnemyAimed,	true);
	add_operator			(eWorldOperatorAimEnemy,			action);

	action					= xr_new<CStalkerActionKillEnemyLostModerate>	(m_object,"kill_enemy_moderate_lost");
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertySafeToKill,	false);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorKillEnemyLostModerate,action);

	action					= xr_new<CStalkerActionTakeCover>	(m_object,"take_cover");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyFireEnough,	true);
	add_effect				(action,eWorldPropertyFireEnough,	false);
	add_operator			(eWorldOperatorTakeCover,			action);
}
