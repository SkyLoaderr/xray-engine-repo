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
#include "ai/stalker/ai_stalker_impl.h"
#include "script_game_object.h"
#include "cover_evaluators.h"
#include "cover_manager.h"
#include "cover_point.h"
#include "stalker_movement_restriction.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "enemy_manager.h"
#include "danger_manager.h"
#include "sound_player.h"
#include "missile.h"
#include "explosive.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "member_order.h"
#include "ai/stalker/ai_stalker_space.h"

using namespace StalkerSpace;

const u32 TOLLS_INTERVAL	= 2000;
const u32 GRENADE_INTERVAL	= 0*1000;
const float FRIENDLY_GRENADE_ALARM_DIST	= 5.f;

using namespace StalkerDecisionSpace;

CStalkerCombatPlanner::CStalkerCombatPlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited									(object,action_name)
{
}

CStalkerCombatPlanner::~CStalkerCombatPlanner	()
{
}

void CStalkerCombatPlanner::setup				(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup		(object,storage);

	CScriptActionPlanner::m_storage.set_property(eWorldPropertyInCover,			false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyLookedOut,		false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyPositionHolded,	false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyEnemyDetoured,	false);

	clear					();
	add_evaluators			();
	add_actions				();
}

IC	void CStalkerCombatPlanner::update_cover	()
{
	if (!m_object->memory().enemy().selected())
		return;

	CMemoryInfo						memory_object = m_object->memory().memory(m_object->memory().enemy().selected());
	if ((memory_object.m_last_level_time == m_last_level_time) && (m_object->memory().enemy().selected()->ID() == m_last_enemy_id))
		return;

	m_last_enemy_id					= m_object->memory().enemy().selected()->ID();
	m_last_level_time				= memory_object.m_last_level_time;

	Fvector							position = memory_object.m_object_params.m_position;
	m_object->m_ce_best->setup		(position,10.f,170.f,10.f);
	CCoverPoint						*point = ai().cover_manager().best_cover(m_object->Position(),10.f,*m_object->m_ce_best,CStalkerMovementRestrictor(m_object,true));
	if (!point) {
		m_object->m_ce_best->setup	(position,10.f,170.f,10.f);
		point						= ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_best,CStalkerMovementRestrictor(m_object,true));
	}

	if (point == m_last_cover)
		return;

//	Msg								("%6d Changing cover for stalker %s",Device.dwTimeGlobal,*m_object->cName());
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyInCover,			false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyLookedOut,		false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyPositionHolded,	false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyEnemyDetoured,	false);
}

void CStalkerCombatPlanner::react_on_grenades		()
{
	CMemberOrder::CGrenadeReaction	&reaction = m_object->agent_manager().member().member(m_object).grenade_reaction();
	if (!reaction.m_processing)
		return;

	if (Device.dwTimeGlobal < reaction.m_time + GRENADE_INTERVAL)
		return;

//	u32							interval = AFTER_GRENADE_DESTROYED_INTERVAL;
	const CMissile				*missile = smart_cast<const CMissile*>(reaction.m_grenade);
//	if (missile && (missile->destroy_time() > Device.dwTimeGlobal))
//		interval				= missile->destroy_time() - Device.dwTimeGlobal + AFTER_GRENADE_DESTROYED_INTERVAL;
//	m_object->agent_manager().add_danger_location(reaction.m_game_object->Position(),Device.dwTimeGlobal,interval,GRENADE_RADIUS);

	if (missile && m_object->agent_manager().member().group_behaviour()) {
//		Msg						("%6d : Stalker %s : grenade reaction",Device.dwTimeGlobal,*m_object->cName());
		CEntityAlive			*initiator = smart_cast<CEntityAlive*>(Level().Objects.net_Find(reaction.m_grenade->CurrentParentID()));
		if (m_object->tfGetRelationType(initiator) == ALife::eRelationTypeEnemy)
			m_object->sound().play	(StalkerSpace::eStalkerSoundGrenadeAlarm);
		else
			if (missile->Position().distance_to(m_object->Position()) < FRIENDLY_GRENADE_ALARM_DIST)
				m_object->sound().play	(StalkerSpace::eStalkerSoundFriendlyGrenadeAlarm);
	}

	reaction.clear				();
}

