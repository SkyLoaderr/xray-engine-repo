////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler.cpp
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_handler.h"
#include "object_handler_space.h"
#include "object_handler_planner.h"
#include "ai_monster_space.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "weaponmagazined.h"
#include "ef_storage.h"
#include "ef_pattern.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "torch.h"
#include "../skeletoncustom.h"
#include "memory_manager.h"
#include "enemy_manager.h"
#include "ai_object_location.h"

CObjectHandler::CObjectHandler		()
{
	m_planner					= xr_new<CObjectHandlerPlanner>();
}

CObjectHandler::~CObjectHandler		()
{
	xr_delete					(m_planner);
}

void CObjectHandler::Load			(LPCSTR section)
{
	inherited::Load				(section);
}

void CObjectHandler::reinit			(CAI_Stalker *object)
{
	inherited::reinit			();
	m_hammer_is_clutched		= false;
	planner().setup				(object);
	CKinematics					*kinematics = smart_cast<CKinematics*>(planner().m_object->Visual());
	m_r_hand					= kinematics->LL_BoneID(pSettings->r_string(*planner().m_object->cNameSect(),"weapon_bone0"));
	m_l_finger1					= kinematics->LL_BoneID(pSettings->r_string(*planner().m_object->cNameSect(),"weapon_bone1"));
	m_r_finger2					= kinematics->LL_BoneID(pSettings->r_string(*planner().m_object->cNameSect(),"weapon_bone2"));
}

void CObjectHandler::reload			(LPCSTR section)
{
	inherited::reload			(section);
}

BOOL CObjectHandler::net_Spawn		(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC))
		return					(FALSE);

	CSE_Abstract				*abstract = static_cast<CSE_Abstract*>(DC);
	CSE_ALifeTraderAbstract		*trader = smart_cast<CSE_ALifeTraderAbstract*>(abstract);
	VERIFY						(trader);

	m_infinite_ammo				= !!trader->m_trader_flags.test(CSE_ALifeTraderAbstract::eTraderFlagInfiniteAmmo);
	return						(TRUE);
}

void CObjectHandler::OnItemTake		(CInventoryItem *inventory_item)
{
	inherited::OnItemTake		(inventory_item);
	planner().add_item			(inventory_item);

	if (planner().object().g_Alive())
		switch_torch			(inventory_item,true);
}

void CObjectHandler::OnItemDrop		(CInventoryItem *inventory_item)
{
	inherited::OnItemDrop	(inventory_item);
	if (m_infinite_ammo && planner().object().g_Alive()) {
		CWeaponAmmo				*weapon_ammo = smart_cast<CWeaponAmmo*>(inventory_item);
		if (weapon_ammo)
			Level().spawn_item	(*weapon_ammo->cNameSect(),planner().object().Position(),planner().object().ai_location().level_vertex_id(),planner().object().ID());
	}
	planner().remove_item		(inventory_item);

	switch_torch				(inventory_item,false);
}

void CObjectHandler::OnItemDropUpdate	()
{
	inherited::OnItemDropUpdate	();
}

CInventoryItem *CObjectHandler::best_weapon() const
{
	CInventoryItem	*best_weapon = 0;

	if (!planner().object().g_Alive())
		return		(0);

	ai().ef_storage().alife().clear		();
	ai().ef_storage().non_alife().clear	();
	ai().ef_storage().non_alife().member()	= &planner().object();
	ai().ef_storage().non_alife().enemy()	= planner().object().memory().enemy().selected() ? planner().object().memory().enemy().selected() : &planner().object();

	float										best_value = 0;
	TIItemSet::const_iterator	I = inventory().m_all.begin();
	TIItemSet::const_iterator	E = inventory().m_all.end();
	for ( ; I != E; ++I) {
		if ((*I)->object().getDestroy())
			continue;
		if ((*I)->can_kill()) {
			ai().ef_storage().non_alife().member_item()	= &(*I)->object();
			float value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
			if (value > best_value) {
				best_value		= value;
				best_weapon		= *I;
			}
		}
	}
	return						(best_weapon);
}

void CObjectHandler::update		()
{
	planner().update		();
}

void CObjectHandler::set_goal	(MonsterSpace::EObjectAction object_action, CGameObject *game_object, u32 queue_size, u32 queue_interval)
{
	planner().set_goal(object_action,game_object,queue_size,queue_interval);
}

void CObjectHandler::set_goal	(MonsterSpace::EObjectAction object_action, CInventoryItem *inventory_item, u32 queue_size, u32 queue_interval)
{
	set_goal(object_action,inventory_item ? &inventory_item->object() : 0,queue_size,queue_interval);
}

bool CObjectHandler::goal_reached	()
{
	return					(planner().solution().size() < 2);
}

void CObjectHandler::weapon_bones	(int &b0, int &b1, int &b2) const
{
	CWeapon						*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
	if (!weapon || !planner().m_storage.property(ObjectHandlerSpace::eWorldPropertyStrapped)) {
		if (weapon)
			weapon->strapped_mode	(false);
		b0						= m_r_hand;
		b1						= m_r_finger2;
		b2						= m_l_finger1;
		return;
	}

	THROW3						(weapon->can_be_strapped(),"Cannot strap weapon %s",*weapon->cName());
	weapon->strapped_mode		(true);
	CKinematics					*kinematics = smart_cast<CKinematics*>(planner().m_object->Visual());
	b0							= kinematics->LL_BoneID(weapon->strap_bone0());
	b1							= kinematics->LL_BoneID(weapon->strap_bone1());
	b2							= b1;
}

bool CObjectHandler::weapon_strapped	() const
{
	CWeapon						*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
	if (!weapon)
		return					(false);

	return						(weapon_strapped(weapon));
}

bool CObjectHandler::weapon_strapped	(CWeapon *weapon) const
{
	VERIFY						(weapon);

	if (!weapon->can_be_strapped())
		return					(false);

	if (
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorStrapping2Idle) ||
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorStrapping) ||
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorUnstrapping2Idle) ||
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorUnstrapping)
	)
		return					(false);

	return						(weapon->strapped_mode());
}

bool CObjectHandler::weapon_unstrapped	() const
{
	CWeapon						*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
	if (!weapon)
		return					(true);

	return						(weapon_unstrapped(weapon));
}

bool CObjectHandler::weapon_unstrapped	(CWeapon *weapon) const
{
	VERIFY						(weapon);

	if (!weapon->can_be_strapped())
		return					(true);

	if (
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorStrapping2Idle) ||
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorStrapping) ||
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorUnstrapping2Idle) ||
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorUnstrapping)
		)
		return					(false);

	return						(!weapon->strapped_mode());
}

IC	void CObjectHandler::switch_torch	(CInventoryItem *inventory_item, bool value)
{
	CTorch						*torch = smart_cast<CTorch*>(inventory_item);
	if (torch && attached(torch))
		torch->Switch			(value);
}

void CObjectHandler::attach				(CInventoryItem *inventory_item)
{
	inherited::attach			(inventory_item);
	switch_torch				(inventory_item,true);
}

void CObjectHandler::detach				(CInventoryItem *inventory_item)
{
	switch_torch				(inventory_item,false);
	inherited::detach			(inventory_item);
}
