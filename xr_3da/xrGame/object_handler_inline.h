////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CObjectHandler::firing		() const
{
	return					(m_bFiring);
}

IC	u32 CObjectHandler::uid(const u32 id0, const u32 id1) const
{
	VERIFY				(!((id0 << 16) & id1));
	return				((id0 << 16) | id1);
}

IC	void CObjectHandler::set_dest_state	(const MonsterSpace::EObjectAction object_action, CGameObject *game_object)
{
	if (game_object)
		inherited::set_dest_state		(uid(object_action, game_object->ID()));
	else
		if (inventory().ActiveItem())
			inherited::set_dest_state	(uid(object_action, inventory().ActiveItem()->ID()));
		else
			inherited::set_dest_state	(u32(MonsterSpace::eObjectActionNoItems));
}

IC	bool CObjectHandler::object_state	(u32 state_id, CInventoryItem *inventory_item)
{
	return				((state_id & 0xffff) == inventory_item->ID());
}

IC	u32	CObjectHandler::current_object_state_id	() const
{
	return				(current_state_id() & 0xffff);
}

IC	u32	CObjectHandler::current_state_state_id	() const
{
	return				(current_state_id() >> 16);
}