void CStalkerCombatPlanner::react_on_member_death	()
{
	CMemberOrder::CMemberDeathReaction	&reaction = m_object->agent_manager().member().member(m_object).member_death_reaction();
	if (!reaction.m_processing)
		return;

	if (Device.dwTimeGlobal < reaction.m_time + TOLLS_INTERVAL)
		return;

	if (m_object->agent_manager().member().group_behaviour())
		m_object->sound().play(StalkerSpace::eStalkerSoundTolls);

	reaction.clear			();
}

void CStalkerCombatPlanner::execute				()
{
	if (first_time())
		object().agent_manager().member().register_in_combat(m_object);

	inherited::execute		();
}

void CStalkerCombatPlanner::update				()
{
	update_cover			();
	inherited::update		();
	react_on_grenades		();
	react_on_member_death	();
}

void CStalkerCombatPlanner::initialize			()
{
	inherited::initialize	();
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyInCover,			false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyLookedOut,		false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyPositionHolded,	false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyEnemyDetoured,	false);
	m_last_cover			= 0;
	m_last_enemy_id			= u16(-1);
	m_last_level_time		= 0;
	if (m_object->memory().visual().visible_now(m_object->memory().enemy().selected()) && m_object->memory().enemy().selected()->human_being())
		object().sound().play		(eStalkerSoundAlarm);
}

void CStalkerCombatPlanner::finalize			()
{
	inherited::finalize		();

	if (!object().g_Alive())
		return;

	object().memory().danger().time_line(Device.dwTimeGlobal);

#ifdef OLD_AGENT_MANAGER_BEHAVIOUR
	object().agent_manager().member().unregister_in_combat	(m_object);
#endif
	object().sound().remove_active_sounds					(u32(-1));
}

void CStalkerCombatPlanner::add_evaluators		()
{
	add_evaluator			(eWorldPropertyPureEnemy		,xr_new<CStalkerPropertyEvaluatorEnemies>			(m_object,"is_there_enemies",0));
	add_evaluator			(eWorldPropertyEnemy			,xr_new<CStalkerPropertyEvaluatorEnemies>			(m_object,"is_there_enemies_delayed",POST_COMBAT_WAIT_INTERVAL));
	add_evaluator			(eWorldPropertySeeEnemy			,xr_new<CStalkerPropertyEvaluatorSeeEnemy>			());
	add_evaluator			(eWorldPropertyItemToKill		,xr_new<CStalkerPropertyEvaluatorItemToKill>		());
	add_evaluator			(eWorldPropertyItemCanKill		,xr_new<CStalkerPropertyEvaluatorItemCanKill>		());
	add_evaluator			(eWorldPropertyFoundItemToKill	,xr_new<CStalkerPropertyEvaluatorFoundItemToKill>	());
	add_evaluator			(eWorldPropertyFoundAmmo		,xr_new<CStalkerPropertyEvaluatorFoundAmmo>			());
	add_evaluator			(eWorldPropertyReadyToKill		,xr_new<CStalkerPropertyEvaluatorReadyToKill>		());
	add_evaluator			(eWorldPropertyPanic			,xr_new<CStalkerPropertyEvaluatorPanic>				());
	add_evaluator			(eWorldPropertyInCover			,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyInCover,true,true));
	add_evaluator			(eWorldPropertyLookedOut		,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyLookedOut,true,true));
	add_evaluator			(eWorldPropertyPositionHolded	,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyPositionHolded,true,true));
	add_evaluator			(eWorldPropertyEnemyCanBeSeen	,xr_new<CStalkerPropertyEvaluatorEnemyCanBeSeen>	());
	add_evaluator			(eWorldPropertyEnemyDetoured	,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyEnemyDetoured,true,true));
}

