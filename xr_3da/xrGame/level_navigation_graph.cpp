////////////////////////////////////////////////////////////////////////////
//	Module 		: level_navigation_graph.cpp
//	Created 	: 02.10.2001
//  Modified 	: 27.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Level navigation graph
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_navigation_graph.h"
#include "level_navigation_graph_space.h"
#include "graph_abstract.h"

typedef LevelNavigationGraph::CCellVertex::_use_type _use_type;

const _use_type up		= 1 << 3;
const _use_type right	= 1 << 2;
const _use_type down	= 1 << 1;
const _use_type left	= 1 << 0;

IC	CLevelNavigationGraph::CCellVertex &CLevelNavigationGraph::vertex_by_group_id	(
	VERTEX_VECTOR &vertices,
	u32 group_id
)
{
	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if ((*I).m_mark == group_id)
			return			(*I);

	NODEFAULT;
#ifdef DEBUG
	static CCellVertex		last_result(u32(-1),0,0);
	return					(last_result);
#endif
}

IC	bool CLevelNavigationGraph::connected		(CCellVertex &vertex, VERTEX_VECTOR &vertices, u32 group_id, u32 link, u32 use)
{
	u32						_link = this->vertex(vertex.m_vertex_id)->link(link);
	if (!valid_vertex_id(_link))
		return				(false);
	if (this->vertex(_link)->link((link + 2) & 3) != vertex.m_vertex_id)
		return				(false);

	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if (_link == (*I).m_vertex_id) {
			if ((*I).m_mark)
				return		(false);

			(*I).m_mark		= group_id;
			(*I).m_use		|= use;
			m_cross[_link]	= &*I;
			vertex			= *I;
			return			(true);
		}
	return					(false);
}

IC	bool CLevelNavigationGraph::connected		(CCellVertex &vertex1, CCellVertex &vertex2, VERTEX_VECTOR &vertices, u32 group_id, u32 use)
{
	u32						link1 = vertex(vertex1.m_vertex_id)->link(2);
	if (!valid_vertex_id(link1))
		return				(false);

	u32						link2 = vertex(vertex2.m_vertex_id)->link(1);
	if (link1 != link2)
		return				(false);

	if (vertex(link1)->link(0) != vertex1.m_vertex_id)
		return				(false);

	if (vertex(link1)->link(3) != vertex2.m_vertex_id)
		return				(false);

	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if (link1 == (*I).m_vertex_id) {
			if ((*I).m_mark)
				return		(false);

			(*I).m_mark		= group_id;
			(*I).m_use		|= use;
			m_cross[link2]	= &*I;
			return			(true);
		}
	return					(false);
}

IC	void CLevelNavigationGraph::remove_marks	(VERTEX_VECTOR &vertices, u32 group_id)
{
	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if ((*I).m_mark == group_id) {
			(*I).m_data				= 0;
			m_cross[(*I).m_vertex_id]	= 0;
			return;
		}
	NODEFAULT;
}

IC	void CLevelNavigationGraph::build_sectors	(u32 i, u32 j, CCellVertex &cell_vertex, u32 &group_id)
{
	++group_id;
	cell_vertex.m_mark			= group_id;
	cell_vertex.m_use			= left | up;
	m_cross[cell_vertex.m_vertex_id]	= &cell_vertex;
	CCellVertex					v = cell_vertex, v1;

	LINE_VECTOR					&vi = m_marks[i];
	for (u32 j2 = j + 1; j2<=max_x(); ++j2)
		if (vi[j2].empty() || !connected(v,vi[j2],group_id,2,up))
			break;

	CCellVertex					&_v = vertex_by_group_id(vi[j2-1],group_id);
	_v.m_use					|= right;
	v							= _v;
	bool						ok = true;

	LINE_VECTOR::iterator		_j2, j1, j1_1, i1_1_j1;
	MARK_TABLE::iterator		i1 = m_marks.begin() + i + 1, i1_1 = i1 - 1;
	MARK_TABLE::iterator		e = m_marks.end();
	for ( ; i1 != e; ++i1, ++i1_1) {
		VERTEX_VECTOR			&table_i1_j = (*i1)[j];
		if (table_i1_j.empty())
			goto enough;

		v1						= vertex_by_group_id((*i1_1)[j],group_id);
		if (!connected(v1,table_i1_j,group_id,1,left))
			goto enough;
		
		j1						= (*i1).begin() + j + 1;
		j1_1					= j1 - 1;
		i1_1_j1					= (*i1_1).begin() + j + 1;
		_j2						= (*i1).begin() + j2;
		for ( ; j1 != _j2; ++j1, ++j1_1, ++i1_1_j1) {
			v					= vertex_by_group_id(*j1_1,group_id);
			v1					= vertex_by_group_id(*i1_1_j1,group_id);
			if ((*j1).empty() || !connected(v,v1,*j1,group_id,0)) {
				ok				= false;

				LINE_VECTOR::iterator	J = (*i1).begin() + j;
				for ( ; J!=j1; ++J)
					remove_marks		(*J,group_id);
				break;
			}
		}

		if (ok) {
			CCellVertex					&_v = vertex_by_group_id(*j1_1,group_id);
			_v.m_use					|= right;
			v							= _v;
			continue;
		}

enough:
		{
			LINE_VECTOR::iterator		j1 = (*i1_1).begin() + j;
			LINE_VECTOR::iterator		_j2 = (*i1_1).begin() + j2;
			for ( ; j1 != _j2; ++j1)
				vertex_by_group_id(*j1,group_id).m_use	|= down;
			break;
		}
	}
	
	sectors().add_vertex		(CSector(cell_vertex.m_vertex_id,vertex_by_group_id((*i1_1)[j2 - 1],group_id).m_vertex_id),group_id - 1);

#ifdef DEBUG
	m_global_count				+= (u32(i1 - m_marks.begin()) - i)*(j2 - j);
#endif
}

