////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast.cpp
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smart_cast.h"


#ifndef PURE_DYNAMIC_CAST
#	include "gameobject.h"
#	include "actor.h"

template <> 
CGameObject* SmartDynamicCast::smart_cast<CGameObject,CObject>(CObject *p)
{
	return static_cast<CGameObject*>(p);
}


template <> 
CEntity* SmartDynamicCast::smart_cast<CEntity,CGameObject>(CGameObject *p)
{
	return p->cast_entity();
}

template <> 
CEntityAlive* SmartDynamicCast::smart_cast<CEntityAlive,CGameObject>(CGameObject *p)
{
	return p->cast_entity_alive();
}

template <> 
CInventoryItem* SmartDynamicCast::smart_cast<CInventoryItem,CGameObject>(CGameObject *p)
{
	return p->cast_inventory_item();
}

template <> 
CInventoryOwner* SmartDynamicCast::smart_cast<CInventoryOwner,CGameObject>(CGameObject *p)
{
	return p->cast_inventory_owner();
}

template <> 
CActor* SmartDynamicCast::smart_cast<CActor,CGameObject>(CGameObject *p)
{
	return p->cast_actor();
}

template <> 
CGameObject* SmartDynamicCast::smart_cast<CGameObject, CInventoryOwner>(CInventoryOwner *p)
{
	return p->cast_game_object();
}

#endif