void CStalkerCombatPlanner::add_actions			()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionGetItemToKill>	(&m_last_cover,m_object,"get_item_to_kill");
	add_condition			(action,eWorldPropertyFoundItemToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,		false);
	add_effect				(action,eWorldPropertyItemToKill,		true);
	add_effect				(action,eWorldPropertyItemCanKill,		true);
	add_operator			(eWorldOperatorGetItemToKill,			action);

	action					= xr_new<CStalkerActionMakeItemKilling>	(&m_last_cover,m_object,"make_item_killing");
	add_condition			(action,eWorldPropertyFoundAmmo,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	false);
	add_effect				(action,eWorldPropertyItemCanKill,	true);
	add_operator			(eWorldOperatorMakeItemKilling,		action);

	action					= xr_new<CStalkerActionRetreatFromEnemy>(&m_last_cover,m_object,"retreat_from_enemy");
	add_effect				(action,eWorldPropertyPureEnemy,	false);
	add_operator			(eWorldOperatorRetreatFromEnemy,	action);
	
	action					= xr_new<CStalkerActionGetReadyToKill>	(&m_last_cover,m_object,"get_ready_to_kill");
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	add_operator			(eWorldOperatorGetReadyToKill,			action);

	action					= xr_new<CStalkerActionKillEnemy>		(&m_last_cover,m_object,"kill_enemy");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyInCover,		true);
	add_condition			(action,eWorldPropertyPanic,		false);
	add_effect				(action,eWorldPropertyPureEnemy,	false);
	add_operator			(eWorldOperatorKillEnemy,				action);

	action					= xr_new<CStalkerActionTakeCover>		(&m_last_cover,m_object,"take_cover");
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyInCover,		false);
	add_effect				(action,eWorldPropertyInCover,		true);
	add_operator			(eWorldOperatorTakeCover,				action);

	action					= xr_new<CStalkerActionLookOut>			(&m_last_cover,m_object,"look_out");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyInCover,		true);
	add_condition			(action,eWorldPropertyLookedOut,	false);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_effect				(action,eWorldPropertyLookedOut,	true);
	add_operator			(eWorldOperatorLookOut,					action);

	action					= xr_new<CStalkerActionHoldPosition>	(&m_last_cover,m_object,"hold_position");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyInCover,		true);
	add_condition			(action,eWorldPropertyLookedOut,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_condition			(action,eWorldPropertyPositionHolded,false);
	add_effect				(action,eWorldPropertyInCover,		false);
	add_effect				(action,eWorldPropertyPositionHolded,true);
	add_operator			(eWorldOperatorHoldPosition,			action);

	action					= xr_new<CStalkerActionGetDistance>		(&m_last_cover,m_object,"get_distance");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyInCover,		false);
	add_condition			(action,eWorldPropertyEnemyDetoured,false);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_condition			(action,eWorldPropertyLookedOut,	true);
	add_condition			(action,eWorldPropertyPositionHolded,true);
	add_condition			(action,eWorldPropertyEnemyCanBeSeen,false);
	add_effect				(action,eWorldPropertyEnemyCanBeSeen,true);
	add_operator			(eWorldOperatorGetDistance,			action);

	action					= xr_new<CStalkerActionDetourEnemy>		(&m_last_cover,m_object,"detour_enemy");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyInCover,		false);
	add_condition			(action,eWorldPropertyEnemyDetoured,false);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_condition			(action,eWorldPropertyLookedOut,	true);
	add_condition			(action,eWorldPropertyPositionHolded,true);
//	add_condition			(action,eWorldPropertyEnemyCanBeSeen,true);
	add_effect				(action,eWorldPropertyEnemyDetoured,true);
	add_operator			(eWorldOperatorDetourEnemy,			action);

	action					= xr_new<CStalkerActionSearchEnemy>		(&m_last_cover,m_object,"search_enemy");
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_condition			(action,eWorldPropertyInCover,		false);
	add_condition			(action,eWorldPropertyLookedOut,	true);
	add_condition			(action,eWorldPropertyPositionHolded,true);
	add_condition			(action,eWorldPropertyEnemyDetoured,true);
//	add_condition			(action,eWorldPropertyEnemyCanBeSeen,true);
	add_condition			(action,eWorldPropertyPanic,		false);
	add_effect				(action,eWorldPropertyPureEnemy,	false);
	add_operator			(eWorldOperatorSearchEnemy,			action);
	action->set_inertia_time(120000);

	action					= xr_new<CStalkerActionPostCombatWait>	(&m_last_cover,m_object,"post_combat_wait");
	add_condition			(action,eWorldPropertyPureEnemy,	false);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorPostCombatWait,		action);
}
