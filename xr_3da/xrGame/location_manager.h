////////////////////////////////////////////////////////////////////////////
//	Module 		: location_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Location manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

class CLocationManager {
private:
	ALife::TERRAIN_VECTOR	m_vertex_types;

protected:
			void	setup_location_types	(LPCSTR S, LPCSTR section);

public:
					CLocationManager		();
	virtual			~CLocationManager		();
			void	init					();
	virtual void	Load					(LPCSTR section);
	virtual void	reinit					();
	virtual void	reload					(LPCSTR section);
	IC		const ALife::TERRAIN_VECTOR &vertex_types	() const;
};

#include "location_manager_inline.h"
