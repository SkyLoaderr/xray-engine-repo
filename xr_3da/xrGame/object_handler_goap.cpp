////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_goap.cpp
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler using GOAP
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "missile.h"
#include "fooditem.h"
#include "weaponmagazined.h"
#include "ef_storage.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "object_handler_goap.h"
#include "object_property_evaluator_const.h"
#include "object_property_evaluator_state.h"
#include "object_property_evaluator_member.h"
#include "object_property_evaluator_ammo.h"
#include "object_property_evaluator_empty.h"
#include "object_property_evaluator_ready.h"
#include "object_property_evaluator_item_id.h"
#include "object_action_command.h"
#include "object_action_show.h"
#include "object_action_hide.h"
#include "object_action_reload.h"
#include "object_action_fire.h"
#include "object_action_aim.h"
#include "object_action_strapping.h"
#include "object_action_unstrapping.h"
#include "object_action_queue_wait.h"
#include "object_action_switch.h"
#include "object_action_drop.h"

CObjectHandlerGOAP::CObjectHandlerGOAP	()
{
	init						();
}

CObjectHandlerGOAP::~CObjectHandlerGOAP	()
{
}

void CObjectHandlerGOAP::init	()
{
}

void CObjectHandlerGOAP::Load			(LPCSTR section)
{
	inherited::Load				(section);
	CInventoryOwner::Load		(section);
	m_dwFireRandomMin  			= pSettings->r_s32(section,"FireRandomMin");
	m_dwFireRandomMax  			= pSettings->r_s32(section,"FireRandomMax");
	m_dwNoFireTimeMin  			= pSettings->r_s32(section,"NoFireTimeMin");
	m_dwNoFireTimeMax  			= pSettings->r_s32(section,"NoFireTimeMax");
	m_fMinMissDistance 			= pSettings->r_float(section,"MinMissDistance");
	m_fMinMissFactor   			= pSettings->r_float(section,"MinMissFactor");
	m_fMaxMissDistance 			= pSettings->r_float(section,"MaxMissDistance");
	m_fMaxMissFactor			= pSettings->r_float(section,"MaxMissFactor");
	m_bCanFire					= true;
	m_bHammerIsClutched			= false;
	m_dwNoFireTime				= 0;
	m_dwStartFireTime			= 0;
}

void CObjectHandlerGOAP::reinit			(CAI_Stalker *object)
{
	inherited::reinit			(object,true);
	CInventoryOwner::reinit		();
	m_aimed1					= false;
	m_aimed2					= false;
	add_evaluator				(eWorldPropertyCurItemID,	xr_new<CObjectPropertyEvaluatorItemID>(m_object));
	add_evaluator				(eWorldPropertyNoItemsIdle,	xr_new<CObjectPropertyEvaluatorConst>(m_object,m_object,false));
	CSObjectActionBase			*action = xr_new<CSObjectActionBase>(m_object,m_object);
	action->add_condition		(CWorldProperty(eWorldPropertyCurItemID,	0xffff));
	action->add_effect			(CWorldProperty(eWorldPropertyNoItemsIdle,	true));
	add_operator				(eWorldOperatorNoItemsIdle,action);
	set_goal					(eObjectActionIdle);
	set_current_action			(eWorldOperatorNoItemsIdle);
}

void CObjectHandlerGOAP::reload			(LPCSTR section)
{
	inherited::reload			(section);
	CInventoryOwner::reload		(section);
}

void CObjectHandlerGOAP::OnItemTake		(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake	(inventory_item);
	Msg							("Adding item %s (%d)",*inventory_item->cName(),inventory_item->ID());
	add_item					(inventory_item);
}

void CObjectHandlerGOAP::OnItemDrop		(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemDrop	(inventory_item);
	Msg							("Removing item %s (%d)",*inventory_item->cName(),inventory_item->ID());
	remove_item					(inventory_item);
}

void CObjectHandlerGOAP::OnItemDropUpdate	()
{
	CInventoryOwner::OnItemDropUpdate	();
}

