////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler.cpp
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "missile.h"
#include "fooditem.h"
#include "weaponmagazined.h"
#include "ef_storage.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "object_handler.h"
#include "object_property_evaluators.h"
#include "object_actions.h"
#include "torch.h"
#include "xr_level_controller.h"

using namespace ObjectHandlerSpace;

CObjectHandler::CObjectHandler	()
{
	init						();
}

CObjectHandler::~CObjectHandler	()
{
}

void CObjectHandler::init	()
{
}

void CObjectHandler::Load			(LPCSTR section)
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

void CObjectHandler::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object,true);
	CInventoryOwner::reinit			();
	CActionBase<CAI_Stalker>		*action;

	m_storage.set_property			(eWorldPropertyAimed1,false);
	m_storage.set_property			(eWorldPropertyAimed2,false);
	m_storage.set_property			(eWorldPropertyUseEnough,false);
	
	add_evaluator					(u32(eWorldPropertyNoItems),			xr_new<CObjectPropertyEvaluatorNoItems>(m_object));
	add_evaluator					(u32(eWorldPropertyNoItemsIdle),		xr_new<CObjectPropertyEvaluatorConst>(false));
	action							= xr_new<CSObjectActionBase>(m_object,m_object,&m_storage,"no items idle");
	add_condition					(action,0xffff,eWorldPropertyItemID,true);
	add_effect						(action,0xffff,eWorldPropertyIdle,	true);
	add_operator					(u32(eWorldOperatorNoItemsIdle),action);

	set_goal						(eObjectActionIdle);

#ifdef LOG_ACTION
//	m_use_log						= true;
#endif
}

void CObjectHandler::reload			(LPCSTR section)
{
	inherited::reload			(section);
	CInventoryOwner::reload		(section);
}

void CObjectHandler::OnItemTake		(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake	(inventory_item);
	add_item					(inventory_item);

	CTorch						*torch = dynamic_cast<CTorch*>(inventory_item);
	if (torch)
		torch->Switch			(true);
}

void CObjectHandler::OnItemDrop		(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemDrop	(inventory_item);
	remove_item					(inventory_item);

	CTorch						*torch = dynamic_cast<CTorch*>(inventory_item);
	if (torch)
		torch->Switch			(false);
}

void CObjectHandler::OnItemDropUpdate	()
{
	CInventoryOwner::OnItemDropUpdate	();
}

CInventoryItem *CObjectHandler::best_weapon() const
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
		if (weapon && (weapon->GetAmmoCurrent() > 0*weapon->GetAmmoMagSize()/10)) {
			ai().ef_storage().m_tpGameObject	= weapon;
			u32	current_weapon_type = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
			if (current_weapon_type > best_weapon_type) {
				best_weapon_type = current_weapon_type;
				best_weapon		 = *I;
			}
			ai().ef_storage().m_tpGameObject	= 0;
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
			ai().ef_storage().m_tpGameObject	= 0;
		}
	}
	return			(best_weapon);
}

u32 CObjectHandler::weapon_state(const CWeapon *weapon) const
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

void CObjectHandler::add_item			(CInventoryItem *inventory_item)
{
	CWeapon						*weapon		= dynamic_cast<CWeapon*>		(inventory_item);
	CMissile					*missile	= dynamic_cast<CMissile*>		(inventory_item);
	CFoodItem					*eatable	= dynamic_cast<CFoodItem*>	(inventory_item);

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

void CObjectHandler::remove_item		(CInventoryItem *inventory_item)
{
	remove_evaluators		(inventory_item);
	remove_operators		(inventory_item);
}

void CObjectHandler::remove_evaluators	(CObject *object)
{
#pragma todo("Dima to Dima : safe, but not optimal!")
	for (;;) {
		EVALUATOR_MAP::iterator	I = std::lower_bound(m_evaluators.begin(),m_evaluators.end(),object->ID(),CMapLocator<EVALUATOR_MAP>());
		if (!object_action((*I).first,object))
			break;
		remove_evaluator((*I).first);
	}
}

void CObjectHandler::remove_operators	(CObject *object)
{
#pragma todo("Dima to Dima : safe, but not optimal!")
	for (;;) {
		OPERATOR_VECTOR::iterator	I = std::lower_bound(m_operators.begin(),m_operators.end(),object->ID());
		if (!object_action((*I).m_operator_id,object))
			break;
		remove_operator	((*I).m_operator_id);
	}
}

#ifdef LOG_ACTION
LPCSTR CObjectHandler::action2string(const _action_id_type &id)
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
		case ObjectHandlerSpace::eWorldOperatorStrapped		: {strcat(S,"StrappedIdle");break;}
		case ObjectHandlerSpace::eWorldOperatorUnstrapping	: {strcat(S,"Unstrapping"); break;}
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
		case ObjectHandlerSpace::eWorldOperatorPrepare		: {strcat(S,"Preparing");	break;}
		case ObjectHandlerSpace::eWorldOperatorUse			: {strcat(S,"Using");		break;}
		case ObjectHandlerSpace::eWorldOperatorGetAmmo1		: {strcat(S,"GetAmmo1");	break;}
		case ObjectHandlerSpace::eWorldOperatorGetAmmo2		: {strcat(S,"GetAmmo2");	break;}
		default												: NODEFAULT;
	}
	return		(S);
}

