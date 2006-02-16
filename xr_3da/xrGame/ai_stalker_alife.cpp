////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife.cpp
//	Created 	: 15.10.2004
//  Modified 	: 15.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai/stalker/ai_stalker.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_task_registry.h"
#include "alife_task.h"
#include "alife_space.h"
#include "inventory.h"
#include "clsid_game.h"
#include "pda.h"
#include "eatable_item.h"
#include "medkit.h"
#include "weapon.h"
#include "customdetector.h"
#include "ef_storage.h"
#include "ef_primary.h"
#include "ef_pattern.h"
#include "trade_parameters.h"

#define MAX_ITEM_FOOD_COUNT		3
#define MAX_ITEM_MEDIKIT_COUNT	3
#define MAX_AMMO_ATTACH_COUNT	10

IC	bool CAI_Stalker::CTradeItem::operator<	(const CTradeItem &trade_item) const
{
	return			(m_item->object().ID() < trade_item.m_item->object().ID());
}

IC	bool CAI_Stalker::CTradeItem::operator==(u16 id) const
{
	return			(m_item->object().ID() == id);
}

bool CAI_Stalker::task_completed			(const CALifeTask *_task)
{
	const CALifeTask	&task = *_task;

	switch (task.m_tTaskType) {
		case ALife::eTaskTypeSearchForItemCL :
		case ALife::eTaskTypeSearchForItemCG : {
			return		(!!inventory().dwfGetSameItemCount(task.m_caSection));
		}
		case ALife::eTaskTypeSearchForItemOL :
		case ALife::eTaskTypeSearchForItemOG : {
			return		(inventory().bfCheckForObject(task.m_tObjectID));
		}
	}
	return				(false);
}

bool CAI_Stalker::similar_task				(const CALifeTask *prev_task, const CALifeTask *new_task)
{
	if (!prev_task)
		return			(false);

	if (prev_task->m_tTaskType != new_task->m_tTaskType)
		return			(false);

	switch (prev_task->m_tTaskType) {
		case ALife::eTaskTypeSearchForItemOG :
		case ALife::eTaskTypeSearchForItemCG : {
			return		(prev_task->m_tGraphID == new_task->m_tGraphID);
		}
		case ALife::eTaskTypeSearchForItemOL :
		case ALife::eTaskTypeSearchForItemCL : {
			return		(prev_task->m_tLocationID == new_task->m_tLocationID);
		}
	}
	return				(false);
}

void CAI_Stalker::select_alife_task			()
{
	CALifeTask						*current_task = m_current_alife_task;
	ALife::OBJECT_IT				I = m_tpKnownCustomers.begin();
	ALife::OBJECT_IT				E = m_tpKnownCustomers.end();
	for ( ; I != E; ++I) {
		ALife::OBJECT_TASK_MAP::const_iterator	J = ai().alife().tasks().cross().find(*I);
		R_ASSERT2					(ai().alife().tasks().cross().end() != J,"Can't find a specified customer in the Task registry!\nPossibly, there is no traders at all or there is no anomalous zones.");

		u32							l_dwMinTryCount = u32(-1);
		ALife::_TASK_ID				l_tBestTaskID = ALife::_TASK_ID(-1);
		ALife::TASK_SET::const_iterator	i = (*J).second.begin();
		ALife::TASK_SET::const_iterator	e = (*J).second.end();
		for ( ; i != e; ++i) {
			CALifeTask				*l_tpTask = ai().alife().tasks().task(*i);
			if (similar_task(current_task,l_tpTask) && !task_completed(l_tpTask))
				continue;

			if (!l_tpTask->m_dwTryCount) {
				l_tBestTaskID		= l_tpTask->m_tTaskID;
				break;
			}
			else
				if (l_tpTask->m_dwTryCount < l_dwMinTryCount) {
					l_dwMinTryCount = l_tpTask->m_dwTryCount;
					l_tBestTaskID	= l_tpTask->m_tTaskID;
				}
		}

		if (ALife::_TASK_ID(-1) != l_tBestTaskID)
			m_current_alife_task	= ai().alife().tasks().task(l_tBestTaskID);
	}
}

