////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_shape.cpp
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction shape
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction_shape.h"
#include "ai_space.h"
#include "level_graph.h"
#include "graph_engine.h"
#include "space_restrictor.h"

struct CBorderMergePredicate {
	CSpaceRestrictionShape		*m_restriction;
	CLevelGraph::CVertex		*vertex;
	CLevelGraph::const_iterator	I;
	CLevelGraph::const_iterator	E;

	IC			CBorderMergePredicate	(CSpaceRestrictionShape *restriction)
	{
		m_restriction			= restriction;
	}

	IC	bool	operator()				(u32 vertex_id)
	{
		if (m_restriction->inside(vertex_id))
			return				(true);

		vertex					= ai().level_graph().vertex(vertex_id);
		ai().level_graph().begin(vertex_id,I,E);
		for ( ; I != E; ++I) {
			u32					neighbour = ai().level_graph().value(vertex,I);
			
			if (!ai().level_graph().valid_vertex_id(neighbour))
				continue;

			if (m_restriction->inside(neighbour))
				return			(false);
		}

		return					(true);
	}
};

void CSpaceRestrictionShape::build_border	()
{
	m_border.clear					();
	CCF_Shape						*shape = smart_cast<CCF_Shape*>(m_restrictor->collidable.model);
	VERIFY							(shape);
	xr_vector<CCF_Shape::shape_def>::const_iterator	I = shape->Shapes().begin();
	xr_vector<CCF_Shape::shape_def>::const_iterator	E = shape->Shapes().end();
	for ( ; I != E; ++I) {
		u32							vertex_id = m_restrictor->level_vertex_id();//ai().level_graph().vertex_id(Fvector().add(position(*I),m_restrictor->Position()));
		VERIFY						(ai().level_graph().valid_vertex_id(vertex_id));
		ai().graph_engine().search	(
			ai().level_graph(),
			vertex_id,
			vertex_id,
			&m_border,
			CGraphEngine::CFlooder(
				radius(*I) + 2*ai().level_graph().header().cell_size()
			)
		);
	}
	
	{
		xr_vector<u32>::iterator	I = remove_if(m_border.begin(),m_border.end(),CBorderMergePredicate(this));
		m_border.erase				(I,m_border.end());
	}
	
	process_borders					();
}

bool CSpaceRestrictionShape::inside	(const Fvector &position, float radius)
{
	VERIFY						(m_initialized);
	VERIFY						(m_restrictor);
	return						(CCF_Shape_inside((CCF_Shape*)m_restrictor->collidable.model,position,radius));
}

ref_str	CSpaceRestrictionShape::name() const
{
	VERIFY						(m_restrictor);
	return						(m_restrictor->cName());
}
