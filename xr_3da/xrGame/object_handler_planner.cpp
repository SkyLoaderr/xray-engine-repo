////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_planner.cpp
//	Created 	: 11.03.2004
//  Modified 	: 01.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler action planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_handler_planner.h"
#include "object_property_evaluators.h"
#include "object_actions.h"
#include "ai_monster_space.h"
#include "object_handler_space.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "object_handler_planner_impl.h"
#include "weaponmagazined.h"
#include "missile.h"
#include "fooditem.h"

#ifdef _DEBUG
#	define USE_LOG
#endif

using namespace ObjectHandlerSpace;

IC	ObjectHandlerSpace::EWorldProperties CObjectHandlerPlanner::object_property(MonsterSpace::EObjectAction object_action) const
{
	switch (object_action) {
		case MonsterSpace::eObjectActionSwitch1		: return(ObjectHandlerSpace::eWorldPropertySwitch1);
		case MonsterSpace::eObjectActionSwitch2		: return(ObjectHandlerSpace::eWorldPropertySwitch2);
		case MonsterSpace::eObjectActionAim1		: return(ObjectHandlerSpace::eWorldPropertyAimingReady1);
		case MonsterSpace::eObjectActionAim2		: return(ObjectHandlerSpace::eWorldPropertyAiming2);
		case MonsterSpace::eObjectActionFire1		: return(ObjectHandlerSpace::eWorldPropertyFiring1);
		case MonsterSpace::eObjectActionFire2		: return(ObjectHandlerSpace::eWorldPropertyFiring2);
		case MonsterSpace::eObjectActionIdle		: return(ObjectHandlerSpace::eWorldPropertyIdle);
		case MonsterSpace::eObjectActionStrapped	: return(ObjectHandlerSpace::eWorldPropertyIdleStrap);
		case MonsterSpace::eObjectActionDrop		: return(ObjectHandlerSpace::eWorldPropertyDropped);
		case MonsterSpace::eObjectActionActivate	: return(ObjectHandlerSpace::eWorldPropertyIdle);
		case MonsterSpace::eObjectActionDeactivate	: return(ObjectHandlerSpace::eWorldPropertyNoItemsIdle);
		case MonsterSpace::eObjectActionAimReady1	: return(ObjectHandlerSpace::eWorldPropertyAimingReady1);
		case MonsterSpace::eObjectActionAimReady2	: return(ObjectHandlerSpace::eWorldPropertyAimingReady2);
		case MonsterSpace::eObjectActionUse			: return(ObjectHandlerSpace::eWorldPropertyUsed);
		default							: NODEFAULT;
	}
#ifdef DEBUG
	return	(ObjectHandlerSpace::eWorldPropertyDummy);
#endif
}

void CObjectHandlerPlanner::set_goal	(MonsterSpace::EObjectAction object_action, CGameObject *game_object, u32 queue_size, u32 queue_interval)
{
	EWorldProperties		goal = object_property(object_action);
	u32						condition_id = goal;

	if (game_object && (eWorldPropertyNoItemsIdle != goal)) {
		CWeapon				*weapon = smart_cast<CWeapon*>(game_object);
		if (weapon && (goal == eWorldPropertyIdleStrap) && !weapon->can_be_strapped())
			goal			= eWorldPropertyIdle;
		condition_id		= uid(game_object->ID(), goal);
	}
	else
		condition_id		= u32(eWorldPropertyNoItemsIdle);

#ifdef LOG_ACTION
	if (m_use_log) {
//		Msg					("%6d : Active item %s",Level().timeServer(),object().inventory().ActiveItem() ? *object().inventory().ActiveItem()->cName() : "no active items");
//		Msg					("%6d : Goal %s",Level().timeServer(),property2string(condition_id));
	}
#endif
	CState					condition;
	condition.add_condition	(CWorldProperty(condition_id,true));
	set_target_state		(condition);

	if (!game_object || !queue_size)
		return;

	CWeaponMagazined		*weapon = smart_cast<CWeaponMagazined*>(game_object);
	if (!weapon)
		return;

	weapon->SetQueueSize	(queue_size);
	this->action(uid(weapon->ID(),eWorldOperatorQueueWait1)).set_inertia_time(queue_interval ? queue_interval : 300);
	this->action(uid(weapon->ID(),eWorldOperatorQueueWait2)).set_inertia_time(queue_interval ? queue_interval : 300);
}