LPCSTR CObjectHandler::property2string(const _condition_type &id)
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
		case ObjectHandlerSpace::eWorldPropertyStrapping	: {strcat(S,"Strapping");	break;}
		case ObjectHandlerSpace::eWorldPropertyStrapped		: {strcat(S,"Strapped");	break;}
		case ObjectHandlerSpace::eWorldPropertyUnstrapping	: {strcat(S,"Unstrapping");	break;}
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

void CObjectHandler::add_evaluators		(CWeapon *weapon)
{
	u16					id = weapon->ID();
	// dynamic state properties
	add_evaluator		(uid(id,eWorldPropertyHidden)		,xr_new<CObjectPropertyEvaluatorState>(weapon,m_object,CWeapon::eHidden));
	// dynamic member properties
	add_evaluator		(uid(id,eWorldPropertyAimed1)		,xr_new<CObjectPropertyEvaluatorMember>(&m_storage,eWorldPropertyAimed1,true));
	add_evaluator		(uid(id,eWorldPropertyAimed2)		,xr_new<CObjectPropertyEvaluatorMember>(&m_storage,eWorldPropertyAimed2,true));
	// dynamic properties
	add_evaluator		(uid(id,eWorldPropertyAmmo1)		,xr_new<CObjectPropertyEvaluatorAmmo>(weapon,m_object,0));
	add_evaluator		(uid(id,eWorldPropertyAmmo2)		,xr_new<CObjectPropertyEvaluatorAmmo>(weapon,m_object,1));
	add_evaluator		(uid(id,eWorldPropertyEmpty1)		,xr_new<CObjectPropertyEvaluatorEmpty>(weapon,m_object,0));
	add_evaluator		(uid(id,eWorldPropertyEmpty2)		,xr_new<CObjectPropertyEvaluatorEmpty>(weapon,m_object,1));
	add_evaluator		(uid(id,eWorldPropertyReady1)		,xr_new<CObjectPropertyEvaluatorReady>(weapon,m_object,0));
	add_evaluator		(uid(id,eWorldPropertyReady2)		,xr_new<CObjectPropertyEvaluatorReady>(weapon,m_object,1));
	// temporarile const properties
	add_evaluator		(uid(id,eWorldPropertyQueueWait1)	,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyQueueWait2)	,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertySwitch1)		,xr_new<CObjectPropertyEvaluatorConst>(true));
	add_evaluator		(uid(id,eWorldPropertySwitch2)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyStrapping)	,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyStrapped)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyUnstrapping)	,xr_new<CObjectPropertyEvaluatorConst>(false));
	// const properties
	add_evaluator		(uid(id,eWorldPropertyFiring1)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyFiring2)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyIdle)			,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyIdleStrap)	,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyDropped)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyAiming1)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyAiming2)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyAimingReady1)	,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyAimingReady2)	,xr_new<CObjectPropertyEvaluatorConst>(false));
}

