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
}

IC	LPCSTR CAttachableItem::bone_name				() const
{
	return				(m_bone_name);
}

IC	const Fmatrix &CAttachableItem::offset			() const
{
	return				(m_offset);
}