#ifdef LOG_ACTION
LPCSTR CObjectHandlerPlanner::action2string(const _action_id_type &id)
{
	LPSTR S = m_temp_string;
	if (action_object_id(id) != 0xffff)
		if (Level().Objects.net_Find(action_object_id(id)))
			strcpy	(S,*Level().Objects.net_Find(action_object_id(id))->cName());
		else
			strcpy	(S,"no_items");
	else
		strcpy	(S,"no_items");
	strcat		(S,":");
	switch (action_state_id(id)) {
		case ObjectHandlerSpace::eWorldOperatorShow			: {strcat(S,"Show");		break;}
		case ObjectHandlerSpace::eWorldOperatorHide			: {strcat(S,"Hide");		break;}
		case ObjectHandlerSpace::eWorldOperatorDrop			: {strcat(S,"Drop");		break;}
		case ObjectHandlerSpace::eWorldOperatorStrapping	: {strcat(S,"Strapping");	break;}
		case ObjectHandlerSpace::eWorldOperatorStrapping2Idle: {strcat(S,"Strapping to idle");	break;}
		case ObjectHandlerSpace::eWorldOperatorUnstrapping	: {strcat(S,"Unstrapping"); break;}
		case ObjectHandlerSpace::eWorldOperatorUnstrapping2Idle: {strcat(S,"Unstrapping to idle");	break;}
		case ObjectHandlerSpace::eWorldOperatorStrapped		: {strcat(S,"StrappedIdle");break;}
		case ObjectHandlerSpace::eWorldOperatorIdle			: {strcat(S,"Idle");		break;}
		case ObjectHandlerSpace::eWorldOperatorAim1			: {strcat(S,"Aim1");		break;}
		case ObjectHandlerSpace::eWorldOperatorAim2			: {strcat(S,"Aim2");		break;}
		case ObjectHandlerSpace::eWorldOperatorReload1		: {strcat(S,"Reload1");		break;}
		case ObjectHandlerSpace::eWorldOperatorReload2		: {strcat(S,"Reload2");		break;}
		case ObjectHandlerSpace::eWorldOperatorFire1		: {strcat(S,"Fire1");		break;}
		case ObjectHandlerSpace::eWorldOperatorFire2		: {strcat(S,"Fire2");		break;}
		case ObjectHandlerSpace::eWorldOperatorAimingReady1	: {strcat(S,"AimingReady1");break;}
		case ObjectHandlerSpace::eWorldOperatorAimingReady2	: {strcat(S,"AimingReady2");break;}
		case ObjectHandlerSpace::eWorldOperatorSwitch1		: {strcat(S,"Switch1");		break;}
		case ObjectHandlerSpace::eWorldOperatorSwitch2		: {strcat(S,"Switch2");		break;}
		case ObjectHandlerSpace::eWorldOperatorQueueWait1	: {strcat(S,"QueueWait1");	break;}
		case ObjectHandlerSpace::eWorldOperatorQueueWait2	: {strcat(S,"QueueWait1");	break;}
		case ObjectHandlerSpace::eWorldOperatorThrowStart	: {strcat(S,"ThrowStart");	break;}
		case ObjectHandlerSpace::eWorldOperatorThrowIdle	: {strcat(S,"ThrowIdle");	break;}
		case ObjectHandlerSpace::eWorldOperatorThrow		: {strcat(S,"Throwing");	break;}
		case ObjectHandlerSpace::eWorldOperatorThreaten		: {strcat(S,"Threaten");	break;}
		case ObjectHandlerSpace::eWorldOperatorPrepare		: {strcat(S,"Preparing");	break;}
		case ObjectHandlerSpace::eWorldOperatorUse			: {strcat(S,"Using");		break;}
		case ObjectHandlerSpace::eWorldOperatorGetAmmo1		: {strcat(S,"GetAmmo1");	break;}
		case ObjectHandlerSpace::eWorldOperatorGetAmmo2		: {strcat(S,"GetAmmo2");	break;}
		default												: NODEFAULT;
	}
	return		(S);
}

