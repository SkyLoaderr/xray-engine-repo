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

struct CSector : public IPureSerializeObject<IReader,IWriter> {
	u32				min_vertex_id;
	u32				max_vertex_id;

	IC				CSector		() {}
	
	virtual void	save		(IWriter &stream)
	{
		stream.w	(&min_vertex_id,sizeof(min_vertex_id));
		stream.w	(&max_vertex_id,sizeof(max_vertex_id));
	}
	
	virtual void	load		(IReader &stream)
	{
		stream.r	(&min_vertex_id,sizeof(min_vertex_id));
		stream.r	(&max_vertex_id,sizeof(max_vertex_id));
	}

	IC		bool	operator==	(const CSector &obj) const
	{
		return		(
			(min_vertex_id == obj.min_vertex_id) &&
			(max_vertex_id == obj.max_vertex_id)
		);
	}
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
//	Msg							("%5d : [%5d][%5d] -> [%5d][%5d] = %5d (%7d : %7d)",group_id,i,j,i1,j2, (i1 - i)*(j2 - j),global_count,level_graph.header().vertex_count());
}

CCellVertex *cell_vertex	(xr_vector<CCellVertex>	***table, const CLevelGraph &level_graph, u32 vertex_id)
{
	const CLevelGraph::CVertex			*vertex = level_graph.vertex(vertex_id);
	u32									x = vertex->position().x(level_graph.row_length());
	u32									z = vertex->position().z(level_graph.row_length());
	xr_vector<CCellVertex>::iterator	I = table[z][x]->begin();
	xr_vector<CCellVertex>::iterator	E = table[z][x]->end();
	for ( ; I != E; ++I)
		if ((*I).m_vertex_id == vertex_id)
			return						(&*I);
	return								(0);
}

CCellVertex *cell_vertex	(xr_vector<CCellVertex>	***table, const CLevelGraph &level_graph, const CLevelGraph::CVertex *vertex)
{
	return	(cell_vertex(table,level_graph,level_graph.vertex_id(vertex)));
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

	u64							s,f;
	s							= CPU::GetCycleCount();
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
	f							= CPU::GetCycleCount();
	Msg							("MinMax time %f",CPU::cycles2seconds*float(f - s));

	// allocating memory
	xr_vector<CCellVertex>		***table = (xr_vector<CCellVertex>***)xr_malloc((max_z - min_z + 1)*sizeof(xr_vector<CCellVertex>**));
	for (u32 i=min_z; i<=max_z; ++i) {
		table[i]				= (xr_vector<CCellVertex>**)xr_malloc((max_x - min_x + 1)*sizeof(xr_vector<CCellVertex>*));
		for (u32 j=min_x; j<=max_x; ++j)
			table[i][j]			= xr_new<xr_vector<CCellVertex> >();
	}
	f							= CPU::GetCycleCount();
	Msg							("Allocate time %f",CPU::cycles2seconds*float(f - s));

	for (u32 i = 0; i<n; ++i)
		table[level_graph.vertex(i)->position().z(r)][level_graph.vertex(i)->position().x(r)]->push_back(CCellVertex(i,0));
	f							= CPU::GetCycleCount();
	Msg							("Fill time %f",CPU::cycles2seconds*float(f - s));

	u32							group_id = 0;
	for (u32 i=min_z; i<=max_z; ++i) {
		for (u32 j=min_x; j<=max_x; ++j) {
			if (!table[i][j]->empty()) {
				xr_vector<CCellVertex>::iterator	I = table[i][j]->begin();
				xr_vector<CCellVertex>::iterator	E = table[i][j]->end();
				for ( ; I != E; ++I) {
					if ((*I).m_mark)
						continue;
					fill_mark		(level_graph,sector_graph,table,i,j,*I,group_id,min_z,max_z,min_x,max_x);
				}
			}
		}
	}
	f							= CPU::GetCycleCount();
	Msg							("Recursive fill time %f",CPU::cycles2seconds*float(f - s));

#ifdef DEBUG
	for (u32 i=min_z; i<=max_z; ++i) {
		for (u32 j=min_x; j<=max_x; ++j) {
			if (!table[i][j]->empty()) {
				xr_vector<CCellVertex>::iterator	I = table[i][j]->begin();
				xr_vector<CCellVertex>::iterator	E = table[i][j]->end();
				for ( ; I != E; ++I)
					VERIFY			((*I).m_mark);
			}
		}
	}
#endif
	f								= CPU::GetCycleCount();
	Msg								("Check marks time %f",CPU::cycles2seconds*float(f - s));

	Msg								("Group ID : %d (%d vertices)",group_id,sector_graph.vertex_count());

	for (u32 i=0; i<n; ++i) {
		CCellVertex					*current_cell = cell_vertex(table,level_graph,i);
		CSectorGraph::CVertex		*sector_vertex = sector_graph.vertex(current_cell->m_mark-1);
		CLevelGraph::const_iterator	I, E;
		level_graph.begin			(i,I,E);
		for ( ; I != E; ++I) {
			u32						vertex_id = level_graph.value(i,I);
			if (!level_graph.valid_vertex_id(vertex_id))
				continue;

			CCellVertex				*cell = cell_vertex(table,level_graph,vertex_id);
			if (cell->m_mark == current_cell->m_mark)
				continue;

			if (sector_vertex->edge(cell->m_mark-1))
				continue;

			sector_graph.add_edge	(current_cell->m_mark-1,cell->m_mark-1,1.f);

//			Msg						("Adding edge %5d -> %5d",current_cell->m_mark,cell->m_mark);
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

	// freeing memory
	for (u32 i=min_z; i<=max_z; ++i) {
		for (u32 j=min_x; j<=max_x; ++j)
			xr_delete				(table[i][j]);
		xr_delete					(table[i]);
	}
	xr_delete						(table);
	
	f								= CPU::GetCycleCount();
	Msg								("Destroy time %f",CPU::cycles2seconds*float(f - s));
}

#define TEST_COUNT 1

void test_hierarchy		(LPCSTR name)
{
	CLevelGraph					*level_graph = xr_new<CLevelGraph>(name);
	CSectorGraph				*sector_graph = xr_new<CSectorGraph>();
	u64							s,f;
#ifndef _DEBUG
	SetPriorityClass			(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
#endif
	Sleep						(1);
	
	s							= CPU::GetCycleCount();
	for (u32 i=0; i<TEST_COUNT; ++i)
		build_convex_hierarchy	(*level_graph,*sector_graph);
	f							= CPU::GetCycleCount();
	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass			(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	Msg							("Total time %f (%d tests : %f)",CPU::cycles2seconds*float(f - s),TEST_COUNT,CPU::cycles2microsec*float(f - s)/float(TEST_COUNT));
	
	CMemoryWriter				stream;
	save_data					(sector_graph,stream);
	stream.save_to				("x:\\sector_graph.dat");

	IReader						*reader = FS.r_open("x:\\sector_graph.dat");
	CSectorGraph				*test;
	load_data					(test,*reader);

	VERIFY						(equal(sector_graph,test));
	
	xr_delete					(level_graph);
	xr_delete					(sector_graph);
	xr_delete					(test);
}