CInventoryItem *CObjectHandlerGOAP::best_weapon() const
{
	CInventoryItem	*best_weapon = 0;
	u32				best_weapon_type = 0;
	ai().ef_storage().m_tpCurrentMember = m_object;
	TIItemSet::const_iterator	I = inventory().m_all.begin();
	TIItemSet::const_iterator	E = inventory().m_all.end();
	for ( ; I != E; ++I) {
		if ((*I)->getDestroy())
			continue;
		CWeapon		*weapon = dynamic_cast<CWeapon*>(*I);
		if (weapon && (weapon->GetAmmoCurrent() > weapon->GetAmmoMagSize()/10)) {
			ai().ef_storage().m_tpGameObject	= weapon;
			u32	current_weapon_type = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
			if (current_weapon_type > best_weapon_type) {
				best_weapon_type = current_weapon_type;
				best_weapon		 = *I;
			}
			continue;
		}
		CMissile	*missile = dynamic_cast<CMissile*>(*I);
		if (missile) {
			ai().ef_storage().m_tpGameObject	= missile;
			u32	current_weapon_type = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
			if (current_weapon_type > best_weapon_type) {
				best_weapon_type = current_weapon_type;
				best_weapon		 = *I;
			}
		}
	}
	return			(best_weapon);
}

u32 CObjectHandlerGOAP::weapon_state(const CWeapon *weapon) const
{
	if (!weapon)
		return		(u32(-1));

	switch (weapon->STATE) {
		case CWeapon::eIdle		:  {
			const CWeaponMagazined	*weapon_magazined = dynamic_cast<const CWeaponMagazined*>(weapon);
			if (!weapon_magazined || (weapon_magazined->GetAmmoElapsed() && !weapon_magazined->IsMisfire()))
				return(CWeapon::eIdle);
			if (!weapon_magazined->GetAmmoElapsed() && (current_action_state_id() != eObjectActionReload1))
				return(CWeapon::eMagEmpty);
			if (weapon_magazined->IsMisfire() && (current_action_state_id() != eObjectActionReload1))
				return(CWeapon::eMisfire);
			
			switch (current_action_state_id()) {
				case eObjectActionIdle		: return(CWeapon::eIdle		);
				case eObjectActionFire1		: return(CWeapon::eFire		);
				case eObjectActionFire2		: return(CWeapon::eFire2	);
				case eObjectActionReload1	: return(CWeapon::eReload	);
				case eObjectActionShow		: return(CWeapon::eShowing	);
				case eObjectActionHide		: return(CWeapon::eHiding	);
			}
		}
		default : 
			return(weapon->STATE);
	}
}

