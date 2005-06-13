////////////////////////////////////////////////////////////////////////////
//	Module 		: attachable_item.cpp
//	Created 	: 11.02.2004
//  Modified 	: 11.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Attachable item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "attachable_item.h"
#include "inventoryowner.h"
#include "inventory.h"

#ifdef DEBUG
	CAttachableItem*	CAttachableItem::m_dbgItem = NULL;
#endif

CAttachableItem::~CAttachableItem	()
{
}

void CAttachableItem::reinit		()
{
	inherited::reinit	();
}

void CAttachableItem::Load			(LPCSTR section)
{
	inherited::Load		(section);
}

void CAttachableItem::reload		(LPCSTR section)
{
	inherited::reload	(section);
	Fvector				angle_offset = pSettings->r_fvector3	(section,"attach_angle_offset");
	Fvector				position_offset	= pSettings->r_fvector3	(section,"attach_position_offset");
	m_offset.setHPB		(VPUSH(angle_offset));
	m_offset.c			= position_offset;
	m_bone_name			= pSettings->r_string	(section,"attach_bone_name");
	enable				(m_auto_attach = !!(READ_IF_EXISTS(pSettings,r_bool,section,"auto_attach",TRUE)));
}

void CAttachableItem::OnH_A_Chield	() 
{
	inherited::OnH_A_Chield			();
	const CInventoryOwner			*inventory_owner = smart_cast<const CInventoryOwner*>(H_Parent());
	VERIFY							(inventory_owner);
	if (inventory_owner->attached(this))
		setVisible					(true);
}

void CAttachableItem::renderable_Render()
{
	::Render->set_Transform			(&XFORM());
	::Render->add_Visual			(Visual());
}
void CAttachableItem::OnH_A_Independent	()
{
	inherited::OnH_A_Independent	();
	enable							(m_auto_attach);
}

void CAttachableItem::enable		(bool value)
{
	if (!H_Parent()) {
		m_enabled			= value;
		return;
	}

	if (value && !enabled() && H_Parent()) {
		CGameObject			*game_object = smart_cast<CGameObject*>(H_Parent());
		CAttachmentOwner	*owner = smart_cast<CAttachmentOwner*>(game_object);
		VERIFY				(owner);
		m_enabled			= value;
		owner->attach		(this);
		setVisible			(true);
	}
	
	if (!value && enabled() && H_Parent()) {
		CGameObject			*game_object = smart_cast<CGameObject*>(H_Parent());
		CAttachmentOwner	*owner = smart_cast<CAttachmentOwner*>(game_object);
		VERIFY				(owner);
		m_enabled			= value;
		owner->detach		(this);
		setVisible			(false);
	}
}

bool  CAttachableItem::can_be_attached		() const
{
	if (!m_pInventory)
		return				(false);

	if (!m_pInventory->IsBeltUseful())
		return				(true);

	if (m_eItemPlace != eItemPlaceBelt)
		return				(false);
	 
	return					(true);
}
void CAttachableItem::afterAttach			()
{
}

void CAttachableItem::afterDetach			()
{
}
