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
#		include "smart_cast_impl0.h"
#	else
#		include "smart_cast_impl2.h"
#		define DO_NOT_DECLARE_TYPE_LIST
#	endif

#	ifdef XRGAME_EXPORTS

#		ifndef DO_NOT_DECLARE_TYPE_LIST
			class CObject;
			class CGameObject;
			add_to_cast_list						(CGameObject,		CObject);
#			undef cast_type_list
#			define cast_type_list save_cast_list	(CGameObject,		CObject)
#		endif

		DECLARE_SPECIALIZATION	(CEntity,			CGameObject,		cast_entity);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CEntity,			CGameObject)

		DECLARE_SPECIALIZATION	(CEntityAlive,		CGameObject,		cast_entity_alive);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CEntityAlive,		CGameObject)

		DECLARE_SPECIALIZATION	(CInventoryItem,	CGameObject,		cast_inventory_item);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CInventoryItem,	CGameObject)

		DECLARE_SPECIALIZATION	(CInventoryOwner,	CGameObject,		cast_inventory_owner);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CInventoryOwner,	CGameObject)

		DECLARE_SPECIALIZATION	(CActor,			CGameObject,		cast_actor);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CActor,			CGameObject)

		DECLARE_SPECIALIZATION	(CGameObject,		CInventoryOwner,	cast_game_object);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CGameObject,		CInventoryOwner)

#	endif
	
#	ifndef DO_NOT_DECLARE_TYPE_LIST
#		include "smart_cast_impl1.h"
#	endif
#endif

#endif //SMART_CAST_H