////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast.h
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast
////////////////////////////////////////////////////////////////////////////

#ifndef SMART_CAST_H
#define SMART_CAST_H

//#define PURE_DYNAMIC_CAST
#define PURE_DYNAMIC_CAST_COMPATIBILITY_CHECK

#ifdef PURE_DYNAMIC_CAST
#	define smart_cast dynamic_cast
#else
#	ifndef DECLARE_SPECIALIZATION
#		include "smart_cast_impl1.h"
#	else
#		include "smart_cast_impl2.h"
#	endif
#	ifdef XRGAME_EXPORTS
		DECLARE_SPECIALIZATION	(CEntity,			CGameObject,		cast_entity);
		DECLARE_SPECIALIZATION	(CEntityAlive,		CGameObject,		cast_entity_alive);
		DECLARE_SPECIALIZATION	(CInventoryItem,	CGameObject,		cast_inventory_item);
		DECLARE_SPECIALIZATION	(CInventoryOwner,	CGameObject,		cast_inventory_owner);
		DECLARE_SPECIALIZATION	(CActor,			CGameObject,		cast_actor);
		DECLARE_SPECIALIZATION	(CGameObject,		CInventoryOwner,	cast_game_object);
#	endif
#endif

#endif //SMART_CAST_H