////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_composition.cpp
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction composition
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction_composition.h"
#include "space_restriction_holder.h"
#include "space_restriction_bridge.h"
#include "restriction_space.h"
#include "ai_space.h"
#include "level_navigation_graph.h"
#include "graph_engine.h"

struct CMergePredicate {
	CSpaceRestrictionComposition *m_restriction;

	IC	CMergePredicate	(CSpaceRestrictionComposition *restriction)
	{
		m_restriction	= restriction;
	}

	IC	bool operator()	(u32 level_vertex_id) const
	{
		return			(m_restriction->inside(level_vertex_id,false));
	}
};

IC	void CSpaceRestrictionComposition::merge	(CBaseRestrictionPtr restriction)
{
	m_restrictions.push_back	(restriction);
	m_border.insert				(m_border.begin(),restriction->border().begin(),restriction->border().end());
}

bool CSpaceRestrictionComposition::inside					(const Fvector &position, float radius)
{
	if (!initialized()) {
		initialize				();
		if (!initialized())
			return				(true);
	}

	RESTRICTIONS::iterator		I = m_restrictions.begin();
	RESTRICTIONS::iterator		E = m_restrictions.end();
	for ( ; I != E; ++I)
		if ((*I)->inside(position,radius))
			return				(true);

	return						(false);
}

void CSpaceRestrictionComposition::initialize	()
{
	string256					temp;
	u32							n = _GetItemCount(*m_space_restrictors);
	VERIFY						(n);
	if (n == 1) {
#ifdef DEBUG		
		m_correct				= true;
#endif
		return;
	}

	for (u32 i=0; i<n ;++i)
		if (!m_space_restriction_holder->restriction(_GetItem(*m_space_restrictors,i,temp))->initialized())
			return;

	for (u32 i=0; i<n ;++i)
		merge					(
			m_space_restriction_holder->restriction(
				_GetItem(
					*m_space_restrictors,
					i,
					temp
				)
			)
		);
	
	m_initialized				= true;

	xr_vector<u32>::iterator	I = remove_if(m_border.begin(),m_border.end(),CMergePredicate(this));
	m_border.erase				(I,m_border.end());

	process_borders				();

#ifdef DEBUG
	test_correctness			();
#endif
}

#ifdef DEBUG
void CSpaceRestrictionComposition::test_correctness()
{
	m_correct					= true;
	m_test_storage.clear		();

	//{
	//	RESTRICTIONS::iterator		I = m_restrictions.begin();
	//	RESTRICTIONS::iterator		E = m_restrictions.end();
	//	for ( ; I != E; ++I)
	//		if (!(*I)->object().m_correct)
	//			m_correct			= false;
	//}

	//if (!m_correct)
	//	return;

	{
		RESTRICTIONS::iterator		I = m_restrictions.begin();
		RESTRICTIONS::iterator		E = m_restrictions.end();
		for ( ; I != E; ++I)
			m_test_storage.insert	(m_test_storage.end(),(*I)->object().m_test_storage.begin(),(*I)->object().m_test_storage.end());
	}

	{
		std::sort					(m_test_storage.begin(),m_test_storage.end());
		xr_vector<u32>::iterator	I = unique(m_test_storage.begin(),m_test_storage.end());
		m_test_storage.erase		(I,m_test_storage.end());
	}

	xr_vector<u32>					nodes;
	{
		RESTRICTIONS::iterator		I = m_restrictions.begin();
		RESTRICTIONS::iterator		E = m_restrictions.end();
		for ( ; I != E; ++I) {
			nodes.clear						();
			ai().level_graph().set_mask		(border());
			ai().graph_engine().search		(ai().level_graph(), (*I)->object().m_test_storage.back(), (*I)->object().m_test_storage.back(), &nodes, GraphEngineSpace::CFlooder());
			ai().level_graph().clear_mask	(border());
			
			if (nodes.size() == 65535)		
				m_correct = true;
			else
				m_correct						= (m_test_storage.size() <= nodes.size());
			
			if (!m_correct)
				break;
		}
	}
}
#endif