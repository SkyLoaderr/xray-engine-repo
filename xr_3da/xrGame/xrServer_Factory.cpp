////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Factory.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects factory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_factory.h"

CSE_Abstract *F_entity_Create	(LPCSTR section)
{
	if (!pSettings->section_exist(section)) {
		if	(
				!xr_strcmp(section,"m_stalker_e") || 
				!xr_strcmp(section,"m_stalker_wolf") || 
				!xr_strcmp(section,"m_stalker_demo")
			)
			return	(object_factory().server_object(pSettings->r_clsid("stalker","class"),"stalker"));
		else
			return	(0);
	}
	return			(object_factory().server_object(pSettings->r_clsid(section,"class"),section));
}
