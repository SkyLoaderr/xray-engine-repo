////////////////////////////////////////////////////////////////////////////
//	Module 		: stdafx.h
//	Created 	: 20.03.2004
//  Modified 	: 20.03.2004
//	Author		: Dmitriy Iassenev
//	Description : precompiled header
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smart_cover.h"
#include "quadtree.h"
#include "level_graph.h"

class CCoverPoint {
public:
	Fvector			m_position;
	u32				m_level_vertex_id;
	u8				m_cover[4][2];

	IC					CCoverPoint	(const Fvector &point) :
							m_position	(point)
	{
	}

	IC	const Fvector	&position	() const
	{
		return		(m_position);
	}

	IC	bool			operator==	(const CCoverPoint &point) const
	{
		return		(!!position().similar(point.position()));
	}
};

typedef CQuadTree<CCoverPoint>				CCoverQuadTree;

void smart_cover(LPCSTR project_name)
{
	u64						s,f;
	CLevelGraph				*graph	= xr_new<CLevelGraph>(project_name);
	CCoverQuadTree			*tree	= xr_new<CCoverQuadTree>(graph->header().box(),graph->header().cell_size()*.5f,65536,65536);
	u32						n = graph->header().vertex_count();
	xr_vector<CCoverPoint*>	objects;
	objects.resize			(n);
	
//	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep					(1);

	for (u32 ii=0; ii<10; ++ii) {
		for (u32 i=0; i<n; ++i)
			objects[i]			= ::Random.randI(0,300) ? 0 : xr_new<CCoverPoint>(graph->vertex_position(graph->vertex(i)->position()));

		s						= CPU::GetCycleCount();
		for (u32 i=0; i<n; ++i)
			if (objects[i])
				tree->insert	(objects[i]);
		f						= CPU::GetCycleCount();
		Msg						("Total insert time %f (%d tests : %f)",CPU::cycles2seconds*float(f - s),graph->header().vertex_count(),CPU::cycles2microsec*float(f - s)/float(graph->header().vertex_count()));

//		s						= CPU::GetCycleCount();
//		for (u32 i=0; i<n; ++i) {
//			CCoverPoint			*point = tree->find(graph->vertex_position(graph->vertex(i)->position()));
//			VERIFY				(point->position().similar(graph->vertex_position(graph->vertex(i)->position())));
//		}
//		f						= CPU::GetCycleCount();
//		Msg						("Total search time %f (%d tests : %f)",CPU::cycles2seconds*float(f - s),graph->header().vertex_count(),CPU::cycles2microsec*float(f - s)/float(graph->header().vertex_count()));
//
		xr_vector<CCoverPoint*>	nearest;
		float					search_radius = 100.f;
		nearest.reserve			(1000);
		s						= CPU::GetCycleCount();
		for (u32 i=0; i<n; ++i) {
			tree->nearest		(graph->vertex_position(graph->vertex(i)->position()),search_radius,nearest);
//			Fvector				position = graph->vertex_position(graph->vertex(i)->position());
//			for (u32 j=0; j<n; ++j) {
//				Fvector			next_position = graph->vertex_position(graph->vertex(j)->position());
//				if (position.distance_to_sqr(next_position) < _sqr(search_radius) + EPS_S) {
//					xr_vector<CCoverPoint*>::const_iterator	I = nearest.begin();
//					xr_vector<CCoverPoint*>::const_iterator	E = nearest.end();
//					bool		ok = false;
//					for ( ; I != E; ++I)
//						if ((*I)->position().similar(next_position)) {
//							ok	= true;
//							break;
//						}
//					VERIFY		(ok);
//				}
//			}
//			xr_vector<CCoverPoint*>::const_iterator	I = nearest.begin();
//			xr_vector<CCoverPoint*>::const_iterator	E = nearest.end();
//			for ( ; I != E; ++I) {
//				VERIFY			((*I)->position().distance_to(position) < search_radius + EPS_L);
//			}
//			if (i % 100 == 0)
//				Msg				("%6d",i);
		}
		f						= CPU::GetCycleCount();
		Msg						("Total nearest(%f metres) search time %f (%d tests : %f)",search_radius,CPU::cycles2seconds*float(f - s),graph->header().vertex_count(),CPU::cycles2microsec*float(f - s)/float(graph->header().vertex_count()));

		s						= CPU::GetCycleCount();
		for (u32 i=0; i<n; ++i)
			if (objects[i])
				tree->remove	(objects[i]);
		f						= CPU::GetCycleCount();
		Msg						("Total remove time %f (%d tests : %f)",CPU::cycles2seconds*float(f - s),graph->header().vertex_count(),CPU::cycles2microsec*float(f - s)/float(graph->header().vertex_count()));

		for (u32 i=0; i<n; ++i)
			xr_delete			(objects[i]);
	}
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	xr_delete				(tree);
}