////////////////////////////////////////////////////////////////////////////
//	Module 		: attachable_item_inline.h
//	Created 	: 11.02.2004
//  Modified 	: 11.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Attachable item inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CAttachableItem::CAttachableItem				()
{
	init				();
}

IC	void CAttachableItem::init						()
{
	m_offset.identity	();
	m_bone_name			= "";
	m_enabled			= true;
}

IC	shared_str CAttachableItem::bone_name				() const
{
	return				(m_bone_name);
}

IC	const Fmatrix &CAttachableItem::offset			() const
{
	return				(m_offset);
}

IC	u16	 CAttachableItem::bone_id					() const
{
	return				(m_bone_id);
}

IC	void CAttachableItem::set_bone_id				(u16 bone_id)
{
	m_bone_id			= bone_id;
}

IC	bool CAttachableItem::enabled					() const
{
	return				(m_enabled);
}
