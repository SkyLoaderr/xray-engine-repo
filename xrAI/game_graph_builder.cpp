////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_builder.cpp
//	Created 	: 14.12.2005
//  Modified 	: 14.12.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph builder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_graph_builder.h"
#include "level_graph.h"
#include "graph_abstract.h"
#include "xrMessages.h"
#include "xrServer_Objects_ALife.h"
#include "factory_api.h"
#include "game_level_cross_table.h"
#include "xrCrossTable.h"
#include "guid_generator.h"

CGameGraphBuilder::CGameGraphBuilder		()
{
	m_level_graph			= 0;
	m_graph					= 0;
	m_cross_table			= 0;
}

CGameGraphBuilder::~CGameGraphBuilder		()
{
	Msg						("Freeing resources");

	xr_delete				(m_level_graph);
	xr_delete				(m_graph);
	xr_delete				(m_cross_table);
}

void CGameGraphBuilder::create_graph		(const float &start, const float &amount)
{
	Progress				(start);

	VERIFY					(!m_graph);
	m_graph					= xr_new<graph_type>();

	m_graph_guid			= generate_guid();

	Progress				(start + amount);
}

void CGameGraphBuilder::load_level_graph	(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Loading AI map");
	
	VERIFY					(!m_level_graph);
	m_level_graph			= xr_new<CLevelGraph>(*m_level_name);
	
	Msg						("%d nodes loaded",level_graph().header().vertex_count());
	
	Progress				(start + amount);
}

void CGameGraphBuilder::load_graph_point	(NET_Packet &net_packet)
{
	string256				section_id;
	u16						id;
	net_packet.r_begin		(id);
	R_ASSERT				(M_SPAWN == id);
	net_packet.r_stringZ	(section_id);

//	if (xr_strcmp("graph_point",section_id))
//		return;

	CSE_Abstract			*entity = F_entity_Create(section_id);
	if (!entity) {
		Msg					("Cannot create entity from section %s, skipping",section_id);
		return;
	}

	CSE_ALifeGraphPoint		*graph_point = smart_cast<CSE_ALifeGraphPoint*>(entity);
	if (!graph_point) {
		F_entity_Destroy	(entity);
		return;
	}

	entity->Spawn_Read		(net_packet);

	vertex_type				vertex;
	vertex.tLocalPoint		= graph_point->o_Position;
	// check for duplicate graph point positions
	{
		graph_type::const_vertex_iterator	I = graph().vertices().begin();
		graph_type::const_vertex_iterator	E = graph().vertices().begin();
		for ( ; I != E; ++I) {
			if ((*I).second->data().tLocalPoint.distance_to_sqr(vertex.tLocalPoint) < EPS_L) {
#pragma todo("Dima to Dima : add reason of skipping graph point here")
				return;
			}
		}
	}

	vertex.tGlobalPoint		= graph_point->o_Position;
	vertex.tNodeID			= level_graph().vertex_id(vertex.tLocalPoint);
	if (!level_graph().valid_vertex_id(vertex.tNodeID)) {
#pragma todo("Dima to Dima : add reason of skipping graph point here")
		return;
	}

	// check for multiple vertices on a single node id
	{
		Fvector				vertex_position = level_graph().vertex_position(vertex.tNodeID);
		float				distance_sqr = vertex.tLocalPoint.distance_to_sqr(vertex_position);

		graph_type::const_vertex_iterator	I = graph().vertices().begin();
		graph_type::const_vertex_iterator	E = graph().vertices().begin();
		for ( ; I != E; ++I) {
			if ((*I).second->data().tNodeID == vertex.tNodeID) {
#pragma todo("Dima to Dima : add reason of skipping graph point here")
				return;
			}

			if ((*I).second->data().tLocalPoint.distance_to_sqr(vertex_position) < distance_sqr) {
#pragma todo("Dima to Dima : add reason of skipping graph point here")
				return;
			}
		}
	}

	vertex.tNeighbourCount	= 0;
	Memory.mem_copy			(vertex.tVertexTypes,graph_point->m_tLocations,GameGraph::LOCATION_TYPE_COUNT*sizeof(GameGraph::_LOCATION_ID));
	vertex.tLevelID			= 0;
	vertex.tDeathPointCount = 0;
	vertex.dwPointOffset	= 0;

	graph().add_vertex		(vertex,graph().vertices().size());

	F_entity_Destroy		(entity);
}

