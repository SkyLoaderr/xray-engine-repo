////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_manager.cpp
//	Created 	: 24.03.2004
//  Modified 	: 24.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover manager class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cover_manager.h"
#include "cover_point.h"

CCoverManager::CCoverManager				()
{
	m_covers				= 0;
}

CCoverManager::~CCoverManager				()
{
	xr_delete				(m_covers);
}

void CCoverManager::compute_static_cover	()
{
	xr_delete				(m_covers);
	m_covers				= xr_new<CPointQuadTree>(ai().level_graph().header().box(),ai().level_graph().header().cell_size()*.5f,2*65536,2*65536);
	m_temp.resize			(ai().level_graph().header().vertex_count());

	for (u32 i=0, n = ai().level_graph().header().vertex_count(); i<n; ++i)
		if (ai().level_graph().vertex(i)->cover(0) + ai().level_graph().vertex(i)->cover(1) + ai().level_graph().vertex(i)->cover(2) + ai().level_graph().vertex(i)->cover(3))
			m_temp[i]		= edge_vertex(i);
		else
			m_temp[i]		= false;

	for (u32 i=0; i<n; ++i)
		if (m_temp[i] && critical_cover(i))
			m_covers->insert(xr_new<CCoverPoint>(ai().level_graph().vertex_position(ai().level_graph().vertex(i)),i));
}

void CCoverManager::clear					()
{
	m_covers->clear			();
}
