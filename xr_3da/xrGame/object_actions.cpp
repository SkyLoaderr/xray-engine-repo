////////////////////////////////////////////////////////////////////////////
//	Module 		: object_actions.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object actions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_actions.h"
#include "inventory.h"
#include "ai/stalker/ai_stalker.h"
#include "xr_level_controller.h"
#include "xrmessages.h"
#include "fooditem.h"
#include "weapon.h"

//////////////////////////////////////////////////////////////////////////
// CObjectActionCommand
//////////////////////////////////////////////////////////////////////////

CObjectActionCommand::CObjectActionCommand(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 command, LPCSTR action_name) :
	inherited			(item,owner,storage,action_name),
	m_command			(command)
{
}

void CObjectActionCommand::initialize	()
{
	inherited::initialize();
	m_object->inventory().Action(m_command,CMD_START);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionShow
//////////////////////////////////////////////////////////////////////////

CObjectActionShow::CObjectActionShow	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
}

void CObjectActionShow::initialize		()
{
	inherited::initialize			();
	VERIFY							(m_item);
	if (m_object->inventory().m_slots[m_item->GetSlot()].m_pIItem)
		m_object->inventory().Ruck	(m_object->inventory().m_slots[m_item->GetSlot()].m_pIItem);
	m_object->inventory().SetActiveSlot(NO_ACTIVE_SLOT);
	m_object->inventory().Slot		(m_item);
	bool							result = m_object->inventory().Activate	(m_item->GetSlot());
	VERIFY							(result);
}

