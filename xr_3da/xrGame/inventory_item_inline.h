////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item_inline.h
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	u64	CInventoryItem::InventoryMask		() const
{
	return				(m_inventory_mask);
}

IC	int	CInventoryItem::GetVolume			() const
{
	return				(m_iGridWidth*m_iGridHeight);
}

IC	int	CInventoryItem::GetHeight			() const
{
	return				(m_iGridHeight);
}

IC	int	CInventoryItem::GetWidth			() const
{
	return				(m_iGridWidth);
}