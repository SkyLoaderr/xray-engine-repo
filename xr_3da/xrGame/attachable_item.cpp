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
}

void CAttachableItem::OnH_A_Chield	() 
{
	inherited::OnH_A_Chield			();
	const CInventoryOwner			*inventory_owner = dynamic_cast<const CInventoryOwner*>(H_Parent());
	VERIFY							(inventory_owner);
	if (inventory_owner->attached(this))
		setVisible					(true);
}