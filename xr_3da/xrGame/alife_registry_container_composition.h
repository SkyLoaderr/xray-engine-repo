////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_container_composition.h
//	Created 	: 01.07.2004
//  Modified 	: 01.07.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry container class, composition of the registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_abstract_registry.h"

#pragma warning(push)
#pragma warning(disable:4005)

typedef CALifeAbstractRegistry<u16,xr_vector<u16> > CInfoPortionRegistry;
add_to_registry_type_list(CInfoPortionRegistry)
#define info_portions define_constant(CInfoPortionRegistry) 
#define registry_type_list save_registry_type_list(CInfoPortionRegistry)




#pragma warning(pop)