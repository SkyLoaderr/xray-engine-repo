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
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "torch.h"

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
}

void CObjectHandler::reload			(LPCSTR section)
{
	inherited::reload			(section);
}

BOOL CObjectHandler::net_Spawn		(LPVOID DC)
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

	if (planner().object()->g_Alive()) {
		CTorch					*torch = smart_cast<CTorch*>(inventory_item);
		if (torch)
			torch->Switch		(true);
	}
}

void CObjectHandler::OnItemDrop		(CInventoryItem *inventory_item)
{
	inherited::OnItemDrop	(inventory_item);
	if (m_infinite_ammo && planner().object()->g_Alive()) {
		CWeaponAmmo				*weapon_ammo = smart_cast<CWeaponAmmo*>(inventory_item);
		if (weapon_ammo)
			Level().spawn_item	(*weapon_ammo->cNameSect(),planner().object()->Position(),planner().object()->level_vertex_id(),planner().object()->ID());
	}
	planner().remove_item		(inventory_item);

	CTorch						*torch = smart_cast<CTorch*>(inventory_item);
	if (torch)
		torch->Switch			(false);
}

void CObjectHandler::OnItemDropUpdate	()
{
	inherited::OnItemDropUpdate	();
}

CInventoryItem *CObjectHandler::best_weapon() const
{
	CInventoryItem	*best_weapon = 0;

	if (!planner().object()->g_Alive())
		return		(0);

	ai().ef_storage().m_tpCurrentMember			= planner().object();
	ai().ef_storage().m_tpCurrentALifeMember	= 0;
	ai().ef_storage().m_tpCurrentEnemy			= planner().object()->enemy() ? planner().object()->enemy() : planner().object();

	float						best_value = 0;
	ai().ef_storage().m_tpCurrentMember = planner().object();
	TIItemSet::const_iterator	I = inventory().m_all.begin();
	TIItemSet::const_iterator	E = inventory().m_all.end();
	for ( ; I != E; ++I) {
		if ((*I)->getDestroy())
			continue;
		if ((*I)->can_kill()) {
			ai().ef_storage().m_tpGameObject	= *I;
			float value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
			if (value > best_value) {
				best_value		= value;
				best_weapon		= *I;
			}
		}
	}
	return			(best_weapon);
}

void CObjectHandler::update		()
{
	planner().update		();
}

void CObjectHandler::set_goal	(MonsterSpace::EObjectAction object_action, CGameObject *game_object, u32 queue_size, u32 queue_interval)
{
	planner().set_goal(object_action,game_object,queue_size,queue_interval);
}

bool CObjectHandler::goal_reached	()
{
	return					(planner().solution().size() < 2);
}