CALifeTask &CAI_Stalker::current_alife_task	()
{
	if (!m_current_alife_task)
		select_alife_task	();

	VERIFY3					(m_current_alife_task,"Not enough ALife tasks",*cName());
	return					(*m_current_alife_task);
}

void CAI_Stalker::failed_to_complete_alife_task	()
{
	++(current_alife_task().m_dwTryCount);
	select_alife_task();
}

bool CAI_Stalker::alife_task_completed			()
{
	return				(task_completed(&current_alife_task()));
}

bool CAI_Stalker::tradable_item					(CInventoryItem *inventory_item, const u16 &current_owner_id)
{
	if (!inventory_item->useful_for_NPC())
		return			(false);

	if (CLSID_DEVICE_PDA == inventory_item->object().CLS_ID) {
		CPda			*pda = smart_cast<CPda*>(inventory_item);
		VERIFY			(pda);
		if (pda->GetOriginalOwnerID() == current_owner_id)
			return		(false);
	}

	return				(
		trade_parameters().enabled(
			CTradeParameters::action_sell(0),
			inventory_item->object().cNameSect()
		)
	);
}

u32 CAI_Stalker::fill_items						(CInventory &inventory, CGameObject *old_owner, ALife::_OBJECT_ID new_owner_id)
{
	u32							result = 0;
	TIItemContainer::iterator	I = inventory.m_all.begin();
	TIItemContainer::iterator	E = inventory.m_all.end();
	for ( ; I != E; ++I) {
		if (!tradable_item(*I,old_owner->ID()))
			continue;
		
		m_temp_items.push_back	(CTradeItem(*I,old_owner->ID(),new_owner_id));
		result					+= (*I)->Cost();
	}

	return						(result);
}

void CAI_Stalker::collect_items						()
{
	m_temp_items.clear	();
	m_total_money		= m_dwMoney;
	u32					money_delta = fill_items(inventory(),this,m_trader_game_object->ID());
	m_total_money		+= money_delta;
	m_current_trader->m_dwMoney -= money_delta;
	fill_items			(m_current_trader->inventory(),m_trader_game_object,m_trader_game_object->ID());
	std::sort			(m_temp_items.begin(),m_temp_items.end());
}

void CAI_Stalker::transfer_item						(CInventoryItem *item, CGameObject *old_owner, CGameObject *new_owner)
{
	NET_Packet			P;
	CGameObject			*O = old_owner;
	O->u_EventGen		(P,GE_TRADE_SELL,O->ID());
	P.w_u16				(u16(item->object().ID()));
	O->u_EventSend		(P);

	O					= new_owner;
	O->u_EventGen		(P,GE_TRADE_BUY,O->ID());
	P.w_u16				(u16(item->object().ID()));
	O->u_EventSend		(P);
}

void CAI_Stalker::process_items						()
{
	xr_vector<CTradeItem>::iterator	I = m_temp_items.begin();
	xr_vector<CTradeItem>::iterator	E = m_temp_items.end();
	for ( ; I != E; ++I) {
		if ((*I).m_owner_id != (*I).m_new_owner_id)
			transfer_item	((*I).m_item,(*I).m_owner_id == ID() ? this : m_trader_game_object,(*I).m_owner_id == ID() ? m_trader_game_object : this);
	}

	m_dwMoney				= m_total_money;
	m_temp_items.clear		();
}

IC	void CAI_Stalker::buy_item_virtual				(CTradeItem &item)
{
	item.m_new_owner_id			= ID();
	m_total_money				-= item.m_item->Cost();
	if (m_current_trader)
		m_current_trader->m_dwMoney += item.m_item->Cost();
}

void CAI_Stalker::choose_food						()
{
	u32								count = 0;
	xr_vector<CTradeItem>::iterator	I = m_temp_items.begin();
	xr_vector<CTradeItem>::iterator	E = m_temp_items.end();
	for ( ; I != E; ++I) {
		if (m_total_money < (*I).m_item->Cost())
			continue;

		CEatableItem				*eatable_item = smart_cast<CEatableItem*>((*I).m_item);
		if (!eatable_item)
			continue;

		CMedkit						*medikit = smart_cast<CMedkit*>((*I).m_item);
		if (medikit)
			continue;

		buy_item_virtual			(*I);
		
		++count;
		if (count >= MAX_ITEM_FOOD_COUNT)
			break;
	}
}

