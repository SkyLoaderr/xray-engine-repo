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
#include "level_graph.h"

struct CMergePredicate {
	CSpaceRestrictionComposition *m_restriction;

	IC	CMergePredicate	(CSpaceRestrictionComposition *restriction)
	{
		m_restriction	= restriction;
	}

	IC	bool operator()	(u32 level_vertex_id) const
	{
		return			(m_restriction->CSpaceRestrictionBase::inside(level_vertex_id,false));
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
	if (n == 1)
		return;

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
}
