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
#include "space_restrictor.h"
#include "graph_engine.h"

struct CBorderMergePredicate {
	CSpaceRestrictionShape			*m_restriction;

	IC			CBorderMergePredicate	(CSpaceRestrictionShape *restriction)
	{
		m_restriction				= restriction;
	}

	IC	void	operator()				(const CLevelGraph::CVertex &vertex) const
	{
		if (m_restriction->inside(ai().level_graph().vertex_id(&vertex),true) && !m_restriction->inside(ai().level_graph().vertex_id(&vertex),false))
			m_restriction->m_border.push_back	(ai().level_graph().vertex_id(&vertex));
	}
};


#ifdef DEBUG
struct CShapeTestPredicate {
	CSpaceRestrictionShape			*m_restriction;

	IC			CShapeTestPredicate		(CSpaceRestrictionShape *restriction)
	{
		m_restriction				= restriction;
	}

	IC	void	operator()				(const CLevelGraph::CVertex &vertex) const
	{
		if (m_restriction->inside(ai().level_graph().vertex_id(&vertex),false))
			m_restriction->m_test_storage.push_back(ai().level_graph().vertex_id(&vertex));
	}
};
#endif


void CSpaceRestrictionShape::fill_shape		(const CCF_Shape::shape_def &shape)
{
	Fvector							start,dest;
	switch (shape.type) {
		case 0 : {
			start.sub				(Fvector().set(shape.data.sphere.P),Fvector().set(shape.data.sphere.R,0.f,shape.data.sphere.R));
			dest.add				(Fvector().set(shape.data.sphere.P),Fvector().set(shape.data.sphere.R,0.f,shape.data.sphere.R));
			start.add				(m_restrictor->Position());
			dest.add				(m_restrictor->Position());
			break;
		}
		case 1 : {
			Fvector					points[8] = {
				Fvector().set(-.5f,-.5f,-.5f),
				Fvector().set(-.5f,-.5f,+.5f),
				Fvector().set(-.5f,+.5f,-.5f),
				Fvector().set(-.5f,+.5f,+.5f),
				Fvector().set(+.5f,-.5f,-.5f),
				Fvector().set(+.5f,-.5f,+.5f),
				Fvector().set(+.5f,+.5f,-.5f),
				Fvector().set(+.5f,+.5f,+.5f)
			};
			start					= Fvector().set(flt_max,flt_max,flt_max);
			dest					= Fvector().set(flt_min,flt_min,flt_min);
			Fmatrix					Q;
			Q.mul_43				(m_restrictor->XFORM(),shape.data.box);
			Fvector					temp;
			for (int i=0; i<8; ++i) {
                Q.transform_tiny	(temp,points[i]);
				start.x				= _min(start.x,temp.x);
				start.y				= _min(start.y,temp.y);
				start.z				= _min(start.z,temp.z);
				dest.x				= _max(dest.x,temp.x);
				dest.y				= _max(dest.y,temp.y);
				dest.z				= _max(dest.z,temp.z);
			}
			break;
		}
		default : NODEFAULT;
	}
	ai().level_graph().iterate_vertices(start,dest,CBorderMergePredicate(this));

#ifdef DEBUG
	ai().level_graph().iterate_vertices(start,dest,CShapeTestPredicate(this));
#endif
}

void CSpaceRestrictionShape::build_border	()
{
	m_border.clear					();
	CCF_Shape						*shape = smart_cast<CCF_Shape*>(m_restrictor->collidable.model);
	VERIFY							(shape);
	xr_vector<CCF_Shape::shape_def>::const_iterator	I = shape->Shapes().begin();
	xr_vector<CCF_Shape::shape_def>::const_iterator	E = shape->Shapes().end();
	for ( ; I != E; ++I)
		fill_shape					(*I);
	
	process_borders					();

#ifdef DEBUG
	test_correctness				();
#endif
}

#ifdef DEBUG
void CSpaceRestrictionShape::test_correctness	()
{
	if (m_test_storage.empty()) {
		string128 s;
		sprintf(s,"Bad Shape for object [%s]", *m_restrictor->cName());
		VERIFY2(!m_test_storage.empty(), s);
	}

	ai().level_graph().set_mask		(border());

	xr_vector<u32>					nodes;
	ai().graph_engine().search		(ai().level_graph(), m_test_storage.back(), m_test_storage.back(), &nodes, GraphEngineSpace::CFlooder());

	ai().level_graph().clear_mask	(border());
	
	m_correct						= (m_test_storage.size() == nodes.size());
}
#endif

bool CSpaceRestrictionShape::inside	(const Fvector &position, float radius)
{
	VERIFY							(m_initialized);
	VERIFY							(m_restrictor);
	return							(CCF_Shape_inside((CCF_Shape*)m_restrictor->collidable.model,position,radius));
}

shared_str	CSpaceRestrictionShape::name() const
{
	VERIFY							(m_restrictor);
	return							(m_restrictor->cName());
}

