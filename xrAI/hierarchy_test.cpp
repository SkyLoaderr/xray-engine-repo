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

typedef CGraphAbstract<CSector,float,u32,u32> CSectorGraph;

IC	CCellVertex get_vertex_by_group_id(xr_vector<CCellVertex> vertices, u32 group_id)
{
	xr_vector<CCellVertex>::iterator	I = vertices.begin();
	xr_vector<CCellVertex>::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if ((*I).m_mark == group_id)
			return				(*I);

	NODEFAULT;
	return						(CCellVertex(u32(-1),0));
}

IC	bool connect(const CLevelGraph &level_graph, CCellVertex &vertex, xr_vector<CCellVertex> &vertices, u32 group_id, u32 link)
{
	xr_vector<CCellVertex>::iterator	I = vertices.begin();
	xr_vector<CCellVertex>::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if (!(*I).m_mark && (level_graph.vertex(vertex.m_vertex_id)->link(link) == (*I).m_vertex_id)) {
			(*I).m_mark			= group_id;
			vertex				= *I;
			return				(true);
		}
	return						(false);
}

IC	bool connect(const CLevelGraph &level_graph, CCellVertex &vertex1, CCellVertex &vertex2, xr_vector<CCellVertex> &vertices, u32 group_id)
{
	xr_vector<CCellVertex>::iterator	I = vertices.begin();
	xr_vector<CCellVertex>::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if (!(*I).m_mark && (level_graph.vertex(vertex1.m_vertex_id)->link(2) == (*I).m_vertex_id)) {
			if (level_graph.vertex(vertex2.m_vertex_id)->link(1) != (*I).m_vertex_id)
				return			(false);
			(*I).m_mark			= group_id;
			return				(true);
		}
	return						(false);
}

IC	void remove_mark(xr_vector<CCellVertex> &vertices, u32 group_id)
{
	xr_vector<CCellVertex>::iterator	I = vertices.begin();
	xr_vector<CCellVertex>::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if ((*I).m_mark == group_id) {
			(*I).m_mark			= 0;
			return;
		}
	NODEFAULT;
}

u32 global_count = 0;

void fill_mark(
	const CLevelGraph &level_graph, 
	CSectorGraph &sector_graph,
	xr_vector<CCellVertex> ***table, 
	u32 i, 
	u32 j, 
	CCellVertex &cell_vertex, 
	u32 &group_id, 
	u32 min_z, 
	u32 max_z, 
	u32 min_x, 
	u32 max_x
)
{
	++group_id;
	cell_vertex.m_mark			= group_id;
	CCellVertex					v = cell_vertex, v1;
	for (u32 j2 = j + 1; j2<=max_x; j2++)
		if (table[i][j2]->empty() || !connect(level_graph,v,*table[i][j2],group_id,2))
			break;

	bool						ok = true;
	for (u32 i1 = i + 1; i1<=max_z; i1++) {
		if (table[i1][j]->empty())
			break;
		
		v1						= get_vertex_by_group_id(*table[i1 - 1][j],group_id);
		if (table[i1][j]->empty() || !connect(level_graph,v1,*table[i1][j],group_id,1))
			break;

		for (u32 j1 = j + 1; j1 < j2; j1++) {
			v					= get_vertex_by_group_id(*table[i1][j1 - 1],group_id);
			v1					= get_vertex_by_group_id(*table[i1 - 1][j1],group_id);
			if (table[i1][j1]->empty() || !connect(level_graph,v,v1,*table[i1][j1],group_id)) {
				ok				= false;
				for (u32 J = j; J<j1; ++J)
					remove_mark	(*table[i1][J],group_id);
				break;
			}
		}
		
		if (!ok)
			break;
	}

	CSector						sector;
	sector.min_vertex_id		= cell_vertex.m_vertex_id;
	sector.max_vertex_id		= get_vertex_by_group_id(*table[i1 - 1][j2 - 1],group_id).m_vertex_id;
	sector_graph.add_vertex		(sector,group_id - 1);

	global_count				+= (i1 - i)*(j2 - j);
	Msg							("%4d : [%3d][%3d] -> [%3d][%3d] = %d (%6d : %6d)",group_id,i,j,i1,j2, (i1 - i)*(j2 - j),global_count,level_graph.header().vertex_count());
}