void CObjectHandlerGOAP::add_item			(CInventoryItem *inventory_item)
{
	CWeapon						*weapon		= dynamic_cast<CWeapon*>		(inventory_item);
	CMissile					*missile	= dynamic_cast<CMissile*>		(inventory_item);
	CEatableItem				*eatable	= dynamic_cast<CEatableItem*>	(inventory_item);

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

void CObjectHandlerGOAP::remove_item		(CInventoryItem *inventory_item)
{
	remove_evaluators		(inventory_item);
	remove_operators		(inventory_item);
}

void CObjectHandlerGOAP::remove_evaluators	(CObject *object)
{
#pragma todo("Dima to Dima : safe, but not optimal!")
	for (;;) {
		EVALUATOR_MAP::iterator	I = std::lower_bound(m_evaluators.begin(),m_evaluators.end(),object->ID(),CMapLocator<EVALUATOR_MAP>());
		if (!object_action((*I).first,object))
			break;
		remove_evaluator((*I).first);
	}
}

void CObjectHandlerGOAP::remove_operators	(CObject *object)
{
#pragma todo("Dima to Dima : safe, but not optimal!")
	for (;;) {
		OPERATOR_VECTOR::iterator	I = std::lower_bound(m_operators.begin(),m_operators.end(),object->ID());
		if (!object_action((*I).m_operator_id,object))
			break;
		remove_operator	((*I).m_operator_id);
	}
}

#ifdef DEBUG
LPCSTR action2string(const u32 id)
{
	static	string4096 S;
	if ((id & 0xffff) != 0xffff)
		if (Level().Objects.net_Find(id & 0xffff))
			strcpy	(S,*Level().Objects.net_Find(id & 0xffff)->cName());
		else
			strcpy	(S,"no_items");
	else
		strcpy	(S,"no_items");
	strcat		(S,":");
	switch (id >> 16) {
		case CObjectHandlerGOAP::eWorldOperatorShow			: {strcat(S,"Show");		break;}
		case CObjectHandlerGOAP::eWorldOperatorHide			: {strcat(S,"Hide");		break;}
		case CObjectHandlerGOAP::eWorldOperatorDrop			: {strcat(S,"Drop");		break;}
		case CObjectHandlerGOAP::eWorldOperatorStrapping	: {strcat(S,"Strapping");	break;}
		case CObjectHandlerGOAP::eWorldOperatorStrapped		: {strcat(S,"StrappedIdle");break;}
		case CObjectHandlerGOAP::eWorldOperatorUnstrapping	: {strcat(S,"Unstrapping"); break;}
		case CObjectHandlerGOAP::eWorldOperatorIdle			: {strcat(S,"Idle");		break;}
		case CObjectHandlerGOAP::eWorldOperatorAim1			: {strcat(S,"Aim1");		break;}
		case CObjectHandlerGOAP::eWorldOperatorAim2			: {strcat(S,"Aim2");		break;}
		case CObjectHandlerGOAP::eWorldOperatorReload1		: {strcat(S,"Reload1");		break;}
		case CObjectHandlerGOAP::eWorldOperatorReload2		: {strcat(S,"Reload2");		break;}
		case CObjectHandlerGOAP::eWorldOperatorFire1		: {strcat(S,"Fire1");		break;}
		case CObjectHandlerGOAP::eWorldOperatorFire2		: {strcat(S,"Fire2");		break;}
		case CObjectHandlerGOAP::eWorldOperatorSwitch1		: {strcat(S,"Switch1");		break;}
		case CObjectHandlerGOAP::eWorldOperatorSwitch2		: {strcat(S,"Switch2");		break;}
		case CObjectHandlerGOAP::eWorldOperatorQueueWait1	: {strcat(S,"QueueWait1");	break;}
		case CObjectHandlerGOAP::eWorldOperatorQueueWait2	: {strcat(S,"QueueWait1");	break;}
		default							: NODEFAULT;
	}
	return		(S);
}

LPCSTR property2string(const u32 id)
{
	static	string4096 S;
	if ((id & 0xffff) != 0xffff)
		if (Level().Objects.net_Find(id & 0xffff))
			strcpy	(S,*Level().Objects.net_Find(id & 0xffff)->cName());
		else
			strcpy	(S,"no_items");
	else
		strcpy	(S,"no_items");
	strcat		(S,":");
	switch (id >> 16) {
		case CObjectHandlerGOAP::eWorldPropertyHidden		: {strcat(S,"Hidden");		break;}
		case CObjectHandlerGOAP::eWorldPropertyStrapping	: {strcat(S,"Strapping");	break;}
		case CObjectHandlerGOAP::eWorldPropertyStrapped		: {strcat(S,"Strapped");	break;}
		case CObjectHandlerGOAP::eWorldPropertyUnstrapping	: {strcat(S,"Unstrapping");	break;}
		case CObjectHandlerGOAP::eWorldPropertySwitch1		: {strcat(S,"Switch1");		break;}
		case CObjectHandlerGOAP::eWorldPropertySwitch2		: {strcat(S,"Switch2");		break;}
		case CObjectHandlerGOAP::eWorldPropertyAimed1		: {strcat(S,"Aimed1");		break;}
		case CObjectHandlerGOAP::eWorldPropertyAimed2		: {strcat(S,"Aimed2");		break;}
		case CObjectHandlerGOAP::eWorldPropertyAiming1		: {strcat(S,"Aiming1");		break;}
		case CObjectHandlerGOAP::eWorldPropertyAiming2		: {strcat(S,"Aiming2");		break;}
		case CObjectHandlerGOAP::eWorldPropertyEmpty1		: {strcat(S,"Empty1");		break;}
		case CObjectHandlerGOAP::eWorldPropertyEmpty2		: {strcat(S,"Empty2");		break;}
		case CObjectHandlerGOAP::eWorldPropertyReady1		: {strcat(S,"Ready1");		break;}
		case CObjectHandlerGOAP::eWorldPropertyReady2		: {strcat(S,"Ready2");		break;}
		case CObjectHandlerGOAP::eWorldPropertyFiring1		: {strcat(S,"Firing1");		break;}
		case CObjectHandlerGOAP::eWorldPropertyFiring2		: {strcat(S,"Firing2");		break;}
		case CObjectHandlerGOAP::eWorldPropertyAmmo1		: {strcat(S,"Ammo1");		break;}
		case CObjectHandlerGOAP::eWorldPropertyAmmo2		: {strcat(S,"Ammo2");		break;}
		case CObjectHandlerGOAP::eWorldPropertyIdle			: {strcat(S,"Idle");		break;}
		case CObjectHandlerGOAP::eWorldPropertyIdleStrap	: {strcat(S,"IdleStrap");	break;}
		case CObjectHandlerGOAP::eWorldPropertyDropped		: {strcat(S,"Dropped");		break;}
		case CObjectHandlerGOAP::eWorldPropertyQueueWait1	: {strcat(S,"QueueWait1");	break;}
		case CObjectHandlerGOAP::eWorldPropertyQueueWait2	: {strcat(S,"QueueWait2");	break;}
		case CObjectHandlerGOAP::eWorldPropertyCurrentItemID: {strcat(S,"CurItemID");	break;}
		default							: NODEFAULT;
	}
	return		(S);
}
#endif

void CObjectHandlerGOAP::add_evaluators		(CWeapon *weapon)
{
	u16					id = weapon->ID();
	// dynamic state properties
	add_evaluator		(uid(id,eWorldPropertyHidden)		,xr_new<CObjectPropertyEvaluatorState>(weapon,m_object,CWeapon::eHidden));
	// dynamic member properties
	add_evaluator		(uid(id,eWorldPropertyAimed1)		,xr_new<CObjectPropertyEvaluatorMember>(weapon,m_object,&m_aimed1));
	add_evaluator		(uid(id,eWorldPropertyAimed2)		,xr_new<CObjectPropertyEvaluatorMember>(weapon,m_object,&m_aimed2));
	// dynamic properties
	add_evaluator		(uid(id,eWorldPropertyAmmo1)		,xr_new<CObjectPropertyEvaluatorAmmo>(weapon,m_object,0));
	add_evaluator		(uid(id,eWorldPropertyAmmo2)		,xr_new<CObjectPropertyEvaluatorAmmo>(weapon,m_object,1));
	add_evaluator		(uid(id,eWorldPropertyEmpty1)		,xr_new<CObjectPropertyEvaluatorEmpty>(weapon,m_object,0));
	add_evaluator		(uid(id,eWorldPropertyEmpty2)		,xr_new<CObjectPropertyEvaluatorEmpty>(weapon,m_object,1));
	add_evaluator		(uid(id,eWorldPropertyReady1)		,xr_new<CObjectPropertyEvaluatorReady>(weapon,m_object,0));
	add_evaluator		(uid(id,eWorldPropertyReady2)		,xr_new<CObjectPropertyEvaluatorReady>(weapon,m_object,1));
	// temporarile const properties
	add_evaluator		(uid(id,eWorldPropertyQueueWait1)	,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyQueueWait2)	,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertySwitch1)		,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,true));
	add_evaluator		(uid(id,eWorldPropertySwitch2)		,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyStrapping)	,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyStrapped)		,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyUnstrapping)	,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	// const properties
	add_evaluator		(uid(id,eWorldPropertyFiring1)		,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyFiring2)		,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyIdle)			,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyIdleStrap)	,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyDropped)		,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyAiming1)		,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
	add_evaluator		(uid(id,eWorldPropertyAiming2)		,xr_new<CObjectPropertyEvaluatorConst>(weapon,m_object,false));
}

