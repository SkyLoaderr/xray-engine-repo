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
#ifdef LOG_ACTION
	m_use_log				= true;
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
	clear					();
	add_motivations			();
	add_evaluators			();
	add_actions				();

	inherited::reinit		(object,clear_all);

	m_storage.set_property	(eWorldPropertyDead,false);
}

void CMotivationActionManagerStalker::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CMotivationActionManagerStalker::update			(u32 time_delta)
{
	inherited::update		(time_delta);
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
	add_evaluator			(eWorldPropertyALife		,xr_new<CStalkerPropertyEvaluatorALife>		());
	add_evaluator			(eWorldPropertyAlive		,xr_new<CStalkerPropertyEvaluatorAlive>		());
	add_evaluator			(eWorldPropertyItems		,xr_new<CStalkerPropertyEvaluatorItems>		());
	add_evaluator			(eWorldPropertyEnemies		,xr_new<CStalkerPropertyEvaluatorEnemies>	());
	
	add_evaluator			(eWorldPropertyDead			,xr_new<CStalkerPropertyEvaluatorMember>(&m_storage,eWorldPropertyDead,true));
	
	add_evaluator			(eWorldPropertyAlreadyDead	,xr_new<CStalkerPropertyEvaluatorConst>	(false));
	add_evaluator			(eWorldPropertyPuzzleSolved	,xr_new<CStalkerPropertyEvaluatorConst>	(false));
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
	add_condition			(action,eWorldPropertyEnemies,		false);
	add_condition			(action,eWorldPropertyPuzzleSolved,	false);
	add_effect				(action,eWorldPropertyPuzzleSolved,	true);
	add_operator			(eWorldOperatorFreeNoALife,			action);

	action					= xr_new<CStalkerActionGatherItems>	(m_object,"gather_items");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyItems,		true);
	add_condition			(action,eWorldPropertyEnemies,		false);
	add_effect				(action,eWorldPropertyItems,		false);
	add_operator			(eWorldOperatorGatherItems,			action);

	action					= xr_new<CStalkerActionKillEnemy>	(m_object,"kill_enemy");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyEnemies,		true);
	add_effect				(action,eWorldPropertyEnemies,		false);
	add_operator			(eWorldOperatorKillEnemy,			action);
}