void CGameGraphBuilder::load_graph_points	(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Loading graph points");

	string256				spawn_file_name;
	strconcat				(spawn_file_name,*m_level_name,"level.spawn");
	IReader					*reader = FS.r_open(spawn_file_name);
	u32						id;
	NET_Packet				net_packet;
	for	(
			IReader *chunk = reader->open_chunk_iterator(id);
			chunk;
			chunk = reader->open_chunk_iterator(id,chunk)
		)
	{
		net_packet.B.count	= chunk->length();
		chunk->r			(net_packet.B.data,net_packet.B.count);
		load_graph_point	(net_packet);
	}
	
	FS.r_close				(reader);

	Msg						("%d graph points loaded",graph().vertices().size());

	Progress				(start + amount);
}

IC	bool sort_predicate(const std::pair<u32,u32> &first, const std::pair<u32,u32> &second)
{
	return					(first.first < second.first);
}

void CGameGraphBuilder::remove_incoherent_points	(const float &start, const float &amount)
{
	Progress				(start);

#if 1
	graph_type				*test_graph = xr_new<graph_type>();
	
	typedef std::pair<u32,u32>	pair;
	xr_vector<pair>			pairs;
	for (u32 i=0; i<graph().header().vertex_count(); ++i)
		pairs.push_back		(std::make_pair(graph().vertex(i)->data().tNodeID,i));

	std::sort				(pairs.begin(),pairs.end(),sort_predicate);

	xr_vector<pair>::iterator	I = pairs.begin(), B = I;
	xr_vector<pair>::iterator	E = pairs.end();
	for ( ; I != E; ++I)
		test_graph->add_vertex(graph().vertex((*I).second)->data(),u32(I - B));

	xr_delete				(m_graph);
	m_graph					= test_graph;
#endif

	Progress				(start + amount);
}

void CGameGraphBuilder::mark_vertices		(u32 level_vertex_id)
{
	CLevelGraph::const_iterator			I, E;
	m_mark_stack.reserve				(8192);
	m_mark_stack.push_back				(level_vertex_id);

	for ( ; !m_mark_stack.empty(); ) {
		level_vertex_id					= m_mark_stack.back();
		m_mark_stack.resize				(m_mark_stack.size() - 1);
		CLevelGraph::CVertex			*node = level_graph().vertex(level_vertex_id);
		level_graph().begin				(level_vertex_id,I,E);
		m_marks[level_vertex_id]		= true;
		for ( ; I != E; ++I) {
			u32							next_level_vertex_id = node->link(I);
			if (!level_graph().valid_vertex_id(next_level_vertex_id))
				continue;
			
			if (!m_marks[next_level_vertex_id])
				m_mark_stack.push_back	(next_level_vertex_id);
		}
	}
}

void CGameGraphBuilder::fill_marks			(const float &start, const float &amount)
{
	Progress							(start);

	m_marks.assign						(level_graph().header().vertex_count(),false);
	graph_type::const_vertex_iterator	I = graph().vertices().begin();
	graph_type::const_vertex_iterator	E = graph().vertices().end();
	for ( ; I != E; ++I)
		mark_vertices					((*I).second->data().level_vertex_id());
	m_marks.flip						();
	
	Progress							(start + amount);
}

void CGameGraphBuilder::fill_distances		(const float &start, const float &amount)
{
	Progress							(start);

	m_distances.resize					(graph().vertices().size());
	{
		DISTANCES::iterator				I = m_distances.begin();
		DISTANCES::iterator				E = m_distances.end();
		for ( ; I != E; I++) {
			(*I).resize					(level_graph().header().vertex_count());
			xr_vector<u32>::iterator	i = (*I).begin();
			xr_vector<u32>::iterator	e = (*I).end();
			for ( ; i != e; i++)
				*i						= u32(-1);
		}
	}

	Progress							(start + amount);
}

