#include "stdafx.h"
#include "level_graph.h"
#include "profile.h"
#include "graph_abstract.h"

struct CCellVertex {
	u32		m_vertex_id;
	u32		m_mark;

			CCellVertex		()
	{
	}

			CCellVertex		(u32 vertex_id, u32 mark) :
				m_vertex_id(vertex_id),
				m_mark(mark)
	{
	}
};

struct CSector {
	u32		min_vertex_id;
	u32		max_vertex_id;

	IC		CSector(int) {}
	IC		CSector() {}
};

typedef CGraphAbstract<CSector,float,u32,u32>	CSectorGraph;
typedef xr_vector<CCellVertex>					VERTEX_VECTOR;
typedef xr_vector<VERTEX_VECTOR>				VERTEX_VECTOR1;
typedef xr_vector<VERTEX_VECTOR1>				VERTEX_VECTOR2;
typedef xr_vector<CCellVertex*>					CROSS_VECTOR;

IC	CCellVertex get_vertex_by_group_id(VERTEX_VECTOR &vertices, u32 group_id)
{
	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if ((*I).m_mark == group_id)
			return			(*I);

	NODEFAULT;
	return					(CCellVertex(u32(-1),0));
}

IC	bool connect(const CLevelGraph &level_graph, CCellVertex &vertex, VERTEX_VECTOR &vertices, u32 group_id, u32 link, CROSS_VECTOR &cross)
{
	u32						_link = level_graph.vertex(vertex.m_vertex_id)->link(link);
	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if (!(*I).m_mark && (_link == (*I).m_vertex_id)) {
			(*I).m_mark		= group_id;
			cross[_link]	= &*I;
			vertex			= *I;
			return			(true);
		}
	return					(false);
}

IC	bool connect(const CLevelGraph &level_graph, CCellVertex &vertex1, CCellVertex &vertex2, VERTEX_VECTOR &vertices, u32 group_id, CROSS_VECTOR &cross)
{
	u32						link1 = level_graph.vertex(vertex1.m_vertex_id)->link(2);
	u32						link2 = level_graph.vertex(vertex2.m_vertex_id)->link(1);
	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if (!(*I).m_mark && (link1 == (*I).m_vertex_id)) {
			if (link2 != (*I).m_vertex_id)
				return		(false);
			(*I).m_mark		= group_id;
			cross[link2]	= &*I;
			return			(true);
		}
	return					(false);
}

IC	void remove_mark(VERTEX_VECTOR &vertices, u32 group_id, CROSS_VECTOR &cross)
{
	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if ((*I).m_mark == group_id) {
			(*I).m_mark			= 0;
			cross[(*I).m_vertex_id]	= 0;
			return;
		}
	NODEFAULT;
}

u32 global_count = 0;