void CObjectHandler::add_operators		(CWeapon *weapon)
{
	u16					id = weapon->ID(), ff = 0xffff;
	CActionBase<CAI_Stalker>	*action;
	
	// show
	action				= xr_new<CObjectActionShow>(weapon,m_object,&m_storage,"show");
	add_condition		(action,id,eWorldPropertyHidden,	true);
	add_condition		(action,ff,eWorldPropertyItemID,	true);
	add_effect			(action,ff,eWorldPropertyItemID,	false);
	add_effect			(action,id,eWorldPropertyHidden,	false);
	add_operator		(uid(id,eWorldOperatorShow),		action);

	// hide
	action				= xr_new<CObjectActionHide>(weapon,m_object,&m_storage,"hide");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,ff,eWorldPropertyItemID,	false);
	add_effect			(action,ff,eWorldPropertyItemID,	true);
	add_effect			(action,id,eWorldPropertyHidden,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorHide),		action);

	// drop
	action				= xr_new<CObjectActionDrop>(weapon,m_object,&m_storage,"drop");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyDropped,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorDrop),		action);

	// idle
	action				= xr_new<CSObjectActionBase>(weapon,m_object,&m_storage,"idle");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyIdle,		true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorIdle),		action);

	// strapping
	action				= xr_new<CObjectActionStrapping>(weapon,m_object,&m_storage,"strapping");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyStrapped,	false);
	add_effect			(action,id,eWorldPropertyStrapped,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorStrapping),	action);

	// unstrapping
	action				= xr_new<CObjectActionUnstrapping>(weapon,m_object,&m_storage,"unstrapping");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyStrapped,	true);
	add_effect			(action,id,eWorldPropertyStrapped,	false);
	add_operator		(uid(id,eWorldOperatorUnstrapping),	action);

	// strapped
	action				= xr_new<CSObjectActionBase>(weapon,m_object,&m_storage,"strapped");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyIdleStrap,	true);
	add_operator		(uid(id,eWorldOperatorStrapped),	action);

	// aim1
	action				= xr_new<CObjectActionAim>(weapon,m_object,&m_storage,eWorldPropertyAimed1,true,"aim1");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	true);
	add_effect			(action,id,eWorldPropertyAiming1,	true);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorAim1),		action);

	// aim2
	action				= xr_new<CObjectActionAim>(weapon,m_object,&m_storage,eWorldPropertyAimed2,true,"aim2");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch2,	true);
	add_effect			(action,id,eWorldPropertyAimed2,	true);
	add_effect			(action,id,eWorldPropertyAiming2,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_operator		(uid(id,eWorldOperatorAim2),		action);

	// aim_queue1
	action				= xr_new<CObjectActionQueueWait>(weapon,m_object,&m_storage,0,"aim_queue1");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_condition		(action,id,eWorldPropertyQueueWait1,false);
	add_effect			(action,id,eWorldPropertyQueueWait1,true);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorQueueWait1),	action);

	// aim_queue2
	action				= xr_new<CObjectActionQueueWait>(weapon,m_object,&m_storage,1,"aim_queue2");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_condition		(action,id,eWorldPropertyQueueWait2,false);
	add_effect			(action,id,eWorldPropertyQueueWait2,true);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorQueueWait2),	action);

	// fire1
	action				= xr_new<CObjectActionFire>(weapon,m_object,&m_storage,0,"fire1");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyReady1,	true);
	add_condition		(action,id,eWorldPropertyEmpty1,	false);
	add_condition		(action,id,eWorldPropertyAimed1,	true);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_condition		(action,id,eWorldPropertyQueueWait1,false);
	add_effect			(action,id,eWorldPropertyFiring1,	true);
	add_operator		(uid(id,eWorldOperatorFire1),		action);

	// fire2
	action				= xr_new<CObjectActionFire>(weapon,m_object,&m_storage,1,"fire2");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyReady2,	true);
	add_condition		(action,id,eWorldPropertyEmpty2,	false);
	add_condition		(action,id,eWorldPropertyAimed2,	true);
	add_condition		(action,id,eWorldPropertySwitch2,	true);
	add_condition		(action,id,eWorldPropertyQueueWait2,false);
	add_effect			(action,id,eWorldPropertyFiring2,	true);
	add_operator		(uid(id,eWorldOperatorFire2),		action);

	// reload1
	action				= xr_new<CObjectActionReload>(weapon,m_object,&m_storage,0,"reload1");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyReady1,	false);
	add_condition		(action,id,eWorldPropertyAmmo1,		true);
	add_effect			(action,id,eWorldPropertyEmpty1,	false);
	add_effect			(action,id,eWorldPropertyReady1,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorReload1),		action);

	// reload2
	action				= xr_new<CObjectActionReload>(weapon,m_object,&m_storage,1,"reload2");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyReady2,	false);
	add_condition		(action,id,eWorldPropertyAmmo2,		true);
	add_effect			(action,id,eWorldPropertyEmpty2,	false);
	add_effect			(action,id,eWorldPropertyReady2,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorReload2),		action);

	// switch1
	action				= xr_new<CObjectActionSwitch>(weapon,m_object,&m_storage,0,"switch1");
	add_condition		(action,id,eWorldPropertySwitch1,	false);
	add_condition		(action,id,eWorldPropertySwitch2,	true);
	add_effect			(action,id,eWorldPropertySwitch1,	true);
	add_effect			(action,id,eWorldPropertySwitch2,	false);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorSwitch1),		action);

	// switch2
	action				= xr_new<CObjectActionSwitch>(weapon,m_object,&m_storage,1,"switch2");
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_condition		(action,id,eWorldPropertySwitch2,	false);
	add_effect			(action,id,eWorldPropertySwitch1,	false);
	add_effect			(action,id,eWorldPropertySwitch2,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorSwitch2),		action);

	// aiming ready1
	action				= xr_new<CObjectActionAim>(weapon,m_object,&m_storage,eWorldPropertyAimed1,true,"aim_ready1");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch1,	true);
	add_condition		(action,id,eWorldPropertyReady1,	true);
	add_effect			(action,id,eWorldPropertyAimed1,	true);
	add_effect			(action,id,eWorldPropertyAimingReady1,true);
	add_effect			(action,id,eWorldPropertyAimed2,	false);
	add_operator		(uid(id,eWorldOperatorAimingReady1),action);

	// aiming ready2
	action				= xr_new<CObjectActionAim>(weapon,m_object,&m_storage,eWorldPropertyAimed2,true,"aim_ready2");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertySwitch2,	true);
	add_effect			(action,id,eWorldPropertyAimed2,	true);
	add_effect			(action,id,eWorldPropertyAimingReady2,true);
	add_effect			(action,id,eWorldPropertyAimed1,	false);
	add_operator		(uid(id,eWorldOperatorAimingReady2),action);

	// fake action get ammo
	action				= xr_new<CSObjectActionBase>(weapon,m_object,&m_storage,"fake_get_ammo1");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyAmmo1,		false);
	add_effect			(action,id,eWorldPropertyAmmo1,		true);
	add_operator		(uid(id,eWorldOperatorGetAmmo1),	action);

	action				= xr_new<CSObjectActionBase>(weapon,m_object,&m_storage,"fake_get_ammo2");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyAmmo2,		false);
	add_effect			(action,id,eWorldPropertyAmmo2,		true);
	add_operator		(uid(id,eWorldOperatorGetAmmo2),	action);

	this->action(uid(id,eWorldOperatorAim1)).set_inertia_time(1000);
	this->action(uid(id,eWorldOperatorAim2)).set_inertia_time(1000);
	this->action(uid(id,eWorldOperatorAimingReady1)).set_inertia_time(1000);
	this->action(uid(id,eWorldOperatorAimingReady2)).set_inertia_time(1000);
}