void build_convex_hierarchy(const CLevelGraph &level_graph, CSectorGraph &sector_graph)
{
	sector_graph.clear			();
	// computing maximum and minimum x and z
	u32							max_z = 0;
	u32							max_x = 0;
	u32							min_z = u32(-1);
	u32							min_x = u32(-1);
	u32							n = level_graph.header().vertex_count();
	u32							r = level_graph.row_length();
	for (u32 i = 0; i<n; ++i) {
		if (level_graph.vertex(i)->position().z(r) > max_z)
			max_z				= level_graph.vertex(i)->position().z(r);
		if (level_graph.vertex(i)->position().x(r) > max_x)
			max_x				= level_graph.vertex(i)->position().x(r);
		if (level_graph.vertex(i)->position().z(r) < min_z)
			min_z				= level_graph.vertex(i)->position().z(r);
		if (level_graph.vertex(i)->position().x(r) < min_x)
			min_x				= level_graph.vertex(i)->position().x(r);
	}

	// allocating memory
	xr_vector<CCellVertex>		***table = (xr_vector<CCellVertex>***)xr_malloc((max_z - min_z + 1)*sizeof(xr_vector<CCellVertex>**));
	for (u32 i=min_z; i<=max_z; ++i) {
		table[i]				= (xr_vector<CCellVertex>**)xr_malloc((max_x - min_x + 1)*sizeof(xr_vector<CCellVertex>*));
		for (u32 j=min_x; j<=max_x; ++j)
			table[i][j]			= xr_new<xr_vector<CCellVertex> >();
	}

	for (u32 i = 0; i<n; ++i)
		table[level_graph.vertex(i)->position().z(r)][level_graph.vertex(i)->position().x(r)]->push_back(CCellVertex(i,0));

	u32							group_id = 0;
	for (u32 i=min_z; i<=max_z; ++i)
		for (u32 j=min_x; j<=max_x; ++j)
			if (!table[i][j]->empty()) {
				xr_vector<CCellVertex>::iterator	I = table[i][j]->begin();
				xr_vector<CCellVertex>::iterator	E = table[i][j]->end();
				for ( ; I != E; ++I) {
					if ((*I).m_mark)
						continue;
					fill_mark	(level_graph,sector_graph,table,i,j,*I,group_id,min_z,max_z,min_x,max_x);
				}
			}

	for (u32 i=min_z; i<=max_z; ++i)
		for (u32 j=min_x; j<=max_x; ++j)
			if (!table[i][j]->empty()) {
				xr_vector<CCellVertex>::iterator	I = table[i][j]->begin();
				xr_vector<CCellVertex>::iterator	E = table[i][j]->end();
				for ( ; I != E; ++I)
					VERIFY	((*I).m_mark);
			}

//	// adding neighbours
//	vector<bool> marks;
//	for ( i=0; i<group_id; i++) {
//		marks.assign(group_id,false);
//		marks[i]				= true;
//		CSectorGraph::CVertex	*vertex = sector_graph.vertex(i);
//		u32						k, j;
////		u32						x1, x2, y1, y2;
////		level_graph.vertex(vertex->m_vertex_id)->position().x(r)
//		k						= tGraphVertex.x1 - 1;
//		for ( j=tGraphVertex.y1; j<=tGraphVertex.y2; j++)
//			if (!ucaCellularNetwork[k*(M + 1) + j] && !marks[dwaMask[k*M + j]])
//				vfAddNeighbour(tGraphVertex,k*M + j,tpaGraph,dwaMask,marks);
//		k = tGraphVertex.x2 + 1;
//		for ( j=tGraphVertex.y1; j<=tGraphVertex.y2; j++)
//			if (!ucaCellularNetwork[k*(M + 1) + j] && !marks[dwaMask[k*M + j]])
//				vfAddNeighbour(tGraphVertex,k*M + j,tpaGraph,dwaMask,marks);
//		k = tGraphVertex.y1 - 1;
//		for ( j=tGraphVertex.x1; j<=tGraphVertex.x2; j++)
//			if (!ucaCellularNetwork[j*(M + 1) + k] && !marks[dwaMask[j*M + k]])
//				vfAddNeighbour(tGraphVertex,j*M + k,tpaGraph,dwaMask,marks);
//		k = tGraphVertex.y2 + 1;
//		for ( j=tGraphVertex.x1; j<=tGraphVertex.x2; j++)
//			if (!ucaCellularNetwork[j*(M + 1) + k] && !marks[dwaMask[j*M + k]])
//				vfAddNeighbour(tGraphVertex,j*M + k,tpaGraph,dwaMask,marks);
//	}

	Msg							("Group ID : %d (%d vertices)",group_id,sector_graph.vertex_count());
	// freeing memory
	for (u32 i=min_z; i<=max_z; ++i) {
		for (u32 j=min_x; j<=max_x; ++j)
			xr_delete			(table[i][j]);
		xr_delete				(table[i]);
	}
	xr_delete					(table);
}

#define TEST_COUNT 1

void test_hierarchy		(LPCSTR name)
{
	CLevelGraph					*level_graph = xr_new<CLevelGraph>(name);
	CSectorGraph				*sector_graph = xr_new<CSectorGraph>();
	u64							s,f;
//	SetPriorityClass			(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep						(1);
	
	s							= CPU::GetCycleCount();
	for (u32 i=0; i<TEST_COUNT; ++i)
		build_convex_hierarchy	(*level_graph,*sector_graph);
	f							= CPU::GetCycleCount();
	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass			(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	Msg							("Total time %f (%d tests : %f)",CPU::cycles2seconds*float(f - s),TEST_COUNT,CPU::cycles2microsec*float(f - s)/float(TEST_COUNT));
	
	xr_delete					(level_graph);
}