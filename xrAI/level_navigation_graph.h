////////////////////////////////////////////////////////////////////////////
//	Module 		: level_navigation_graph.h
//	Created 	: 02.10.2001
//  Modified 	: 27.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Level navigation graph
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level_graph.h"

#ifdef AI_COMPILER
//#	define OPTIMAL_GRAPH
#endif

namespace LevelNavigationGraph {
	struct CCellVertex;
	class  CSector;
};

template <typename, typename, typename> class CGraphAbstractSerialize;

class CLevelNavigationGraph : public CLevelGraph {
private:
	typedef CLevelGraph									inherited;

public:
	typedef LevelNavigationGraph::CSector				CSector;
	typedef CGraphAbstractSerialize<CSector,u32,u32>	CSectorGraph;

public:
	typedef LevelNavigationGraph::CCellVertex			CCellVertex;
	typedef xr_vector<CCellVertex>						VERTEX_VECTOR;
	typedef xr_vector<VERTEX_VECTOR>					LINE_VECTOR;
	typedef xr_vector<LINE_VECTOR>						MARK_TABLE;

public:
	typedef xr_vector<CCellVertex*>						CROSS_TABLE;

private:
	CSectorGraph				*m_sectors;

// temporary structures for building sector hierarchy
private:
	MARK_TABLE					m_marks;
	CROSS_TABLE					m_cross;
#ifdef OPTIMAL_GRAPH
	CROSS_TABLE					m_temp;
#endif

#ifdef DEBUG
private:
	u32							m_global_count;

protected:
	IC		void				check_vertices			();
	IC		void				check_edges				();

public:
			void				render					();
#endif

public:
#ifndef AI_COMPILER
								CLevelNavigationGraph	();
#else
								CLevelNavigationGraph	(LPCSTR file_name, u32 current_version = XRAI_CURRENT_VERSION);
#endif
	virtual						~CLevelNavigationGraph	();

protected:
	IC		CCellVertex			&vertex_by_group_id		(VERTEX_VECTOR &vertices, u32 group_id);
	IC		bool				connected				(CCellVertex &vertex, VERTEX_VECTOR &vertices, u32 group_id, u32 link, u32 use);
	IC		bool				connected				(CCellVertex &vertex1, CCellVertex &vertex2, VERTEX_VECTOR &vertices, u32 group_id, u32 use);
	IC		void				remove_marks			(VERTEX_VECTOR &vertices, u32 group_id);
	IC		void				build_sectors			(u32 i, u32 j, CCellVertex &cell_vertex, u32 &group_id);
	IC		void				init_marks				();
	IC		void				fill_marks				();
	IC		void				build_sectors			();
	IC		void				build_edges				();

#ifdef OPTIMAL_GRAPH
protected:
	IC		void				fill_cross				();
	IC		void				fill_cell				(CCellVertex &v, u32 link);
	IC		void				fill_cells				();
	IC		void				update_cells			(u32 &vertex_id, u32 &right, u32 &down);
	IC		void				select_sector			(CCellVertex *v, u32 &right, u32 &down, u32 max_square);
	IC		bool				select_sector			(u32 &vertex_id, u32 &right, u32 &down);
	IC		void				build_sector			(u32 vertex_id, u32 right, u32 down, u32 &group_id);
	IC		void				generate_sectors		();
#endif

public:
	IC		const CSectorGraph	&sectors				() const;

protected:
	IC		CSectorGraph		&sectors				();
};

#include "level_navigation_graph_inline.h"