void CAI_Stalker::attach_available_ammo	(CWeapon *weapon)
{
	if (!weapon || weapon->m_ammoTypes.empty())
		return;

	u32							count = 0;
	xr_vector<CTradeItem>::iterator	I = m_temp_items.begin();
	xr_vector<CTradeItem>::iterator	E = m_temp_items.end();
	for ( ; I != E; ++I) {
		if (m_total_money < (*I).m_item->Cost())
			continue;

		if (
			std::find(
				weapon->m_ammoTypes.begin(),
				weapon->m_ammoTypes.end(),
				(*I).m_item->object().cNameSect()
			) == 
			weapon->m_ammoTypes.end()
		)
			continue;

		buy_item_virtual			(*I);

		++count;
		if (count >= MAX_AMMO_ATTACH_COUNT)
			break;
	}
}

void CAI_Stalker::choose_weapon						(ALife::EWeaponPriorityType weapon_priority_type)
{
	CTradeItem						*best_weapon	= 0;
	float							best_value		= -1.f;
	ai().ef_storage().non_alife().member()	= this;

	xr_vector<CTradeItem>::iterator	I = m_temp_items.begin();
	xr_vector<CTradeItem>::iterator	E = m_temp_items.end();
	for ( ; I != E; ++I) {
		if (m_total_money < (*I).m_item->Cost())
			continue;

		ai().ef_storage().non_alife().member_item() = &(*I).m_item->object();
		int						j = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
		float					current_value = -1.f;
		switch (weapon_priority_type) {
			case ALife::eWeaponPriorityTypeKnife : {
				if (1 != j)
					continue;
				current_value		= ai().ef_storage().m_pfItemValue->ffGetValue();
				break;
			}
			case ALife::eWeaponPriorityTypeSecondary : {
				if (5 != j)
					continue;
				current_value		= ai().ef_storage().m_pfSmallWeaponValue->ffGetValue();
				break;
			}
			case ALife::eWeaponPriorityTypePrimary : {
				if ((6 != j) && (8 != j) && (9 != j))
					continue;
				current_value		= ai().ef_storage().m_pfMainWeaponValue->ffGetValue();
				break;
			}
			case ALife::eWeaponPriorityTypeGrenade : {
				if (7 != j)
					continue;
				current_value		= ai().ef_storage().m_pfItemValue->ffGetValue();
				break;
			}
			default : NODEFAULT;
		}
		
		if ((current_value > best_value)) {
			best_value	= current_value;
			best_weapon = &*I;
		}
	}
	if (best_weapon) {
		buy_item_virtual			(*best_weapon);
		attach_available_ammo		(smart_cast<CWeapon*>(best_weapon->m_item));
	}
}

void CAI_Stalker::choose_medikit					()
{
	u32								count = 0;
	xr_vector<CTradeItem>::iterator	I = m_temp_items.begin();
	xr_vector<CTradeItem>::iterator	E = m_temp_items.end();
	for ( ; I != E; ++I) {
		if (m_total_money < (*I).m_item->Cost())
			continue;

		CMedkit						*medikit = smart_cast<CMedkit*>((*I).m_item);
		if (!medikit)
			continue;

		(*I).m_new_owner_id			= ID();

		++count;
		if (count >= MAX_ITEM_MEDIKIT_COUNT)
			break;
	}
}

void CAI_Stalker::choose_detector					()
{
	CTradeItem					*best_detector	= 0;
	float						best_value		= -1.f;
	ai().ef_storage().non_alife().member()	= this;
	xr_vector<CTradeItem>::iterator	I = m_temp_items.begin();
	xr_vector<CTradeItem>::iterator	E = m_temp_items.end();
	for ( ; I != E; ++I) {
		if (m_total_money < (*I).m_item->Cost())
			continue;

		CCustomDetector			*detector = smart_cast<CCustomDetector*>((*I).m_item);
		if (!detector)
			continue;

		// evaluating item
		ai().ef_storage().non_alife().member_item() = detector;
		float					current_value = ai().ef_storage().m_pfDetectorType->ffGetValue();
		// choosing the best item
		if ((current_value > best_value)) {
			best_detector		= &*I;
			best_value			= current_value;
		}
	}
	if (best_detector)
		buy_item_virtual		(*best_detector);
}