void CObjectHandlerGOAP::add_operators		(CWeapon *weapon)
{
	u16					id = weapon->ID();
	CActionBase<CAI_Stalker>	*action;
	
	// show
	action				= xr_new<CObjectActionShow>(weapon,m_object);
	add_condition		(action,id,eWorldPropertyHidden,	true);
	action->add_condition(CWorldProperty(eWorldPropertyCurItemID,0xffff));
	action->add_effect	(CWorldProperty(eWorldPropertyCurItemID,id));
	add_effect			(action,id,eWorldPropertyHidden,	false);
	add_operator		(uid(id,eWorldOperatorShow),		action);

	// hide
	action				= xr_new<CObjectActionHide>(weapon,m_object);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	action->add_effect	(CWorldProperty(eWorldPropertyCurItemID,0xffff));
	action->add_condition(CWorldProperty(eWorldPropertyCurItemID,id));
	add_effect			(action,id,eWorldPropertyHidden,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorHide),		action);

	// drop
	action				= xr_new<CObjectActionDrop>(weapon,m_object);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyDropped,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorDrop),		action);

	// idle
	action				= xr_new<CSObjectActionBase>(weapon,m_object);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyIdle,		true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorIdle),		action);

	// strapping
	action				= xr_new<CObjectActionStrapping>(weapon,m_object);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyStrapped,	false);
	add_effect			(action,id,eWorldPropertyStrapped,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorStrapping),	action);

	// unstrapping
	action				= xr_new<CObjectActionUnstrapping>(weapon,m_object);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyStrapped,	true);
	add_effect			(action,id,eWorldPropertyStrapped,	false);
	add_operator		(uid(id,eWorldOperatorUnstrapping),	action);

	// strapped
	action				= xr_new<CSObjectActionBase>(weapon,m_object);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyIdleStrap,	true);
	add_operator		(uid(id,eWorldOperatorStrapped),	action);

	// aim1
	action				= xr_new<CObjectActionAim>(weapon,m_object,0);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	true);
	add_effect			(action,id,eWorldPropertyAiming1,	true);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorAim1),		action);

	// aim2
	action				= xr_new<CObjectActionAim>(weapon,m_object,1);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch2,	true);
	add_effect			(action,id,eWorldPropertyAimed2,	true);
	add_effect			(action,id,eWorldPropertyAiming2,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_operator		(uid(id,eWorldOperatorAim2),		action);

	// aim_queue1
	action				= xr_new<CObjectActionQueueWait>(weapon,m_object,0);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_condition		(action,id,eWorldPropertyQueueWait1,false);
	add_effect			(action,id,eWorldPropertyQueueWait1,true);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorQueueWait1),	action);

	// aim_queue2
	action				= xr_new<CObjectActionQueueWait>(weapon,m_object,1);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_condition		(action,id,eWorldPropertyQueueWait2,false);
	add_effect			(action,id,eWorldPropertyQueueWait2,true);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorQueueWait2),	action);

	// fire1
	action				= xr_new<CObjectActionFire>(weapon,m_object,0);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyEmpty1,	false);
	add_condition		(action,id,eWorldPropertyAimed1,	true);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_condition		(action,id,eWorldPropertyQueueWait1,false);
	add_effect			(action,id,eWorldPropertyFiring1,	true);
	add_operator		(uid(id,eWorldOperatorFire1),		action);

	// fire2
	action				= xr_new<CObjectActionFire>(weapon,m_object,1);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyEmpty2,	false);
	add_condition		(action,id,eWorldPropertyAimed2,	true);
	add_condition		(action,id,eWorldPropertySwitch2,	true);
	add_condition		(action,id,eWorldPropertyQueueWait2,false);
	add_effect			(action,id,eWorldPropertyFiring2,	true);
	add_operator		(uid(id,eWorldOperatorFire2),		action);

	// reload1
	action				= xr_new<CObjectActionReload>(weapon,m_object,0);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyReady1,	false);
	add_condition		(action,id,eWorldPropertyAmmo1,		true);
	add_effect			(action,id,eWorldPropertyEmpty1,	false);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorReload1),		action);

	// reload2
	action				= xr_new<CObjectActionReload>(weapon,m_object,1);
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyReady2,	false);
	add_condition		(action,id,eWorldPropertyAmmo2,		true);
	add_effect			(action,id,eWorldPropertyEmpty2,	false);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorReload2),		action);

	// switch1
	action				= xr_new<CObjectActionSwitch>(weapon,m_object,0);
	add_condition		(action,id,eWorldPropertySwitch1,	false);
	add_condition		(action,id,eWorldPropertySwitch2,	true);
	add_effect			(action,id,eWorldPropertySwitch1,	true);
	add_effect			(action,id,eWorldPropertySwitch2,	false);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorSwitch1),		action);

	// switch2
	action				= xr_new<CObjectActionSwitch>(weapon,m_object,1);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_condition		(action,id,eWorldPropertySwitch2,	false);
	add_effect			(action,id,eWorldPropertySwitch1,	false);
	add_effect			(action,id,eWorldPropertySwitch2,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorSwitch2),		action);

	this->action(uid(id,eWorldOperatorAim1)).set_inertia_time(1000);
	this->action(uid(id,eWorldOperatorAim2)).set_inertia_time(1000);
}