void CObjectActionShow::execute		()
{
	inherited::execute				();
	VERIFY							(m_item);
	if (!m_object->inventory().ActiveItem() || (m_object->inventory().ActiveItem()->ID() != m_item->ID())) {
		CHudItem					*hud_item = smart_cast<CHudItem*>(m_object->inventory().ActiveItem());
		if (!hud_item)
			return;
		if (!hud_item->IsPending()) {
			if (m_object->inventory().m_slots[m_item->GetSlot()].m_pIItem)
				m_object->inventory().Ruck	(m_object->inventory().m_slots[m_item->GetSlot()].m_pIItem);
			m_object->inventory().SetActiveSlot(NO_ACTIVE_SLOT);
			m_object->inventory().Slot		(m_item);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionHide
//////////////////////////////////////////////////////////////////////////

CObjectActionHide::CObjectActionHide	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
}

void CObjectActionHide::execute		()
{
	inherited::execute				();
	VERIFY							(m_item);
	m_object->inventory().Activate	(NO_ACTIVE_SLOT);
	set_property					(ObjectHandlerSpace::eWorldPropertyUseEnough,false);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionReload
//////////////////////////////////////////////////////////////////////////

CObjectActionReload::CObjectActionReload	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 type, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name),
	m_type			(type)
{
}

void CObjectActionReload::initialize		()
{
	inherited::initialize		();
	m_object->inventory().Action(kWPN_RELOAD,	CMD_START);
}

void CObjectActionReload::execute			()
{
	inherited::execute();
	CWeapon			*weapon = smart_cast<CWeapon*>(m_object->inventory().ActiveItem());
	VERIFY			(weapon);
	if (!weapon->IsPending() && !weapon->GetAmmoElapsed())
		m_object->inventory().Action(kWPN_RELOAD,	CMD_START);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionFire
//////////////////////////////////////////////////////////////////////////

CObjectActionFire::CObjectActionFire	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 type, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name),
	m_type			(type)
{
}

void CObjectActionFire::initialize		()
{
	inherited::inherited::initialize	();
	if (!m_object->can_kill_member())
		m_object->inventory().Action(kWPN_FIRE,	CMD_START);
	else
		m_object->inventory().Action(kWPN_FIRE,	CMD_STOP);
}

void CObjectActionFire::execute			()
{
	inherited::execute			();
	if (!m_object->can_kill_member())
		m_object->inventory().Action(kWPN_FIRE,	CMD_START);
	else
		m_object->inventory().Action(kWPN_FIRE,	CMD_STOP);
}

void CObjectActionFire::finalize		()
{
	inherited::finalize();
	m_object->inventory().Action(kWPN_FIRE,	CMD_STOP);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionStrapping
//////////////////////////////////////////////////////////////////////////

CObjectActionStrapping::CObjectActionStrapping	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
}

void CObjectActionStrapping::initialize		()
{
	inherited::initialize();
}

void CObjectActionStrapping::execute			()
{
	inherited::execute();
}

void CObjectActionStrapping::finalize		()
{
	inherited::finalize();
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionUnstrapping
//////////////////////////////////////////////////////////////////////////

CObjectActionUnstrapping::CObjectActionUnstrapping	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
}

void CObjectActionUnstrapping::initialize		()
{
	inherited::initialize();
}

void CObjectActionUnstrapping::execute			()
{
	inherited::execute();
}

void CObjectActionUnstrapping::finalize		()
{
	inherited::finalize();
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionQueueWait
//////////////////////////////////////////////////////////////////////////

CObjectActionQueueWait::CObjectActionQueueWait	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 type, LPCSTR action_name) :
	inherited				(item,owner,storage,action_name),
	m_type					(type)
{
}

void CObjectActionQueueWait::initialize		()
{
	inherited::inherited::initialize	();
//	m_object->set_aimed		(m_type ? 0 : 1,FALSE);
}

void CObjectActionQueueWait::execute			()
{
	inherited::execute		();
}

void CObjectActionQueueWait::finalize		()
{
	inherited::finalize		();
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionSwitch
//////////////////////////////////////////////////////////////////////////

CObjectActionSwitch::CObjectActionSwitch	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 type, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name),
	m_type			(type)
{
}

void CObjectActionSwitch::initialize		()
{
	inherited::initialize	();
}

void CObjectActionSwitch::execute			()
{
	inherited::execute		();
}

void CObjectActionSwitch::finalize		()
{
	inherited::finalize		();
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionDrop
//////////////////////////////////////////////////////////////////////////

CObjectActionDrop::CObjectActionDrop	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
}

void CObjectActionDrop::initialize		()
{
	inherited::initialize	();
	if (!m_item || !m_item->H_Parent() || (m_object->ID() != m_item->H_Parent()->ID()))
		return;

	NET_Packet				P;
	m_object->u_EventGen	(P,GE_OWNERSHIP_REJECT,m_object->ID());
	P.w_u16					(u16(m_item->ID()));
	m_object->u_EventSend	(P);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionThreaten
//////////////////////////////////////////////////////////////////////////

CObjectActionThreaten::CObjectActionThreaten	(CAI_Stalker *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited				(item,owner,storage,action_name)
{
}

void CObjectActionThreaten::execute			()
{
	inherited::execute		();
	if (completed())
		m_object->inventory().Action(kWPN_FIRE,	CMD_STOP);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionAim
//////////////////////////////////////////////////////////////////////////

CObjectActionAim::CObjectActionAim			(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, _condition_type condition_id, _value_type value, LPCSTR action_name) :
	inherited							(item,owner,storage,condition_id,value,action_name)
{
}

void CObjectActionAim::initialize			()
{
	inherited::inherited::initialize	();
}

void CObjectActionAim::execute				()
{
	inherited::execute					();
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionPrepare
//////////////////////////////////////////////////////////////////////////

CObjectActionPrepare::CObjectActionPrepare(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited			(item,owner,storage,action_name)
{
}

void CObjectActionPrepare::execute	()
{
	inherited::execute();
	m_object->inventory().Action(kWPN_FIRE,CMD_START);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionUse
//////////////////////////////////////////////////////////////////////////

CObjectActionUse::CObjectActionUse(CFoodItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited			(item,owner,storage,action_name)
{
}

void CObjectActionUse::execute	()
{
	inherited::execute();
	m_object->inventory().Action(kWPN_FIRE,CMD_START);
	if (m_item->State() != FOOD_EATING)
		m_storage->set_property(ObjectHandlerSpace::eWorldPropertyUseEnough,true);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionIdle
//////////////////////////////////////////////////////////////////////////

CObjectActionIdle::CObjectActionIdle(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited			(item,owner,storage,action_name)
{
}

void CObjectActionIdle::initialize	()
{
	inherited::initialize();
	if (m_storage->property(ObjectHandlerSpace::eWorldPropertyUseEnough))
		m_object->CObjectHandler::set_goal(MonsterSpace::eObjectActionActivate,m_object->inventory().ActiveItem());
	m_storage->set_property	(ObjectHandlerSpace::eWorldPropertyUseEnough,false);
}