void CAI_Stalker::choose_equipment					()
{
	// stalker cannot change their equipment due to the game design :-(((
	return;
}

void CAI_Stalker::select_items						()
{
	choose_food			();
	choose_weapon		(ALife::eWeaponPriorityTypeKnife);
	choose_weapon		(ALife::eWeaponPriorityTypeSecondary);
	choose_weapon		(ALife::eWeaponPriorityTypePrimary);
	choose_weapon		(ALife::eWeaponPriorityTypeGrenade);
	choose_medikit		();
	choose_detector		();
	choose_equipment	();
}

void CAI_Stalker::communicate						(CInventoryOwner *trader)
{
	m_sell_info_actuality	= false;
	VERIFY					(trader);
	m_current_trader		= trader;
	m_trader_game_object	= smart_cast<CGameObject*>(trader);
	VERIFY					(m_trader_game_object);

	collect_items			();
	select_items			();
	process_items			();
}

void CAI_Stalker::update_sell_info					()
{
	if (m_sell_info_actuality)
		return;

	m_sell_info_actuality	= true;

	m_temp_items.clear		();
	m_current_trader		= 0;
	m_total_money			= m_dwMoney;
	u32						money_delta = fill_items(inventory(),this,ALife::_OBJECT_ID(-1));
	m_total_money			+= money_delta;
	std::sort				(m_temp_items.begin(),m_temp_items.end());
	select_items			();

	TIItemContainer::iterator	I = inventory().m_all.begin();
	TIItemContainer::iterator	E = inventory().m_all.end();
	for ( ; I != E; ++I) {
		if (!tradable_item(*I,ID()))
			m_temp_items.push_back	(CTradeItem(*I,ID(),ID()));
	}
}

bool CAI_Stalker::can_sell							(CInventoryItem const * item)
{
	update_sell_info		();
	xr_vector<CTradeItem>::const_iterator	I = std::find(m_temp_items.begin(),m_temp_items.end(),item->object().ID());
	VERIFY					(I != m_temp_items.end());
	return					((*I).m_new_owner_id != ID());
}

bool CAI_Stalker::AllowItemToTrade 					(CInventoryItem const * item, EItemPlace place) const
{
	if (!g_Alive())
		return				(trade_parameters().enabled(CTradeParameters::action_show(0),item->object().cNameSect()));

	return					(const_cast<CAI_Stalker*>(this)->can_sell(item));
}

bool CAI_Stalker::non_conflicted					(const CInventoryItem *item, const CWeapon *new_weapon) const
{
	if (item->object().ID() == new_weapon->ID())
		return				(true);

	const CWeapon			*weapon = smart_cast<const CWeapon*>(item);
	if (!weapon)
		return				(true);

	switch (weapon->ef_weapon_type()) {
		// knives
		case 1 : {
			if (weapon->ef_weapon_type() != new_weapon->ef_weapon_type())
				return		(true);

			break;
		}
		// pistols
		case 5 : {
			if (weapon->ef_weapon_type() != new_weapon->ef_weapon_type())
				return		(true);

			break;
		}
		// automatic weapon
		case 6 :
		// shotguns
		case 7 :
		// sniper rifles
		case 8 : {
			if ((new_weapon->ef_weapon_type() < 6) || (new_weapon->ef_weapon_type() > 8))
				return		(true);

			break;
		}
		case 9 : {
			if (weapon->ef_weapon_type() != new_weapon->ef_weapon_type())
				return		(true);

			break;
		}
		case 10 : {
			if (weapon->ef_weapon_type() != new_weapon->ef_weapon_type())
				return		(true);

			break;
		}
	}

	return					(false);
}

static int enough_ammo_box_count = 1;

bool CAI_Stalker::enough_ammo						(const CWeapon *new_weapon) const
{
	int						ammo_box_count = 0;

	TIItemContainer::const_iterator	I = inventory().m_all.begin();
	TIItemContainer::const_iterator	E = inventory().m_all.end();
	for ( ; I != E; ++I) {
		if (std::find(new_weapon->m_ammoTypes.begin(),new_weapon->m_ammoTypes.end(),(*I)->object().cNameSect()) == new_weapon->m_ammoTypes.end())
			continue;

		++ammo_box_count;
		if (ammo_box_count >= enough_ammo_box_count)
			return			(true);
	}

	return					(false);
}

