////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction.cpp
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction.h"
#include "space_restrictor_manager.h"
#include "space_restrictor.h"
#include "level.h"
#include "ai_space.h"
#include "level_graph.h"
#include "graph_engine.h"

extern bool CCF_Shape_inside(const CCF_Shape *self, const Fvector &position, float radius);

struct CBorderMergePredicate {
	CSpaceRestriction			*m_restriction;
	float						m_offset;
	Fvector						m_position;
	CLevelGraph::CVertex		*vertex;
	CLevelGraph::const_iterator	I;
	CLevelGraph::const_iterator	E;

	IC			CBorderMergePredicate	(CSpaceRestriction *restriction)
	{
		m_restriction			= restriction;
		m_offset				= ai().level_graph().header().cell_size()*.5f;
	}

	IC	bool	check					(u32 vertex_id)
	{
		m_position				= ai().level_graph().vertex_position(vertex_id);
		if	(
				m_restriction->inside(Fvector().set(m_position.x + m_offset,m_position.y,m_position.z + m_offset)) || 
				m_restriction->inside(Fvector().set(m_position.x + m_offset,m_position.y,m_position.z - m_offset)) || 
				m_restriction->inside(Fvector().set(m_position.x - m_offset,m_position.y,m_position.z + m_offset)) || 
				m_restriction->inside(Fvector().set(m_position.x - m_offset,m_position.y,m_position.z - m_offset))
			)
			return				(true);
		return					(false);
	}

	IC	bool	operator()				(u32 vertex_id)
	{
		if (check(vertex_id))
			return				(true);

		vertex					= ai().level_graph().vertex(vertex_id);
		ai().level_graph().begin(vertex_id,I,E);
		for ( ; I != E; ++I) {
			u32					neighbour = ai().level_graph().value(vertex,I);
			
			if (!ai().level_graph().valid_vertex_id(neighbour))
				continue;

			if (check(neighbour))
				return			(false);
		}

		return					(true);
	}
};

CSpaceRestriction::CSpaceRestriction	(ref_str space_restrictors)
{
	m_space_restrictors			= space_restrictors;
	m_applied					= false;
	m_initialized				= false;
	m_restrictor				= 0;
}

CSpaceRestriction::CSpaceRestriction	(CSpaceRestrictor *space_restrictor)
{
	VERIFY						(space_restrictor);
	m_space_restrictors			= space_restrictor->cName();
	m_applied					= false;
	m_initialized				= true;
	m_restrictor				= space_restrictor;
	build_border				();
}

CSpaceRestriction::~CSpaceRestriction	()
{
}

bool CSpaceRestriction::inside			(const Fvector &position, float radius)
{
	if (!m_initialized) {
		initialize				();
		if (!m_initialized)
			return				(true);
	}

	if (m_restrictor)
		return					(CCF_Shape_inside((CCF_Shape*)m_restrictor->collidable.model,position,radius));

	RESTRICTIONS::iterator		I = m_restrictions.begin();
	RESTRICTIONS::iterator		E = m_restrictions.end();
	for ( ; I != E; ++I)
		if ((*I)->inside(position,radius))
			return				(true);

	return						(false);
}

void CSpaceRestriction::initialize		()
{
	string256					temp;
	
	u32							n = _GetItemCount(*m_space_restrictors);
	VERIFY						(n);
	if (n == 1)
		return;

	for (u32 i=0; i<n ;++i)
		merge					(
			Level().space_restrictor_manager().restriction(
				ref_str(
					_GetItem(*m_space_restrictors,i,temp)
				)
			)
		);
	
	process_borders				();
	
	m_initialized				= true;
}

void CSpaceRestriction::process_borders	()
{
	std::sort						(m_border.begin(),m_border.end());
	xr_vector<u32>::iterator		I = unique(m_border.begin(),m_border.end());
	m_border.erase					(I,m_border.end());
}

void CSpaceRestriction::merge			(CSpaceRestriction *restriction)
{
	m_restrictions.push_back		(restriction);
	m_border.insert					(m_border.begin(),restriction->border().begin(),restriction->border().end());
}

void CSpaceRestriction::add_border		()
{
	VERIFY							(!m_applied);
	m_applied						= true;
	ai().level_graph().set_mask		(border());
}

void CSpaceRestriction::remove_border	()
{
	VERIFY							(m_applied);
	m_applied						= false;
	ai().level_graph().clear_mask	(border());
}

void CSpaceRestriction::build_border	()
{
	m_border.clear					();
	CCF_Shape						*shape = dynamic_cast<CCF_Shape*>(m_restrictor->collidable.model);
	VERIFY							(shape);
	xr_vector<CCF_Shape::shape_def>::const_iterator	I = shape->Shapes().begin();
	xr_vector<CCF_Shape::shape_def>::const_iterator	E = shape->Shapes().end();
	for ( ; I != E; ++I) {
		u32							vertex_id = ai().level_graph().vertex_id(Fvector().add(position(*I),m_restrictor->Position()));
		VERIFY						(ai().level_graph().valid_vertex_id(vertex_id));
		ai().graph_engine().search	(
			ai().level_graph(),
			vertex_id,
			vertex_id,
			&m_border,
			CGraphEngine::CFlooder(
				radius(*I) + _sqrt(2.5f)*ai().level_graph().header().cell_size()
			)
		);
	}
	
	{
		xr_vector<u32>::iterator	I = remove_if(m_border.begin(),m_border.end(),CBorderMergePredicate(this));
		m_border.erase				(I,m_border.end());
	}
	
	process_borders					();
}
