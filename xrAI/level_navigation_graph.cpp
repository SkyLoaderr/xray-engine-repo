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

#ifndef OPTIMAL_GRAPH

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

IC	void CLevelNavigationGraph::build_edges		()
{
	CROSS_TABLE::const_iterator	_i = m_cross.begin();
	const_vertex_iterator		i = begin();
	const_vertex_iterator		e = end();
	for ( ; i != e; ++i, ++_i) {
		CCellVertex				*current_cell = *_i;
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

			CSectorGraph::CVertex	*sector_vertex1 = sectors().vertex(mark);

			const CPosition		&min0 = vertex(sector_vertex->data().min_vertex_id())->position();
			const CPosition		&max0 = vertex(sector_vertex->data().max_vertex_id())->position();
			const CPosition		&min1 = vertex(sector_vertex1->data().min_vertex_id())->position();
			const CPosition		&max1 = vertex(sector_vertex1->data().max_vertex_id())->position();

			int					x0	 = (min0.x(row_length()) + max0.x(row_length())) >> 1;
			int					z0	 = (min0.z(row_length()) + max0.z(row_length())) >> 1;
			int					x1	 = (min1.x(row_length()) + max1.x(row_length())) >> 1;
			int					z1	 = (min1.z(row_length()) + max1.z(row_length())) >> 1;

			u32					edge_weight = _abs(x1 - x0) + _abs(z1 - z0);

			sectors().add_edge	(current_mark,mark,edge_weight);
		}
		while (usage);
	}
}