IC	void CLevelNavigationGraph::init_marks	()
{
	m_cross.assign			(header().vertex_count(),0);
	m_marks.resize			(max_z() + 1);
	u32						size_x = max_x() + 1;
	MARK_TABLE::iterator	I = m_marks.begin();
	MARK_TABLE::iterator	E = m_marks.end();
	for ( ; I != E; ++I)
		(*I).resize			(size_x);
}

IC	void CLevelNavigationGraph::fill_marks		()
{
	u32									cur_x, cur_z;
	CCellVertex							v(0,0,0);
	const_vertex_iterator				I = begin();
	const_vertex_iterator				E = end();
	for ( ; I != E; ++I, ++v.m_vertex_id) {
		const CPosition					&position = (*I).position();
		cur_z							= position.z(row_length());
		cur_x							= position.x(row_length());
		m_marks[cur_z][cur_x].push_back	(v);
	}
}

IC	void CLevelNavigationGraph::build_sectors	()
{
	u32								group_id = 0;
	MARK_TABLE::iterator			I = m_marks.begin(), B = m_marks.begin();
	MARK_TABLE::iterator			E = m_marks.end();
	for ( ; I != E; ++I) {
		LINE_VECTOR::iterator		i = (*I).begin(), b = (*I).begin();
		LINE_VECTOR::iterator		e = (*I).end();
		for ( ; i != e; ++i) {
			VERTEX_VECTOR::iterator	II = (*i).begin();
			VERTEX_VECTOR::iterator	EE = (*i).end();
			for ( ; II != EE; ++II) {
				if ((*II).m_mark)
					continue;
				build_sectors		(u32(I - B),u32(i - b),*II,group_id);
			}
		}
	}
}

#ifdef DEBUG
IC	void CLevelNavigationGraph::check_vertices	()
{
	VERIFY							(m_global_count == header().vertex_count());
	MARK_TABLE::iterator			I = m_marks.begin(), B = m_marks.begin();
	MARK_TABLE::iterator			E = m_marks.end();
	for ( ; I != E; ++I) {
		LINE_VECTOR::iterator		i = (*I).begin(), b = (*I).begin();
		LINE_VECTOR::iterator		e = (*I).end();
		for ( ; i != e; ++i) {
			VERTEX_VECTOR::iterator	II = (*i).begin();
			VERTEX_VECTOR::iterator	EE = (*i).end();
			for ( ; II != EE; ++II) {
				VERIFY				((*II).m_mark);
			}
		}
	}
}
#endif

#if 1
struct CRemoveNonBorderPredicate {
	IC	bool operator() (const LevelNavigationGraph::CCellVertex *v) const
	{
		return					(!v->m_use);
	}
};
#endif

