////////////////////////////////////////////////////////////////////////////
//	Module 		: item_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Item manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const xr_set<const CGameObject*> &CItemManager::items() const
{
	return					(objects());
}