LPCSTR CObjectHandlerPlanner::property2string(const _condition_type &id)
{
	LPSTR S = m_temp_string;
	if (action_object_id(id) != 0xffff)
		if (Level().Objects.net_Find(action_object_id(id)))
			strcpy	(S,*Level().Objects.net_Find(action_object_id(id))->cName());
		else
			strcpy	(S,"no_items");
	else
		strcpy	(S,"no_items");
	strcat		(S,":");
	switch (action_state_id(id)) {
		case ObjectHandlerSpace::eWorldPropertyHidden		: {strcat(S,"Hidden");		break;}
		case ObjectHandlerSpace::eWorldPropertyStrapped		: {strcat(S,"Strapped");	break;}
		case ObjectHandlerSpace::eWorldPropertyStrapped2Idle: {strcat(S,"Strapped to idle");break;}
		case ObjectHandlerSpace::eWorldPropertySwitch1		: {strcat(S,"Switch1");		break;}
		case ObjectHandlerSpace::eWorldPropertySwitch2		: {strcat(S,"Switch2");		break;}
		case ObjectHandlerSpace::eWorldPropertyAimed1		: {strcat(S,"Aimed1");		break;}
		case ObjectHandlerSpace::eWorldPropertyAimed2		: {strcat(S,"Aimed2");		break;}
		case ObjectHandlerSpace::eWorldPropertyAiming1		: {strcat(S,"Aiming1");		break;}
		case ObjectHandlerSpace::eWorldPropertyAiming2		: {strcat(S,"Aiming2");		break;}
		case ObjectHandlerSpace::eWorldPropertyEmpty1		: {strcat(S,"Empty1");		break;}
		case ObjectHandlerSpace::eWorldPropertyEmpty2		: {strcat(S,"Empty2");		break;}
		case ObjectHandlerSpace::eWorldPropertyReady1		: {strcat(S,"Ready1");		break;}
		case ObjectHandlerSpace::eWorldPropertyReady2		: {strcat(S,"Ready2");		break;}
		case ObjectHandlerSpace::eWorldPropertyFiring1		: {strcat(S,"Firing1");		break;}
		case ObjectHandlerSpace::eWorldPropertyFiring2		: {strcat(S,"Firing2");		break;}
		case ObjectHandlerSpace::eWorldPropertyAimingReady1	: {strcat(S,"AimingReady1");break;}
		case ObjectHandlerSpace::eWorldPropertyAimingReady2	: {strcat(S,"AimingReady2");break;}
		case ObjectHandlerSpace::eWorldPropertyAmmo1		: {strcat(S,"Ammo1");		break;}
		case ObjectHandlerSpace::eWorldPropertyAmmo2		: {strcat(S,"Ammo2");		break;}
		case ObjectHandlerSpace::eWorldPropertyIdle			: {strcat(S,"Idle");		break;}
		case ObjectHandlerSpace::eWorldPropertyIdleStrap	: {strcat(S,"IdleStrap");	break;}
		case ObjectHandlerSpace::eWorldPropertyDropped		: {strcat(S,"Dropped");		break;}
		case ObjectHandlerSpace::eWorldPropertyQueueWait1	: {strcat(S,"QueueWait1");	break;}
		case ObjectHandlerSpace::eWorldPropertyQueueWait2	: {strcat(S,"QueueWait2");	break;}
		case ObjectHandlerSpace::eWorldPropertyThrowStarted	: {strcat(S,"ThrowStarted");break;}
		case ObjectHandlerSpace::eWorldPropertyThrowIdle	: {strcat(S,"ThrowIdle");	break;}
		case ObjectHandlerSpace::eWorldPropertyThrow		: {strcat(S,"Throwing");	break;}
		case ObjectHandlerSpace::eWorldPropertyThreaten		: {strcat(S,"Threaten");	break;}
		case ObjectHandlerSpace::eWorldPropertyPrepared		: {strcat(S,"Prepared");	break;}
		case ObjectHandlerSpace::eWorldPropertyUsed			: {strcat(S,"Used");		break;}
		case ObjectHandlerSpace::eWorldPropertyUseEnough	: {strcat(S,"UseEnough");	break;}
		case ObjectHandlerSpace::eWorldPropertyItemID		: {S[xr_strlen(S) - 1] = 0;	break;}
		default												: NODEFAULT;
	}
	return		(S);
}
#endif

