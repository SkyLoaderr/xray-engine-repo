////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export_space.h
//	Created 	: 22.09.2003
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export space 
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef SCRIPT_REGISTRATOR
#	undef	STATIC_CHECK
#	include <typelist.h>
#	define	script_type_list		Loki::NullType
#	define	add_to_type_list(type)	namespace SR {typedef Loki::TL::Append<script_type_list,type>::Result TypeList_##type; };
#	define	save_type_list(type)	SR::TypeList_##type
#else
#	define	script_type_list		
#	define	add_to_type_list(type)	;
#	define	save_type_list(type)		
#endif