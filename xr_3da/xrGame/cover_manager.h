////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_manager.h
//	Created 	: 24.03.2004
//  Modified 	: 24.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "quadtree.h"

class CCoverPoint;

class CCoverManager {
public:
	typedef CQuadTree<CCoverPoint> CPointQuadTree;

protected:
	CPointQuadTree		*m_covers;
	xr_vector<bool>		m_temp;

protected:
	IC		bool					edge_vertex			(u32 index);
	IC		bool					cover				(CLevelGraph::CVertex *v, u32 index0, u32 index1);
	IC		bool					critical_point		(CLevelGraph::CVertex *v, u32 index, u32 index0, u32 index1);
	IC		bool					critical_cover		(u32 index);

public:
									CCoverManager		();
	virtual							~CCoverManager		();
			void					compute_static_cover();
	IC		CPointQuadTree			&covers				() const;
	IC		CPointQuadTree			*get_covers			();
			void					clear				();
};

#include "cover_manager_inline.h"