IC	void fill_mark(
	const CLevelGraph &level_graph, 
	CSectorGraph &sector_graph,
	VERTEX_VECTOR2 &table, 
	u32 i, 
	u32 j, 
	CCellVertex &cell_vertex, 
	u32 &group_id, 
	u32 min_z, 
	u32 max_z, 
	u32 min_x, 
	u32 max_x,
	CROSS_VECTOR &cross
)
{
	++group_id;
	cell_vertex.m_mark			= group_id;
	cross[cell_vertex.m_vertex_id] = &cell_vertex;
	CCellVertex					v = cell_vertex, v1;
	
	VERTEX_VECTOR1				&vi = table[i];
#if 0
	u32							j2 = j + 1;
	{
		VERTEX_VECTOR1::iterator	I = vi.begin() + j2;
		VERTEX_VECTOR1::iterator	E = vi.begin() + max_x + 1;//vi.end();
		for ( ; I != E; ++I)
			if ((*I).empty() || !connect(level_graph,v,*I,group_id,2,cross))
				break;
		j2							= u32(I - vi.begin());
	}
#else
	for (u32 j2 = j + 1; j2<=max_x; j2++)
		if (vi[j2].empty() || !connect(level_graph,v,vi[j2],group_id,2,cross))
			break;
#endif

	bool						ok = true;

#if 1
	VERTEX_VECTOR2::iterator	i1 = table.begin() + i + 1, i1_1 = i1 - 1;
	VERTEX_VECTOR2::iterator	e = table.end();
	for ( ; i1 != e; ++i1, ++i1_1) {
		VERTEX_VECTOR			&table_i1_j = (*i1)[j];
		if (table_i1_j.empty())
			break;

		v1						= get_vertex_by_group_id((*i1_1)[j],group_id);
		if (!connect(level_graph,v1,table_i1_j,group_id,1,cross))
			break;
		
		VERTEX_VECTOR1::iterator	j1 = (*i1).begin() + j + 1, j1_1 = j1 - 1, i1_1_j1 = (*i1_1).begin() + j + 1;
		VERTEX_VECTOR1::iterator	_j2 = (*i1).begin() + j2;
		for ( ; j1 != _j2; ++j1, ++j1_1, ++i1_1_j1) {
			v					= get_vertex_by_group_id(*j1_1,group_id);
			v1					= get_vertex_by_group_id(*i1_1_j1,group_id);
			if ((*j1).empty() || !connect(level_graph,v,v1,*j1,group_id,cross)) {
				ok				= false;

				VERTEX_VECTOR1::iterator	J = (*i1).begin() + j;
				for ( ; J!=j1; ++J)
					remove_mark	(*J,group_id,cross);
				break;
			}
		}

		if (!ok)
			break;
	}
	
	CSector						sector;
	sector.min_vertex_id		= cell_vertex.m_vertex_id;
	sector.max_vertex_id		= get_vertex_by_group_id((*i1_1)[j2 - 1],group_id).m_vertex_id;
	sector_graph.add_vertex		(sector,group_id - 1);

	global_count				+= (u32(i1 - table.begin()) - i)*(j2 - j);

#else
	for (u32 i1 = i + 1; i1<=max_z; i1++) {
		if (table[i1][j].empty())
			break;
		
		v1						= get_vertex_by_group_id(table[i1 - 1][j],group_id);
		if (table[i1][j].empty() || !connect(level_graph,v1,table[i1][j],group_id,1))
			break;

		for (u32 j1 = j + 1; j1 < j2; j1++) {
			v					= get_vertex_by_group_id(table[i1][j1 - 1],group_id);
			v1					= get_vertex_by_group_id(table[i1 - 1][j1],group_id);
			if (table[i1][j1].empty() || !connect(level_graph,v,v1,table[i1][j1],group_id)) {
				ok				= false;
				for (u32 J = j; J<j1; ++J)
					remove_mark	(table[i1][J],group_id);
				break;
			}
		}
		
		if (!ok)
			break;
	}
	
	CSector						sector;
	sector.min_vertex_id		= cell_vertex.m_vertex_id;
	sector.max_vertex_id		= get_vertex_by_group_id(table[i1 - 1][j2 - 1],group_id).m_vertex_id;
	sector_graph.add_vertex		(sector,group_id - 1);

	global_count				+= (i1 - i)*(j2 - j);
//	Msg							("%5d : [%5d][%5d] -> [%5d][%5d] = %5d (%7d : %7d)",group_id,i,j,i1,j2, (i1 - i)*(j2 - j),global_count,level_graph.header().vertex_count());
#endif
}

CCellVertex *cell_vertex	(VERTEX_VECTOR2 &table, const CLevelGraph &level_graph, u32 r, const CLevelGraph::CVertex	*vertex, u32 vertex_id)
{
	u32							x = vertex->position().x(r);
	u32							z = vertex->position().z(r);
	VERTEX_VECTOR				&v = table[z][x];
	VERTEX_VECTOR::iterator		I = v.begin();
	VERTEX_VECTOR::iterator		E = v.end();
	for ( ; I != E; ++I)
		if ((*I).m_vertex_id == vertex_id)
			return				(&*I);
	return						(0);
}

u64							s,f;