IC	void CLevelNavigationGraph::build_edges		()
{
#if 1
	const_vertex_iterator		i = begin(), b = i;
	const_vertex_iterator		e = end();
	for ( ; i != e; ++i) {
		u32						current_vertex_id = u32(i - b);
		
		CCellVertex				*current_cell = m_cross[current_vertex_id];
		VERIFY					(current_cell);

		if (!current_cell->m_use)
			continue;

		u32						current_mark = current_cell->m_mark - 1;
		CSectorGraph::CVertex	*sector_vertex = sectors().vertex(current_mark);
		u32						usage = current_cell->m_use;
		u32						I;
		do {
			I					= usage;
			usage				&= usage - 1;
			I					^= usage;
			I					= (I >> 1) + 1;
			I					= (I ^ (I >> 2)) - 1;

			u32					vertex_id = (*i).link(I);
			if (!valid_vertex_id(vertex_id))
				continue;

			CCellVertex			*cell = m_cross[vertex_id];
			VERIFY				(cell);

			u32					mark = cell->m_mark - 1;
			VERIFY				(mark != current_mark);

			if (sector_vertex->edge(mark))
				continue;

			sectors().add_edge	(current_mark,mark,1.f);
		}
		while (usage);
	}
#else
#if 0
	const_vertex_iterator		i;
	CROSS_TABLE::iterator		_i = m_cross.begin();
	CROSS_TABLE::iterator		_e = m_cross.end();
	for ( ; _i != _e; ++_i) {
		if (!(*_i)->m_use)
			continue;

		CCellVertex				*current_cell = *_i;
		VERIFY					(current_cell);

		i						= vertex(current_cell->m_vertex_id);

		u32						current_mark = current_cell->m_mark - 1;
		CSectorGraph::CVertex	*sector_vertex = sectors().vertex(current_mark);
		u32						usage = current_cell->m_use;
		u32						I;
		do {
			I					= usage;
			usage				&= usage - 1;
			I					^= usage;
			I					= (I >> 1) + 1;
			I					= (I ^ (I >> 2)) - 1;

			u32					vertex_id = (*i).link(I);
			if (!valid_vertex_id(vertex_id))
				continue;

			CCellVertex			*cell = m_cross[vertex_id];
			VERIFY				(cell);

			u32					mark = cell->m_mark - 1;
			VERIFY				(mark != current_mark);

			if (sector_vertex->edge(mark))
				continue;

			sectors().add_edge	(current_mark,mark,1.f);
		}
		while (usage);
	}
#else
	CROSS_TABLE					temp = m_cross;
	CROSS_TABLE::iterator		_e = remove_if(temp.begin(),temp.end(),CRemoveNonBorderPredicate());
	CROSS_TABLE::iterator		_i = temp.begin();
	for ( ; _i != _e; ++_i) {
		const_vertex_iterator	i = vertex((*_i)->m_vertex_id);
		CCellVertex				*current_cell = *_i;
		VERIFY					(current_cell);
		VERIFY					(current_cell->m_use);

		u32						current_mark = current_cell->m_mark - 1;
		CSectorGraph::CVertex	*sector_vertex = sectors().vertex(current_mark);
		u32						usage = current_cell->m_use;
		u32						I;
		do {
			I					= usage;
			usage				&= usage - 1;
			I					^= usage;
			I					= (I >> 1) + 1;
			I					= (I ^ (I >> 2)) - 1;

			u32					vertex_id = (*i).link(I);
			if (!valid_vertex_id(vertex_id))
				continue;

			CCellVertex			*cell = m_cross[vertex_id];
			VERIFY				(cell);

			u32					mark = cell->m_mark - 1;
			VERIFY				(mark != current_mark);

			if (sector_vertex->edge(mark))
				continue;

			sectors().add_edge	(current_mark,mark,1.f);
		}
		while (usage);
	}
#endif
#endif
}

#ifdef DEBUG
IC	void CLevelNavigationGraph::check_edges		()
{
	u32									count = 0;
	CSectorGraph::const_vertex_iterator	I = sectors().vertices().begin();
	CSectorGraph::const_vertex_iterator	E = sectors().vertices().end();
	for ( ; I != E; ++I) {
//		VERIFY							(!(*I).second->edges().empty());
		if (!(*I).second->edges().empty())
			continue;

		++count;
		if ((*I).second->data().min_vertex_id() == (*I).second->data().max_vertex_id())
			Msg							("! Node %d is not connected to the graph!",(*I).second->data().min_vertex_id());
		else
			Msg							("! Sector [%d][%d] is not connected to the graph!",(*I).second->data().min_vertex_id(),(*I).second->data().max_vertex_id());
	}
	if (count) {
		if (count == 1)
			Msg							("! There is single not connected island");
		else
			Msg							("! There are %d not connected islands",count);
	}
}
#endif

CLevelNavigationGraph::CLevelNavigationGraph	(
#ifdef AI_COMPILER
		LPCSTR file_name, 
		u32 current_version
	) : 
	inherited	(
		file_name,
		current_version
#endif
	)
{
	u64				start, finish;
	m_sectors		= xr_new<CSectorGraph>();
#ifdef DEBUG
	m_global_count	= 0;
#endif
	start			= CPU::GetCycleCount();
	init_marks		();
	fill_marks		();
	build_sectors	();
#ifdef DEBUG
	check_vertices	();
#endif
	build_edges		();
#ifdef DEBUG
	check_edges		();
#endif
	m_marks.clear	();
	m_cross.clear	();
	finish			= CPU::GetCycleCount();
#ifdef DEBUG
	Msg				("* Navigation info : %d nodes, %d sectors, %d edges (%f seconds)",header().vertex_count(),sectors().vertex_count(),sectors().edge_count(),CPU::cycles2seconds*float(finish - start));
#endif
}

CLevelNavigationGraph::~CLevelNavigationGraph	()
{
	xr_delete		(m_sectors);
}
