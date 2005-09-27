////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_smart_terrain_registry.h
//	Created 	: 20.09.2005
//  Modified 	: 20.09.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife smart terrain registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

class CSE_ALifeDynamicObject;
class CSE_ALifeSmartZone;

class CALifeSmartTerrainRegistry {
public:
	typedef xr_map<ALife::_OBJECT_ID,CSE_ALifeSmartZone*>	REGISTRY;

protected:
	REGISTRY				m_objects;

public:
	virtual					~CALifeSmartTerrainRegistry	();
			void			add							(CSE_ALifeDynamicObject *object);
	IC		const REGISTRY	&objects					() const;
};

#include "alife_smart_terrain_registry_inline.h"