IC	void build_convex_hierarchy(const CLevelGraph &level_graph, CSectorGraph &sector_graph)
{
	sector_graph.clear			();
	// computing maximum and minimum x and z
	u32							max_z = 0;
	u32							max_x = 0;
	u32							min_z = u32(-1);
	u32							min_x = u32(-1);
	u32							n = level_graph.header().vertex_count();
	u32							r = level_graph.row_length();

	{
		u32									cur_x, cur_z;
		CLevelGraph::const_vertex_iterator	I = level_graph.begin();
		CLevelGraph::const_vertex_iterator	E = level_graph.end();
		for ( ; I != E; ++I) {
			const CLevelGraph::CPosition &position = (*I).position();
			cur_x					= position.x(r);
			cur_z					= position.z(r);
			if (cur_z > max_z)
				max_z				= cur_z;
			if (cur_x > max_x)
				max_x				= cur_x;
			if (cur_z < min_z)
				min_z				= cur_z;
			if (cur_x < min_x)
				min_x				= cur_x;
		}
	}
	f							= CPU::GetCycleCount();
	Msg							("MinMax time %f",CPU::cycles2seconds*float(f - s));

	// allocating memory
	VERTEX_VECTOR2				table;
	CROSS_VECTOR				cross;
	cross.assign				(n,0);
	{
		table.resize				(max_z - min_z + 1);
		u32							size_x = max_x - min_x + 1;
		VERTEX_VECTOR2::iterator	I = table.begin() + min_z;
		VERTEX_VECTOR2::iterator	E = table.end();
		for ( ; I != E; ++I)
			(*I).resize				(size_x);
	}
	f							= CPU::GetCycleCount();
	Msg							("Allocate time %f",CPU::cycles2seconds*float(f - s));

	{
		u32									cur_x, cur_z;
		CCellVertex							v(0,0);
		CLevelGraph::const_vertex_iterator	I = level_graph.begin();
		CLevelGraph::const_vertex_iterator	E = level_graph.end();
		for ( ; I != E; ++I, ++v.m_vertex_id) {
			const CLevelGraph::CPosition	&position = (*I).position();
			cur_x							= position.x(r);
			cur_z							= position.z(r);
			table[cur_z][cur_x].push_back	(v);
		}
	}
	f							= CPU::GetCycleCount();
	Msg							("Fill time %f",CPU::cycles2seconds*float(f - s));

	u32							group_id = 0;
	{
		VERTEX_VECTOR2::iterator	I = table.begin() + min_z, B = table.begin();
		VERTEX_VECTOR2::iterator	E = table.end();
		for ( ; I != E; ++I) {
			VERTEX_VECTOR1::iterator	i = (*I).begin() + min_x, b = (*I).begin();
			VERTEX_VECTOR1::iterator	e = (*I).end();
			for ( ; i != e; ++i) {
				VERTEX_VECTOR::iterator	II = (*i).begin();
				VERTEX_VECTOR::iterator	EE = (*i).end();
				for ( ; II != EE; ++II) {
					if ((*II).m_mark)
						continue;
					fill_mark		(level_graph,sector_graph,table,u32(I - B),u32(i - b),*II,group_id,min_z,max_z,min_x,max_x,cross);
				}
			}
		}
	}
	f							= CPU::GetCycleCount();
	Msg							("Recursive fill time %f",CPU::cycles2seconds*float(f - s));

#ifdef DEBUG
	{
		VERTEX_VECTOR2::iterator	I = table.begin() + min_z, B = table.begin();
		VERTEX_VECTOR2::iterator	E = table.end();
		for ( ; I != E; ++I) {
			VERTEX_VECTOR1::iterator	i = (*I).begin() + min_x, b = (*I).begin();
			VERTEX_VECTOR1::iterator	e = (*I).end();
			for ( ; i != e; ++i) {
				VERTEX_VECTOR::iterator	II = (*i).begin();
				VERTEX_VECTOR::iterator	EE = (*i).end();
				for ( ; II != EE; ++II) {
					VERIFY			((*II).m_mark);
				}
			}
		}
	}
#endif
	f								= CPU::GetCycleCount();
	Msg								("Check marks time %f",CPU::cycles2seconds*float(f - s));

	Msg								("Group ID : %d (%d vertices)",group_id,sector_graph.vertex_count());

	{
		CLevelGraph::const_vertex_iterator	i = level_graph.begin(), b = i;
		CLevelGraph::const_vertex_iterator	e = level_graph.end();
		for (u32 j=0; i != e; ++i, ++j) {
			CCellVertex					*current_cell = cross[i - b]; VERIFY(current_cell);//cell_vertex(table,level_graph,r,i,j);
			u32							current_mark = current_cell->m_mark - 1;
			CSectorGraph::CVertex		*sector_vertex = sector_graph.vertex(current_mark);
			CLevelGraph::const_iterator	I, E;
			level_graph.begin			(j,I,E);
			for ( ; I != E; ++I) {
				u32						vertex_id = level_graph.value(j,I);
				if (!level_graph.valid_vertex_id(vertex_id))
					continue;

				CCellVertex				*cell = cross[vertex_id];VERIFY(cell);//cell_vertex(table,level_graph,r,level_graph.vertex(vertex_id),vertex_id);
				u32						mark = cell->m_mark - 1;
				if (mark == current_mark)
					continue;

				if (sector_vertex->edge(mark))
					continue;

				sector_graph.add_edge	(current_mark,mark,1.f);
			}
		}
	}
	f								= CPU::GetCycleCount();
	Msg								("Fill edges time %f",CPU::cycles2seconds*float(f - s));

#ifdef DEBUG
	CSectorGraph::const_vertex_iterator	I = sector_graph.vertices().begin();
	CSectorGraph::const_vertex_iterator	E = sector_graph.vertices().end();
	for ( ; I != E; ++I) {
		VERIFY						(!(*I)->edges().empty());
	}
#endif
	f								= CPU::GetCycleCount();
	Msg								("Check edges time %f",CPU::cycles2seconds*float(f - s));

	Msg								("Sector Graph : %d vertices, %d edges",sector_graph.vertex_count(),sector_graph.edge_count());
}

#define TEST_COUNT 1

void test_hierarchy		(LPCSTR name)
{
	CLevelGraph					*level_graph = xr_new<CLevelGraph>(name);
	CSectorGraph				*sector_graph = xr_new<CSectorGraph>();
	SetPriorityClass			(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep						(1);
	
	s							= CPU::GetCycleCount();
//	for (u32 i=0; i<TEST_COUNT; ++i) 
	{
		build_convex_hierarchy	(*level_graph,*sector_graph);
		f						= CPU::GetCycleCount();
		Msg						("Destroy time %f",CPU::cycles2seconds*float(f - s));
	}
	f							= CPU::GetCycleCount();
	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass			(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	Msg							("Total time %f (%d tests : %f)",CPU::cycles2seconds*float(f - s),TEST_COUNT,CPU::cycles2microsec*float(f - s)/float(TEST_COUNT));
	
	xr_delete					(level_graph);
}