void CObjectHandler::add_evaluators		(CMissile *missile)
{
	u16					id = missile->ID();
	// dynamic state properties
	add_evaluator		(uid(id,eWorldPropertyHidden)		,xr_new<CObjectPropertyEvaluatorMissile>(missile,m_object,MS_HIDDEN));
	add_evaluator		(uid(id,eWorldPropertyThrowStarted)	,xr_new<CObjectPropertyEvaluatorMissile>(missile,m_object,MS_THREATEN));
	add_evaluator		(uid(id,eWorldPropertyThrowIdle)	,xr_new<CObjectPropertyEvaluatorMissile>(missile,m_object,MS_THROW));
	add_evaluator		(uid(id,eWorldPropertyThrow)		,xr_new<CObjectPropertyEvaluatorMissile>(missile,m_object,MS_END));

	// const properties
	add_evaluator		(uid(id,eWorldPropertyDropped)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyFiring1)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyIdle)			,xr_new<CObjectPropertyEvaluatorConst>(false));
}

void CObjectHandler::add_operators		(CMissile *missile)
{
	u16					id = missile->ID(), ff = u16(-1);
	CActionBase<CAI_Stalker>	*action;

	// show
	action				= xr_new<CObjectActionShow>(missile,m_object,&m_storage,"show");
	add_condition		(action,id,eWorldPropertyHidden,	true);
	add_condition		(action,ff,eWorldPropertyItemID,	true);
	add_effect			(action,ff,eWorldPropertyItemID,	false);
	add_effect			(action,id,eWorldPropertyHidden,	false);
	add_operator		(uid(id,eWorldOperatorShow),		action);

	// hide
	action				= xr_new<CObjectActionHide>(missile,m_object,&m_storage,"hide");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,ff,eWorldPropertyItemID,	false);
	add_effect			(action,ff,eWorldPropertyItemID,	true);
	add_effect			(action,id,eWorldPropertyHidden,	true);
	add_operator		(uid(id,eWorldOperatorHide),		action);

	// drop
	action				= xr_new<CObjectActionDrop>(missile,m_object,&m_storage,"drop");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyDropped,	true);
	add_operator		(uid(id,eWorldOperatorDrop),		action);

	// idle
	action				= xr_new<CSObjectActionBase>(missile,m_object,&m_storage,"idle");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyThrowStarted,false);
	add_condition		(action,id,eWorldPropertyThrowIdle,	false);
	add_condition		(action,id,eWorldPropertyFiring1,	false);
	add_effect			(action,id,eWorldPropertyIdle,		true);
	add_operator		(uid(id,eWorldOperatorIdle),		action);

	// fire start
	action				= xr_new<CObjectActionFire>(missile,m_object,&m_storage,0,"throw start");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyThrowStarted,false);
	add_effect			(action,id,eWorldPropertyThrowStarted,true);
	add_operator		(uid(id,eWorldOperatorThrowStart),	action);

	// fire idle
	action				= xr_new<CObjectActionThreaten>(m_object,m_object,&m_storage,"throw idle");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyThrowStarted,true);
	add_condition		(action,id,eWorldPropertyThrowIdle,	false);
	add_effect			(action,id,eWorldPropertyThrowIdle,	true);
	add_operator		(uid(id,eWorldOperatorThrowIdle),	action);

	// fire throw
	action				= xr_new<CSObjectActionBase>(missile,m_object,&m_storage,"throwing");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyThrowIdle,	true);
	add_condition		(action,id,eWorldPropertyThrow,		false);
	add_effect			(action,id,eWorldPropertyThrow,		true);
	add_operator		(uid(id,eWorldOperatorThrow),		action);

	action				= xr_new<CSObjectActionBase>(missile,m_object,&m_storage,"threaten");
	add_condition		(action,id,eWorldPropertyThrow,		true);
	add_condition		(action,id,eWorldPropertyFiring1,	false);
	add_effect			(action,id,eWorldPropertyFiring1,	true);
	add_operator		(uid(id,eWorldOperatorThreaten),action);
	
	this->action(uid(id,eWorldOperatorThrowIdle)).set_inertia_time	(2000);
}

