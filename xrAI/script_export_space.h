////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export_space.h
//	Created 	: 22.09.2003
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export space 
////////////////////////////////////////////////////////////////////////////

#pragma once

struct lua_State;

#ifdef SCRIPT_REGISTRATOR
#	undef	STATIC_CHECK
#	include <typelist.h>
#	define	script_type_list		Loki::NullType
#	define	add_to_type_list(type)	typedef Loki::Typelist<type,script_type_list> TypeList_##type;
#	define	save_type_list(type)	TypeList_##type
#else
#	define	script_type_list		
#	define	add_to_type_list(type)	;
#	define	save_type_list(type)		
#endif

#define DECLARE_SCRIPT_REGISTER_FUNCTION static void script_register(lua_State *);
