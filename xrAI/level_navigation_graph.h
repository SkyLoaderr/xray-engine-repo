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

#ifdef OPTIMAL_GRAPH
//#	define USE_HASH_SET
#	ifdef USE_HASH_SET
#		pragma warning(push)
#		pragma warning(disable:4995)
#		pragma warning(disable:4996)
#		include <hash_set>
#		pragma warning(pop)
#	endif
#endif

namespace LevelNavigationGraph {
	struct CCellVertex;
#ifdef OPTIMAL_GRAPH
	struct CCellVertexEx;
#endif
	class  CSector;
};

#include "level_navigation_graph_space.h"

template <typename, typename, typename> class CGraphAbstractSerialize;

#ifndef OPTIMAL_GRAPH

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

public:
	IC		const CSectorGraph	&sectors				() const;

protected:
	IC		CSectorGraph		&sectors				();
};

#else

struct sort_cells_predicate {
	ICF	bool	operator()	(const LevelNavigationGraph::CCellVertexEx *v0, const LevelNavigationGraph::CCellVertexEx *v1) const
	{
		u32					s0 = v0->m_down*v0->m_right;
		u32					s1 = v1->m_down*v1->m_right;
		if (s0 > s1)
			return			(true);
		else
			if (s0 < s1)
				return		(false);
		return				(v0 < v1);
	}
};

class CLevelNavigationGraph : public CLevelGraph {
private:
	typedef CLevelGraph									inherited;

public:
	typedef LevelNavigationGraph::CSector				CSector;
	typedef CGraphAbstractSerialize<CSector,u32,u32>	CSectorGraph;

public:
	typedef LevelNavigationGraph::CCellVertexEx			CCellVertex;
	typedef xr_vector<CCellVertex>						VERTEX_VECTOR;
	typedef xr_vector<VERTEX_VECTOR>					LINE_VECTOR;
	typedef xr_vector<LINE_VECTOR>						MARK_TABLE;

public:
	typedef xr_vector<CCellVertex>						CROSS_TABLE;
#ifdef USE_HASH_SET
	typedef std::hash_set<
				CCellVertex*,
				std::hash_compare<
					CCellVertex*,
					sort_cells_predicate
				>,
				xr_allocator_t<CCellVertex*>
			>	CROSS_PTABLE;
#else
	typedef xr_set<CCellVertex*,sort_cells_predicate>	CROSS_PTABLE;
#endif

private:
	CSectorGraph				*m_sectors;

// temporary structures for building sector hierarchy
private:
	CROSS_TABLE					m_cross;
	CROSS_PTABLE				m_temp;
	
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
	using CLevelGraph::vertex_id;

#ifndef AI_COMPILER
								CLevelNavigationGraph	();
#else
								CLevelNavigationGraph	(LPCSTR file_name, u32 current_version = XRAI_CURRENT_VERSION);
#endif
	virtual						~CLevelNavigationGraph	();

protected:
	IC		u32					vertex_id				(const CCellVertex &vertex) const;
	IC		u32					vertex_id				(const CCellVertex *vertex) const;
	IC		u32					vertex_id				(const CROSS_TABLE::const_iterator &vertex) const;
	IC		u32					vertex_id				(const CROSS_PTABLE::const_iterator &vertex) const;
	ICF		bool				check_left				(u32 vertex_id, u32 left_vertex_id) const;
	IC		void				fill_cell				(u32 start_vertex_id, u32 link);
	IC		void				fill_cell				(u32 start_vertex_id);
	IC		void				fill_cells				();
	IC		void				fast_sort				();
	IC		void				update_cell				(u32 start_vertex_id, u32 link);
	IC		void				update_cells			(u32 vertex_id, u32 right, u32 down);
	IC		void				select_sector			(CCellVertex *v, u32 &right, u32 &down, u32 max_square);
	IC		void				select_sector			(u32 &vertex_id, u32 &right, u32 &down);
	IC		void				build_sector			(u32 vertex_id, u32 right, u32 down, u32 group_id);
	IC		void				generate_sectors		();
	IC		void				generate_edges			();

public:
	IC		const CSectorGraph	&sectors				() const;

protected:
	IC		CSectorGraph		&sectors				();
};

#endif

#include "level_navigation_graph_inline.h"