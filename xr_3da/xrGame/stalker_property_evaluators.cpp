////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluators.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker property evaluators classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_decision_space.h"
#include "script_game_object.h"
#include "ai/ai_monsters_misc.h"
#include "inventory.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "item_manager.h"
#include "enemy_manager.h"

using namespace StalkerDecisionSpace;

//#define NO_ALIFE_ACTIONS

typedef CStalkerPropertyEvaluator::_value_type _value_type;

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorALife
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorALife::evaluate	()
{
#ifdef NO_ALIFE_ACTIONS
	return			(false);
#else
	return			(!!ai().get_alife());
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAlive
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorAlive::CStalkerPropertyEvaluatorAlive	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorAlive::evaluate	()
{
	return			(!!object().g_Alive());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItems
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorItems::evaluate	()
{
	return			(!!m_object->memory().item().selected());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemies
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorEnemies::CStalkerPropertyEvaluatorEnemies	(
	CAI_Stalker *object, 
	LPCSTR evaluator_name, 
	u32 time_to_wait
) : 
	inherited	(object ? object->lua_game_object() : 0,evaluator_name)
{
	m_time_to_wait	= time_to_wait;
}

_value_type CStalkerPropertyEvaluatorEnemies::evaluate	()
{
	if (m_object->memory().enemy().selected())
		return			(true);

	if (Device.dwTimeGlobal < m_object->memory().enemy().last_enemy_time() + m_time_to_wait)
		return			(true);

	return				(false);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSeeEnemy
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorSeeEnemy::evaluate	()
{
	return				(m_object->memory().enemy().selected() ? m_object->memory().visual().visible_now(m_object->memory().enemy().selected()) : false);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemToKill
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorItemToKill::evaluate	()
{
	return				(!!m_object->item_to_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemCanKill
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorItemCanKill::evaluate	()
{
	return				(m_object->item_can_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundItemToKill
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorFoundItemToKill::evaluate	()
{
	return				(m_object->remember_item_to_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundAmmo
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorFoundAmmo::evaluate	()
{
	return				(m_object->remember_ammo());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorReadyToKill
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorReadyToKill::evaluate	()
{
	return				(m_object->ready_to_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAnomaly
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorAnomaly::CStalkerPropertyEvaluatorAnomaly	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorAnomaly::evaluate	()
{
	if (!m_object->undetected_anomaly())
		return			(false);

	if (!m_object->memory().enemy().selected())
		return			(true);

	u32					result = dwfChooseAction(2000,m_object->panic_threshold(),0.f,0.f,0.f,m_object->g_Team(),m_object->g_Squad(),m_object->g_Group(),0,1,2,3,4,m_object,30.f);
	return				(!result);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorInsideAnomaly
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorInsideAnomaly::evaluate	()
{
	if (!m_object->inside_anomaly())
		return			(false);

	if (!m_object->memory().enemy().selected())
		return			(true);

	u32					result = dwfChooseAction(2000,m_object->panic_threshold(),0.f,0.f,0.f,m_object->g_Team(),m_object->g_Squad(),m_object->g_Group(),0,1,2,3,4,m_object,30.f);
	return				(!result);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorPanic
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorPanic::evaluate	()
{
	u32					result = dwfChooseAction(2000,m_object->panic_threshold(),0.f,0.f,0.f,m_object->g_Team(),m_object->g_Squad(),m_object->g_Group(),0,1,2,3,4,m_object,30.f);
	return				(!!result);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorNotEnoughFood
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorNotEnoughFood::evaluate	()
{
	return				(m_object->not_enough_food());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorCanBuyFood
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorCanBuyFood::evaluate	()
{
	return				(m_object->can_buy_food());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorNotEnoughMedikits
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorNotEnoughMedikits::evaluate	()
{
	return				(m_object->not_enough_medikits());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorCanBuyMedikit
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorCanBuyMedikit::evaluate	()
{
	return				(m_object->can_buy_medikits());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorNoOrBadWeapon
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorNoOrBadWeapon::evaluate	()
{
	return				(m_object->no_or_bad_weapon());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorCanBuyWeapon
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorCanBuyWeapon::evaluate	()
{
	return				(m_object->can_buy_weapon());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorNotEnoughAmmo
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorNotEnoughAmmo::evaluate	()
{
	return				(m_object->not_enough_ammo());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorCanBuyAmmo
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorCanBuyAmmo::evaluate	()
{
	return				(m_object->can_buy_ammo());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorHumanToDialog
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorHumanToDialog::evaluate	()
{
	return				(false);//true);//false);//true);
}