void CObjectHandler::add_evaluators		(CFoodItem *food_item)
{
	u16					id = food_item->ID();
	// dynamic state properties
	add_evaluator		(uid(id,eWorldPropertyHidden)		,xr_new<CObjectPropertyEvaluatorFood>(food_item,m_object,FOOD_HIDDEN));
	add_evaluator		(uid(id,eWorldPropertyPrepared)		,xr_new<CObjectPropertyEvaluatorPrepared>(food_item,m_object));
	add_evaluator		(uid(id,eWorldPropertyUseEnough)	,xr_new<CObjectPropertyEvaluatorMember>(&m_storage,eWorldPropertyUseEnough,true));
	
	// const properties
	add_evaluator		(uid(id,eWorldPropertyDropped)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyIdle)			,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyUsed)			,xr_new<CObjectPropertyEvaluatorConst>(false));
}

void CObjectHandler::add_operators		(CFoodItem *food_item)
{
	u16					id = food_item->ID(), ff = u16(-1);
	CActionBase<CAI_Stalker>	*action;

	// show
	action				= xr_new<CObjectActionShow>(food_item,m_object,&m_storage,"show");
	add_condition		(action,id,eWorldPropertyHidden,	true);
	add_condition		(action,ff,eWorldPropertyItemID,	true);
	add_effect			(action,ff,eWorldPropertyItemID,	false);
	add_effect			(action,id,eWorldPropertyHidden,	false);
	add_operator		(uid(id,eWorldOperatorShow),		action);

	// hide
	action				= xr_new<CObjectActionHide>(food_item,m_object,&m_storage,"hide");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,ff,eWorldPropertyItemID,	false);
	add_effect			(action,ff,eWorldPropertyItemID,	true);
	add_effect			(action,id,eWorldPropertyHidden,	true);
	add_operator		(uid(id,eWorldOperatorHide),		action);

	// drop
	action				= xr_new<CObjectActionDrop>(food_item,m_object,&m_storage,"drop");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyDropped,	true);
	add_operator		(uid(id,eWorldOperatorDrop),		action);

	// idle
	action				= xr_new<CObjectActionIdle>(food_item,m_object,&m_storage,"idle");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyIdle,		true);
	add_effect			(action,id,eWorldPropertyUseEnough,	false);
	add_operator		(uid(id,eWorldOperatorIdle),		action);

	// prepare
	action				= xr_new<CObjectActionPrepare>(food_item,m_object,&m_storage,"prepare");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyPrepared,	false);
	add_effect			(action,id,eWorldPropertyPrepared,	true);
	add_operator		(uid(id,eWorldOperatorPrepare),		action);

	// action
	action				= xr_new<CObjectActionUse>(food_item,m_object,&m_storage,"use");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyPrepared,	true);
	add_condition		(action,id,eWorldPropertyUsed,		false);
	add_condition		(action,id,eWorldPropertyUseEnough,	false);
	add_effect			(action,id,eWorldPropertyUsed,		true);
	add_operator		(uid(id,eWorldOperatorUse),			action);
}