#ifdef DEBUG
IC	void CLevelNavigationGraph::check_edges		()
{
	u32									count = 0;
	CSectorGraph::const_vertex_iterator	I = sectors().vertices().begin();
	CSectorGraph::const_vertex_iterator	E = sectors().vertices().end();
	for ( ; I != E; ++I) {
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

#else

const _use_type up		= 1 << 0;
const _use_type right	= 1 << 1;
const _use_type down	= 1 << 2;
const _use_type left	= 1 << 3;

ICF	u32	CLevelNavigationGraph::vertex_id		(const CCellVertex &vertex) const
{
	return			(u32(&vertex - &*m_cross.begin()));
}

ICF	u32	CLevelNavigationGraph::vertex_id		(const CCellVertex *vertex) const
{
	return			(vertex_id(*vertex));
}

ICF	u32	CLevelNavigationGraph::vertex_id		(const CROSS_TABLE::const_iterator &vertex) const
{
	return			(vertex_id(*vertex));
}

ICF	u32	CLevelNavigationGraph::vertex_id		(const CROSS_PTABLE::const_iterator &vertex) const
{
	return			(vertex_id((*vertex).second));
}

ICF	bool CLevelNavigationGraph::check_left(u32 vertex_id, u32 left_vertex_id) const
{
	u32				link = vertex(vertex_id)->link(3);
	return			((left_vertex_id == link) && valid_vertex_id(link) && (vertex(link)->link(1) == vertex_id));
}

IC	void CLevelNavigationGraph::fill_cell		(u32 start_vertex_id, u32 link)
{
	for (u32 current_vertex_id = start_vertex_id, i = 1; ;++i) {
		u32					vertex_id = vertex(current_vertex_id)->link(link);
		if (!valid_vertex_id(vertex_id))
			break;

//		VERIFY				(!m_cross[vertex_id].m_mark);
		if (link == 1)
			m_cross[current_vertex_id].m_right_next = &m_cross[vertex_id];

		if (vertex(vertex_id)->link((link + 2) & 3) != current_vertex_id)
			break;

		current_vertex_id	= vertex_id;
	}

	current_vertex_id		= start_vertex_id;
	for (u32 j=0; j<i; ++j) {
		m_cross[current_vertex_id].m_dirs[link-1] = u16(i-j);
		current_vertex_id	= vertex(current_vertex_id)->link(link);
	}
}

IC	void CLevelNavigationGraph::fill_cell		(u32 start_vertex_id)
{
	u32							left_current_vertex_id = vertex(start_vertex_id)->link(3);
	if (!valid_vertex_id(left_current_vertex_id) || (vertex(left_current_vertex_id)->link(1) != start_vertex_id)) {
		m_cross[start_vertex_id].m_down_left = 1;
		return;
	}

	u32							current_vertex_id = start_vertex_id;
	u32							n = m_cross[start_vertex_id].m_down;
	u32							i = 1;
	for ( ; i<n; ++i) {
		u32						vertex_id = vertex(current_vertex_id)->link(2);
		u32						left_vertex_id = vertex(left_current_vertex_id)->link(2);
		if (!check_left(vertex_id,left_vertex_id))
			break;
		current_vertex_id		= vertex_id;
		left_current_vertex_id	= left_vertex_id;
	}

	current_vertex_id			= start_vertex_id;
	for (u32 j=0; j<i; ++j) {
		m_cross[current_vertex_id].m_down_left = u16(i-j);
		current_vertex_id		= vertex(current_vertex_id)->link(2);
	}
}

#define cell_id(cell) \
	(\
		(\
			u64(\
				u32(\
					(cell)->m_right\
				)\
				*\
				u32(\
					(cell)->m_down\
				)\
			)\
			<< 32\
		)\
		|\
		u64((void*)(cell))\
	)

IC	void CLevelNavigationGraph::fill_cells		()
{
	m_temp.clear			();

	CROSS_TABLE::iterator	I = m_cross.begin();
	CROSS_TABLE::iterator	E = m_cross.end();
	for ( ; I != E; ++I) {
		if (!(*I).m_right)
			fill_cell		(vertex_id(I),1);

		if (!(*I).m_down)
			fill_cell		(vertex_id(I),2);

		if (!(*I).m_down_left)
			fill_cell		(vertex_id(I));

		m_temp.insert		(std::make_pair(cell_id(&*I),&*I));
	}
}

#ifdef USE_COMPUTED
IC	void CLevelNavigationGraph::update_cell_computed(u32 start_vertex_id, u32 link)
{
	for (u32 current_vertex_id = start_vertex_id, i = 1; ;++i) {
		m_cross[current_vertex_id].m_all_dirs_computed = 0;
		u32					vertex_id = vertex(current_vertex_id)->link(link);
		if (!valid_vertex_id(vertex_id))
			break;

		if (m_cross[vertex_id].m_mark)
			break;

		if (vertex(vertex_id)->link((link + 2) & 3) != current_vertex_id)
			break;
		current_vertex_id	= vertex_id;
	}
}
#endif

IC	void CLevelNavigationGraph::update_cell		(u32 start_vertex_id, u32 link)
{
	for (u32 current_vertex_id = start_vertex_id, i = 1, index = (link + 1) & 3; ;++i) {
		u32					vertex_id = vertex(current_vertex_id)->link(link);
		if (!valid_vertex_id(vertex_id))
			break;

		if (m_cross[vertex_id].m_mark)
			break;

		if (vertex(vertex_id)->link((link + 2) & 3) != current_vertex_id)
			break;

#ifdef USE_COMPUTED
		update_cell_computed(vertex_id,link ^ 3);
#endif

		CCellVertex			*cell = &m_cross[vertex_id];

		m_temp.erase		(cell_id(cell));

		cell->m_dirs[index]	= (u16)i;

		m_temp.insert		(std::make_pair(cell_id(cell),cell));

		if (!link)
			cell->m_down_left = _min(cell->m_down,cell->m_down_left);
		current_vertex_id	= vertex_id;
	}
}

IC	void CLevelNavigationGraph::update_cells	(u32 vertex_id, u32 right, u32 down)
{
	for (u32 right_id = vertex_id, i=0; i<right; right_id = vertex(right_id)->link(1), ++i)
		update_cell			(right_id,0);

	for (u32 down_id = vertex_id, j=0; j<down; down_id = vertex(down_id)->link(2), ++j)
		update_cell			(down_id,3);
}

IC	void CLevelNavigationGraph::select_sector	(CCellVertex *v, u32 &right, u32 &down, u32 max_square)
{
//#ifdef _DEBUG
//	VERIFY					(!v->m_mark);
//#endif

	right					= 1;
	down					= v->m_down;
	u32						current_down = v->m_down;
	CCellVertex				*right_id = v->m_right_next;
	u32						i = 2;
	for	( ; i<=v->m_right; right_id = right_id->m_right_next, ++i) {
//#ifdef _DEBUG
//		VERIFY				(valid_vertex_id(right_id));
//		VERIFY				(!m_cross[right_id].m_mark);
//#endif
//		VERIFY				(right_id);

		current_down		= (u32)_min((s32)current_down,(s32)right_id->m_down_left);
		
		if (current_down*v->m_right <= max_square)
			return;

		if (i*current_down > max_square) {
			max_square		= i*current_down;
			right			= i;
			down			= current_down;
#ifdef USE_COMPUTED
			v->m_right_computed	= (u16)right;
			v->m_down_computed	= (u16)down;
#endif
			continue;
		}
	}
}

IC	void CLevelNavigationGraph::select_sector	(u32 &vertex_id, u32 &right, u32 &down)
{
	u32						max_square = 0, current_right, current_down;
	CROSS_PTABLE::iterator	I = m_temp.begin();
	CROSS_PTABLE::iterator	E = m_temp.end();

	select_sector			((*I).second,current_right,current_down,max_square);
	right					= current_right;
	down					= current_down;
	max_square				= right*down;
	vertex_id				= this->vertex_id(I);

	for (++I; I != E; ++I) {
		if (u32((*I).first >> 32) <= max_square)
			return;

#ifdef USE_COMPUTED
		if ((*I).second->m_all_dirs_computed) {
			current_right	= (*I).second->m_right_computed;
			current_down	= (*I).second->m_down_computed;
		}
		else
#endif
			select_sector	((*I).second,current_right,current_down,max_square);

		if (current_right*current_down <= max_square)
			continue;

		right				= current_right;
		down				= current_down;
		max_square			= right*down;
		vertex_id			= this->vertex_id(I);
	}
}

IC	void CLevelNavigationGraph::build_sector	(u32 vertex_id, u32 _right, u32 _down, u32 group_id)
{
//	Msg							("* Sector %5d (%3dx%3d) %6d",_right*_down,_right,_down,vertex_id);
	CCellVertex					*cell = 0, *right_id = &m_cross[vertex_id];
	u32							j, down_id, mask = left;
	for (u32 i=0; i<_right; right_id = right_id->m_right_next, ++i) {
		right_id->m_use			= mask | up;

		if (i == (_right-1))
			mask				|= right;
		
		for (down_id = this->vertex_id(right_id), j=0; j<_down; down_id = vertex(down_id)->link(2), ++j) {
			cell				= &m_cross[down_id];

			m_temp.erase		(cell_id(cell));
			VERIFY				(!cell->m_mark);
			cell->m_mark		= group_id;
			cell->m_use			|= mask;
			if (j == (_down-1)) {
				cell->m_use		|= down;
				break;
			}
		}

		mask					= 0;
	}

	sectors().add_vertex		(CSector(vertex_id,down_id),group_id - 1);

#ifdef DEBUG
	m_global_count				+= _right*_down;
#endif
}

IC	void CLevelNavigationGraph::generate_sectors()
{
	u32						group_id = 0;
	u32						id,right,down;
	
	u64						start, finish;

	start					= CPU::GetCycleCount();
	m_cross.resize			(header().vertex_count());
//	finish					= CPU::GetCycleCount();
//	Msg						("* Filling cross (%f seconds)",CPU::cycles2seconds*float(finish - start));

	fill_cells				();
//	finish					= CPU::GetCycleCount();
//	Msg						("* Filling cells (%f seconds)",CPU::cycles2seconds*float(finish - start));
	
#ifdef AI_COMPILER
#	ifdef DEBUG
		Phase				("generating sectors");
		Progress			(0.f);
#	endif
#endif
	while (!m_temp.empty()) {
		select_sector		(id,right,down);
		build_sector		(id,right,down,++group_id);
#ifdef AI_COMPILER
#	ifdef DEBUG
		Progress			((float(m_global_count)/float(header().vertex_count())));
#	endif
#endif
		update_cells		(id,right,down);
	}
	finish					= CPU::GetCycleCount();
//	Msg						("* Building sectors (%f seconds)",CPU::cycles2seconds*float(finish - start));
}

IC	void CLevelNavigationGraph::generate_edges	()
{
	CROSS_TABLE::const_iterator	_i = m_cross.begin();
	const_vertex_iterator		i = begin();
	const_vertex_iterator		e = end();
	for ( ; i != e; ++i, ++_i) {
		const CCellVertex		*current_cell = &*_i;
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

			CCellVertex			*cell = &m_cross[vertex_id];
			VERIFY				(cell);

			u32					mark = cell->m_mark - 1;
			VERIFY				(mark != current_mark);

			if (sector_vertex->edge(mark))
				continue;

			CSectorGraph::CVertex	*sector_vertex1 = sectors().vertex(mark);

			const CPosition		&min0 = vertex(sector_vertex->data().min_vertex_id())->position();
			const CPosition		&max0 = vertex(sector_vertex->data().max_vertex_id())->position();
			const CPosition		&min1 = vertex(sector_vertex1->data().min_vertex_id())->position();
			const CPosition		&max1 = vertex(sector_vertex1->data().max_vertex_id())->position();

			int					x0	 = (min0.x(row_length()) + max0.x(row_length())) >> 1;
			int					z0	 = (min0.z(row_length()) + max0.z(row_length())) >> 1;
			int					x1	 = (min1.x(row_length()) + max1.x(row_length())) >> 1;
			int					z1	 = (min1.z(row_length()) + max1.z(row_length())) >> 1;

			u32					edge_weight = _abs(x1 - x0) + _abs(z1 - z0);

			sectors().add_edge	(current_mark,mark,edge_weight);
		}
		while (usage);
	}
}

#ifdef DEBUG
IC	void CLevelNavigationGraph::check_vertices	()
{
	VERIFY						(m_global_count == header().vertex_count());
	CROSS_TABLE::const_iterator	I = m_cross.begin();
	CROSS_TABLE::const_iterator	E = m_cross.end();
	for ( ; I != E; ++I)
		VERIFY					((*I).m_mark);
}

IC	void CLevelNavigationGraph::check_edges		()
{
	u32									count = 0;
	CSectorGraph::const_vertex_iterator	I = sectors().vertices().begin();
	CSectorGraph::const_vertex_iterator	E = sectors().vertices().end();
	for ( ; I != E; ++I) {
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
	generate_sectors();
#ifdef DEBUG
	check_vertices	();
#endif
	generate_edges	();
#ifdef DEBUG
	check_edges		();
#endif
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

#endif