////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_object_location.h
//	Created 	: 27.11.2003
//  Modified 	: 27.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI object location
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAI_ObjectLocation {
protected:
	u32						m_dwLevelVertexID;
	CLevelGraph::CVertex	*m_tpLevelVertex;
	ALife::_GRAPH_ID		m_tGameVertexID;
public:
	IC						CAI_ObjectLocation	();
	IC	virtual				~CAI_ObjectLocation	();
	IC			void		Init				();
};

#include "ai_object_location_inline.h"