IC	bool CObjectHandler::firing		() const
{
	return					(m_bFiring);
}

IC	CObjectHandler::_condition_type CObjectHandler::uid(const u32 id0, const u32 id1) const
{
	VERIFY				(!((id0 << 16) & id1));
	return				((id0 << 16) | id1);
}

IC	bool CObjectHandler::object_action	(u32 action_id, CObject *object)
{
	return				((action_id >> 16) == object->ID());
}

IC	u32	CObjectHandler::current_action_object_id	() const
{
	return				(action_object_id(current_action_id()));
}

IC	u32	CObjectHandler::current_action_state_id	() const
{
	return				(action_state_id(current_action_id()));
}

IC	u32	CObjectHandler::action_object_id		(_condition_type action_id) const
{
	return				(action_id >> 16);
}

IC	u32	CObjectHandler::action_state_id			(_condition_type action_id) const
{
	return				(action_id & 0xffff);
}

IC	void CObjectHandler::add_condition			(CActionBase<CAI_Stalker> *action, u16 id, ObjectHandlerSpace::EWorldProperties property, _value_type value)
{
	action->add_condition(CWorldProperty(uid(id,property),value));
}

IC	void CObjectHandler::add_effect				(CActionBase<CAI_Stalker> *action, u16 id, ObjectHandlerSpace::EWorldProperties property, _value_type value)
{
	action->add_effect	(CWorldProperty(uid(id,property),value));
}

IC	ObjectHandlerSpace::EWorldProperties CObjectHandler::object_property(MonsterSpace::EObjectAction object_action) const
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

void CObjectHandler::set_goal	(MonsterSpace::EObjectAction object_action, CGameObject *game_object)
{
	EWorldProperties		goal = object_property(object_action);
	u32						condition_id = goal;

	if (game_object && (eWorldPropertyNoItemsIdle != goal))
		condition_id		= uid(game_object->ID(), goal);
	else
		condition_id		= u32(eWorldPropertyNoItemsIdle);

#ifdef LOG_ACTION
	if (m_use_log) {
		Msg					("%6d : Active item %s",Level().timeServer(),inventory().ActiveItem() ? *inventory().ActiveItem()->cName() : "no active items");
		Msg					("%6d : Goal %s",Level().timeServer(),property2string(condition_id));
	}
#endif
	CState					condition;
	condition.add_condition	(CWorldProperty(condition_id,true));
	set_target_state		(condition);
}

void CObjectHandler::update()
{
	inherited::update		();
}