void CGameGraphBuilder::recursive_update	(const u32 &index, const float &start, const float &amount)
{
	Progress					(start);

	u32							level_vertex_id = graph().vertex(index)->data().level_vertex_id();
	xr_vector<u32>				&distances = m_distances[index];

	m_current_fringe.reserve	(distances.size());
	m_next_fringe.reserve		(distances.size());
	distances.assign			(distances.size(),u32(-1));
	m_current_fringe.push_back	(level_vertex_id);
	u32							curr_dist = 0, total_count = 0;

	u32							vertex_count = graph().header().vertex_count();
	
	Progress					(start);
	for ( ; !m_current_fringe.empty(); ) {
		xr_vector<u32>::iterator	I = m_current_fringe.begin();
		xr_vector<u32>::iterator	E = m_current_fringe.end();
		for ( ; I != E; ++I) {
			distances[*I]					= curr_dist;
			CLevelGraph::const_iterator		i, e;
			CLevelGraph::CVertex			*node = level_graph().vertex(*I);
			level_graph().begin					(*I,i,e);
			for ( ; i != e; ++i) {
				u32							dwNexNodeID = node->link(i);
				if (!level_graph().valid_vertex_id(dwNexNodeID) || m_marks[dwNexNodeID])
					continue;
				if (distances[dwNexNodeID] > curr_dist) {
					m_next_fringe.push_back(dwNexNodeID);
					m_marks[dwNexNodeID] = true;
				}
			}
		}
		I = m_current_fringe.begin();
		E = m_current_fringe.end();
		for ( ; I != E; ++I)
			m_marks[*I] = false;

		total_count			+= m_current_fringe.size();
		m_current_fringe	= m_next_fringe;
		m_next_fringe.clear	();
		++curr_dist;

		Progress			(start + amount*(float(total_count)/(float(vertex_count)*float(level_graph().header().vertex_count()))));
	}

	Progress				(start + amount);
}

void CGameGraphBuilder::iterate_distances	(const float &start, const float &amount)
{
	Progress							(start);
	for (int i=0, n=(int)graph().vertices().size(); i<n; ++i) {
		if (i) {
			for (int k=0, kn=(int)level_graph().header().vertex_count(); k<kn; ++k)
				m_distances[i][k]		= m_distances[i - 1][k];
		}

		recursive_update				(i,start + amount*float(i)/float(n),amount*1.f/float(n));
	}
	Progress							(start + amount);
}