extern int get_rank									(const shared_str &section);

bool CAI_Stalker::conflicted						(const CInventoryItem *item, const CWeapon *new_weapon, bool new_wepon_enough_ammo, int new_weapon_rank) const
{
	if (non_conflicted(item,new_weapon))
		return				(false);

	const CWeapon			*weapon = smart_cast<const CWeapon*>(item);
	VERIFY					(weapon);

	bool					current_wepon_enough_ammo = enough_ammo(weapon);
	if (current_wepon_enough_ammo && !new_wepon_enough_ammo)
		return				(true);

	if (!current_wepon_enough_ammo && new_wepon_enough_ammo)
		return				(false);

	if (!fsimilar(weapon->GetCondition(),new_weapon->GetCondition(),EPS_L))
		return				(weapon->GetCondition() >= new_weapon->GetCondition());

	if (weapon->ef_weapon_type() != new_weapon->ef_weapon_type())
		return				(weapon->ef_weapon_type() >= new_weapon->ef_weapon_type());

	int						weapon_rank = get_rank(*weapon->cNameSect());

	if (weapon_rank != new_weapon_rank)
		return				(weapon_rank >= new_weapon_rank);

	return					(weapon->GetCondition() >= new_weapon->GetCondition());
}

bool CAI_Stalker::can_take							(CInventoryItem const * item)
{
	const CWeapon				*new_weapon = smart_cast<const CWeapon*>(item);
	if (!new_weapon)
		return					(false);

	bool						new_weapon_enough_ammo = enough_ammo(new_weapon);
	int							new_weapon_rank = get_rank(new_weapon->cNameSect());

	TIItemContainer::iterator	I = inventory().m_all.begin();
	TIItemContainer::iterator	E = inventory().m_all.end();
	for ( ; I != E; ++I)
		if (conflicted(*I,new_weapon,new_weapon_enough_ammo,new_weapon_rank))
			return				(false);

	return						(true);
}

void CAI_Stalker::remove_personal_only_ammo			(const CInventoryItem *item)
{
	const CWeapon			*weapon = smart_cast<const CWeapon*>(item);
	VERIFY					(weapon);

	xr_vector<shared_str>::const_iterator	I = weapon->m_ammoTypes.begin();
	xr_vector<shared_str>::const_iterator	E = weapon->m_ammoTypes.end();
	for ( ; I != E; ++I) {
		bool				found = false;

		TIItemContainer::const_iterator	i = inventory().m_all.begin();
		TIItemContainer::const_iterator	e = inventory().m_all.end();
		for ( ; i != e; ++i) {
			if ((*i)->object().ID() == weapon->ID())
				continue;

			const CWeapon	*temp = smart_cast<const CWeapon*>(*i);
			if (!temp)
				continue;

			if (std::find(temp->m_ammoTypes.begin(),temp->m_ammoTypes.end(),*I) == temp->m_ammoTypes.end())
				continue;

			found			= true;
			break;
		}

		if (found)
			continue;

		for (i = inventory().m_all.begin(); i != e; ++i) {
			if (!xr_strcmp(*I,(*i)->object().cNameSect()))
				continue;

			NET_Packet		packet;
			u_EventGen		(packet,GE_DESTROY,(*i)->object().ID());
			u_EventSend		(packet);
		}
	}
}

void CAI_Stalker::update_conflicted					(CInventoryItem *item, const CWeapon *new_weapon)
{
	if (non_conflicted(item,new_weapon))
		return;

	remove_personal_only_ammo	(item);
	item->Drop					();
}

void CAI_Stalker::on_after_take						(const CGameObject *object)
{
	const CWeapon				*new_weapon = smart_cast<const CWeapon*>(object);
	if (!new_weapon)
		return;

	TIItemContainer::iterator	I = inventory().m_all.begin();
	TIItemContainer::iterator	E = inventory().m_all.end();
	for ( ; I != E; ++I)
		update_conflicted		(*I,new_weapon);
}
