////////////////////////////////////////////////////////////////////////////
//	Module 		: location_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Location manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"

class CLocationManager {
private:
	GameGraph::TERRAIN_VECTOR	m_vertex_types;
	CGameObject					*m_object;

protected:
			void	setup_location_types	(LPCSTR S, LPCSTR section);

public:
					CLocationManager		(CGameObject *object);
	virtual			~CLocationManager		();
	virtual void	Load					(LPCSTR section);
	virtual void	reload					(LPCSTR section);
	IC		const GameGraph::TERRAIN_VECTOR &vertex_types	() const;
};

#include "location_manager_inline.h"