void CGameGraphBuilder::save_cross_table	(const float &start, const float &amount)
{
	Progress							(start);

	Msg									("Saving cross table");

	CMemoryWriter						tMemoryStream;
	CGameLevelCrossTable::CHeader		tCrossTableHeader;
	
	tCrossTableHeader.dwVersion			= XRAI_CURRENT_VERSION;
	tCrossTableHeader.dwNodeCount		= level_graph().header().vertex_count();
	tCrossTableHeader.dwGraphPointCount = graph().header().vertex_count();
	tCrossTableHeader.m_level_guid		= level_graph().header().guid();
	tCrossTableHeader.m_game_guid		= m_graph_guid;
	
	tMemoryStream.open_chunk			(CROSS_TABLE_CHUNK_VERSION);
	tMemoryStream.w						(&tCrossTableHeader,sizeof(tCrossTableHeader));
	tMemoryStream.close_chunk			();
	
	tMemoryStream.open_chunk			(CROSS_TABLE_CHUNK_DATA);
	{
		for (int i=0, n=level_graph().header().vertex_count(); i<n; i++) {
			DISTANCES::iterator			I = m_distances.begin(), B = I;
			DISTANCES::iterator			E = m_distances.end();
			CGameLevelCrossTable::CCell tCrossTableCell;
			tCrossTableCell.fDistance	= flt_max;
			tCrossTableCell.tGraphIndex = u16(-1);
			for ( ; I != E; I++)
				if (float((*I)[i])*level_graph().header().cell_size() < tCrossTableCell.fDistance) {
					tCrossTableCell.fDistance	= float((*I)[i])*level_graph().header().cell_size();
					tCrossTableCell.tGraphIndex = GameGraph::_GRAPH_ID(I - B);
				}
			
			for (int j=0, n=graph().header().vertex_count(); j<n; j++)
				if ((graph().vertex(j)->data().level_vertex_id() == (u32)i) && (tCrossTableCell.tGraphIndex != j)) {
					Msg("! Warning : graph points are too close, therefore cross table is automatically validated");
					Msg("%d : [%f][%f][%f] %d[%f] -> %d[%f]",i,VPUSH(graph().vertex(j)->data().level_point()),tCrossTableCell.tGraphIndex,tCrossTableCell.fDistance,j,m_distances[j][i]);
					tCrossTableCell.fDistance	= float(m_distances[j][i])*level_graph().header().cell_size();
					tCrossTableCell.tGraphIndex = (GameGraph::_GRAPH_ID)j;
				}
			tMemoryStream.w(&tCrossTableCell,sizeof(tCrossTableCell));
		}
	}
	tMemoryStream.close_chunk();
	
	string_path				file_name;
	strconcat				(file_name,*m_level_name,CROSS_TABLE_NAME_RAW);
	tMemoryStream.save_to	(file_name);

	m_marks.clear			();
	m_mark_stack.clear		();
	m_distances.clear		();
	m_current_fringe.clear	();
	m_next_fringe.clear		();

	Progress				(start + amount);
}

void CGameGraphBuilder::build_cross_table	(const float &start, const float &amount)
{
	Progress				(start);
	
	Msg						("Building cross table");

	fill_marks				(start + .00f*amount,.01f*amount);
	fill_distances			(start + .01f*amount,.01f*amount);
	iterate_distances		(start + .02f*amount,.96f*amount);
	save_cross_table		(start + .98f*amount,.01f*amount);
	load_cross_table		(start + .99f*amount,.01f*amount);
	
	Progress				(start + amount);
}

void CGameGraphBuilder::load_cross_table	(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Loading cross table");

	string_path				file_name;
	strconcat				(file_name,*m_level_name,CROSS_TABLE_NAME_RAW);

	VERIFY					(!m_cross_table);
	m_cross_table			= xr_new<CGameLevelCrossTable>(file_name);

	Progress				(start + amount);
}

void CGameGraphBuilder::build_neighbours	(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Building neighbours");
	


	Progress				(start + amount);
}

void CGameGraphBuilder::generate_edges		(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Generating edges");
	

	Progress				(start + amount);
}

void CGameGraphBuilder::optimize_graph		(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Optimizing graph");


	Progress				(start + amount);
}

void CGameGraphBuilder::connectivity_check	(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Checking graph connectivity");


	Progress				(start + amount);
}

void CGameGraphBuilder::save_graph			(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Saving graph");


	Progress				(start + amount);
}

void CGameGraphBuilder::build_graph			(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Building graph");

	build_neighbours		(start + .00f*amount, amount*.20f);
	generate_edges			(start + .20f*amount, amount*.20f);
	optimize_graph			(start + .40f*amount, amount*.20f);
	connectivity_check		(start + .60f*amount, amount*.20f);
	save_graph				(start + .80f*amount, amount*.20f);

	Progress				(start + amount);
}

void CGameGraphBuilder::build_graph			(LPCSTR level_name)
{
	Phase					("Building level graph");
	Msg						("level \"%s\"",level_name);

	m_level_name			= level_name;
	
	create_graph			(0.000f,0.001f);
	load_level_graph		(0.001f,0.001f);
	load_graph_points		(0.002f,0.001f);
	remove_incoherent_points(0.003f,0.001f);
	build_cross_table		(0.004f,0.995f);
	build_graph				(0.999f,0.001f);

	Msg						("Level graph is generated successfully");
}
