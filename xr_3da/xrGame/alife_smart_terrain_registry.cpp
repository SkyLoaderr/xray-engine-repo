////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_smart_terrain_registry.cpp
//	Created 	: 20.09.2005
//  Modified 	: 20.09.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife smart terrain registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_smart_terrain_registry.h"
#include "xrServer_Objects_ALife_Monsters.h"

CALifeSmartTerrainRegistry::~CALifeSmartTerrainRegistry	()
{
}

void CALifeSmartTerrainRegistry::add	(CSE_ALifeDynamicObject *object)
{
	CSE_ALifeSmartZone			*zone = smart_cast<CSE_ALifeSmartZone*>(object);
	if (!zone)
		return;

	if (objects().find(object->ID) != objects().end()) {
		THROW2					((*(objects().find(object->ID))).second == object,"The specified object is already presented in the Object Registry!");
		THROW2					((*(objects().find(object->ID))).second != object,"Object with the specified ID is already presented in the Object Registry!");
	}

	m_objects.insert			(std::make_pair(object->ID,zone));
}