void CObjectHandlerGOAP::add_evaluators		(CMissile *missile)
{
}

void CObjectHandlerGOAP::add_operators		(CMissile *missile)
{
}

void CObjectHandlerGOAP::add_evaluators		(CEatableItem *eatable_item)
{
}

void CObjectHandlerGOAP::add_operators		(CEatableItem *eatable_item)
{
}

void CObjectHandlerGOAP::set_goal	(MonsterSpace::EObjectAction object_action, CGameObject *game_object)
{
	EWorldProperties		goal = object_property(object_action);
	u32						condition_id = goal;

	if (game_object && (eWorldPropertyNoItemsIdle != goal))
		condition_id		= uid(game_object->ID(), goal);
	else
		condition_id		= eWorldPropertyNoItemsIdle;

#ifdef LOG_ACTION
	Msg						("%6d : Goal %s",Level().timeServer(),property2string(condition_id));
#endif
	CState					condition;
	condition.add_condition	(CWorldProperty(condition_id,true));
	set_target_state		(condition);
}

void CObjectHandlerGOAP::update(u32 time_delta)
{
	inherited::update			(time_delta);
#ifdef LOG_ACTION
	if (!solution().empty()) {
		// printing current world state
		{
			Msg						("%6d : Current world state",Level().timeServer());
			EVALUATOR_MAP::const_iterator	I = evaluators().begin();
			EVALUATOR_MAP::const_iterator	E = evaluators().end();
			for ( ; I != E; ++I) {
				xr_vector<COperatorCondition>::const_iterator J = std::lower_bound(current_state().conditions().begin(),current_state().conditions().end(),CWorldProperty((*I).first,false));
				if (action_state_id((*I).first) == eWorldPropertyCurrentItemID) {
					if ((J != current_state().conditions().end()) && ((*J).condition() == (*I).first))
						Msg			("%5d : %s",(*J).value(),property2string((*I).first));
					else
						Msg			("%5c : %s",'?',property2string((*I).first));
					continue;
				}
				char				temp = '?';
				if ((J != current_state().conditions().end()) && ((*J).condition() == (*I).first))
					temp				= (*J).value() ? '+' : '-';
				Msg					("%5c : %s",temp,property2string((*I).first));
			}
		}
		// printing target world state
		{
			Msg						("%6d : Target world state",Level().timeServer());
			EVALUATOR_MAP::const_iterator	I = evaluators().begin();
			EVALUATOR_MAP::const_iterator	E = evaluators().end();
			for ( ; I != E; ++I) {
				xr_vector<COperatorCondition>::const_iterator J = std::lower_bound(target_state().conditions().begin(),target_state().conditions().end(),CWorldProperty((*I).first,false));
				if (action_state_id((*I).first) == eWorldPropertyCurItemID) {
					if ((J != target_state().conditions().end()) && ((*J).condition() == (*I).first))
						Msg			("%5d : %s",(*J).value(),property2string((*I).first));
					else
						Msg			("%5c : %s",'?',property2string((*I).first));
					continue;
				}
				char				temp = '?';
				if ((J != target_state().conditions().end()) && ((*J).condition() == (*I).first))
					temp				= (*J).value() ? '+' : '-';
				Msg					("%5c : %s",temp,property2string((*I).first));
			}
		}
		// printing solution
		Msg						("%6d : Solution",Level().timeServer());
		for (int i=0; i<(int)solution().size(); ++i)
			Msg					("%s",action2string(solution()[i]));
	}
#endif
}