void CObjectHandlerPlanner::remove_evaluators	(CObject *object)
{
#pragma todo("Dima to Dima : safe, but not optimal!")
	for (;;) {
		EVALUATOR_MAP::iterator	I = std::lower_bound(m_evaluators.begin(),m_evaluators.end(),object->ID(),CMapLocator<EVALUATOR_MAP>());
		if (!object_action((*I).first,object))
			break;
		remove_evaluator((*I).first);
	}
}

void CObjectHandlerPlanner::remove_operators	(CObject *object)
{
#pragma todo("Dima to Dima : safe, but not optimal!")
	for (;;) {
		OPERATOR_VECTOR::iterator	I = std::lower_bound(m_operators.begin(),m_operators.end(),object->ID());
		if (!object_action((*I).m_operator_id,object))
			break;
		remove_operator	((*I).m_operator_id);
	}
}

void CObjectHandlerPlanner::setup(CAI_Stalker *object)
{
	inherited::setup			(object);
	CActionBase<CAI_Stalker>	*action;

	clear						();

	m_storage.set_property		(eWorldPropertyAimed1,false);
	m_storage.set_property		(eWorldPropertyAimed2,false);
	m_storage.set_property		(eWorldPropertyUseEnough,false);
	m_storage.set_property		(eWorldPropertyStrapped,false);
	m_storage.set_property		(eWorldPropertyStrapped2Idle,false);
	
	add_evaluator				(u32(eWorldPropertyNoItems),			xr_new<CObjectPropertyEvaluatorNoItems>(m_object));
	add_evaluator				(u32(eWorldPropertyNoItemsIdle),		xr_new<CObjectPropertyEvaluatorConst>(false));
	action						= xr_new<CSObjectActionBase>(m_object,m_object,&m_storage,"no items idle");
	add_condition				(action,0xffff,eWorldPropertyItemID,true);
	add_effect					(action,0xffff,eWorldPropertyIdle,	true);
	add_operator				(u32(eWorldOperatorNoItemsIdle),action);

	set_goal					(eObjectActionIdle);

#ifdef USE_LOG
#	ifdef LOG_ACTION
		set_use_log				(!!psAI_Flags.test(aiGOAP));
#	endif
#else
#	ifdef LOG_ACTION
	set_use_log					(false);
#	endif
#endif
}

void CObjectHandlerPlanner::add_item			(CInventoryItem *inventory_item)
{
	CWeapon						*weapon		= smart_cast<CWeapon*>		(inventory_item);
	CMissile					*missile	= smart_cast<CMissile*>		(inventory_item);
	CFoodItem					*eatable	= smart_cast<CFoodItem*>	(inventory_item);

	if (weapon) {
		add_evaluators			(weapon);
		add_operators			(weapon);
	}
	else
		if (missile) {
			add_evaluators		(missile);
			add_operators		(missile);
		}
		else
			if (eatable) {
				add_evaluators	(eatable);
				add_operators	(eatable);
			}
}

void CObjectHandlerPlanner::remove_item		(CInventoryItem *inventory_item)
{
	remove_evaluators		(inventory_item);
	remove_operators		(inventory_item);
}

void CObjectHandlerPlanner::update			()
{
#ifdef USE_LOG
#	ifdef LOG_ACTION
		if ((psAI_Flags.test(aiGOAP) && !m_use_log) || (!psAI_Flags.test(aiGOAP) && m_use_log))
			set_use_log			(!!psAI_Flags.test(aiGOAP));
#	endif
#endif
	inherited::update		();
}
