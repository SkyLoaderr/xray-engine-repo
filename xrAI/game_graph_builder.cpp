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
#include "graph_engine.h"

#define CROSS_TABLE_NAME_TEST	"level.gct.test"
#define GAME_LEVEL_GRAPH_TEST	"level.graph.test"

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

template <typename T>
IC	bool sort_predicate(const T &first, const T &second)
{
	return					(first.first < second.first);
}

void CGameGraphBuilder::remove_incoherent_points	(const float &start, const float &amount)
{
	Progress				(start);

#if 1
	graph_type				*test_graph = xr_new<graph_type>();
	
	xr_vector<PAIR>			pairs;
	for (u32 i=0; i<graph().header().vertex_count(); ++i)
		pairs.push_back		(std::make_pair(graph().vertex(i)->data().tNodeID,i));

	std::sort				(pairs.begin(),pairs.end(),sort_predicate<PAIR>);

	xr_vector<PAIR>::iterator	I = pairs.begin(), B = I;
	xr_vector<PAIR>::iterator	E = pairs.end();
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

void CGameGraphBuilder::recursive_update	(const u32 &game_vertex_id, const float &start, const float &amount)
{
	Progress					(start);

	u32							level_vertex_id = graph().vertex(game_vertex_id)->data().level_vertex_id();
	xr_vector<u32>				&distances = m_distances[game_vertex_id];

	m_current_fringe.reserve	(distances.size());
	m_next_fringe.reserve		(distances.size());
	distances.assign			(distances.size(),u32(-1));
	m_current_fringe.push_back	(level_vertex_id);
	u32							curr_dist = 0;
	u32							total_count = 0;

	u32							vertex_count = graph().header().vertex_count();

	float						amount_i = 
		amount
		/
		(
			float(vertex_count)*
			float(level_graph().header().vertex_count())
		);

	Progress					(start);
	for ( ; !m_current_fringe.empty(); ) {
		xr_vector<u32>::iterator			I = m_current_fringe.begin();
		xr_vector<u32>::iterator			E = m_current_fringe.end();
		for ( ; I != E; ++I) {
			u32								*result = &m_results[*I];
			if (curr_dist < m_distances[*result][*I])
				*result						= game_vertex_id;

			distances[*I]					= curr_dist;
			CLevelGraph::const_iterator		i, e;
			CLevelGraph::CVertex			*node = level_graph().vertex(*I);
			level_graph().begin				(*I,i,e);
			for ( ; i != e; ++i) {
				u32							dwNexNodeID = node->link(i);
				if (!level_graph().valid_vertex_id(dwNexNodeID))
					continue;
				
				if (m_marks[dwNexNodeID])
					continue;
				
				if (distances[dwNexNodeID] <= curr_dist)
					continue;

				m_next_fringe.push_back		(dwNexNodeID);
				m_marks[dwNexNodeID]		= true;
			}
		}
		
		I = m_current_fringe.begin();
		E = m_current_fringe.end();
		for ( ; I != E; ++I)
			m_marks[*I]		= false;

		total_count			+= m_current_fringe.size();
		m_current_fringe	= m_next_fringe;
		m_next_fringe.clear	();
		++curr_dist;

		Progress			(start + amount_i*float(total_count));
	}

	Progress				(start + amount);
}

void CGameGraphBuilder::iterate_distances	(const float &start, const float &amount)
{
	Progress							(start);

	m_results.assign					(level_graph().header().vertex_count(),0);
	
	float								amount_i = amount/float(graph().vertices().size());
	for (int i=0, n=(int)graph().vertices().size(); i<n; ++i) {
		if (i) {
			for (int k=0, kn=(int)level_graph().header().vertex_count(); k<kn; ++k)
				m_distances[i][k]		= m_distances[i - 1][k];
		}

		recursive_update				(i,start + amount_i*float(i),amount_i);
	}

	Progress							(start + amount);
}

void CGameGraphBuilder::save_cross_table	(const float &start, const float &amount)
{
	Progress							(start);

	Msg									("Saving cross table");

	CTimer								timer;
	timer.Start							();

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

#if 0
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
#else
	for (int i=0, n=level_graph().header().vertex_count(); i<n; i++) {
		CGameLevelCrossTable::CCell	tCrossTableCell;

		/**
		DISTANCES::iterator			I = m_distances.begin(), B = I;
		DISTANCES::iterator			E = m_distances.end();
		tCrossTableCell.fDistance	= flt_max;
		tCrossTableCell.tGraphIndex = u16(-1);
		for ( ; I != E; I++)
			if (float((*I)[i])*level_graph().header().cell_size() < tCrossTableCell.fDistance) {
				tCrossTableCell.fDistance	= float((*I)[i])*level_graph().header().cell_size();
				tCrossTableCell.tGraphIndex = GameGraph::_GRAPH_ID(I - B);
			}

		if (m_results[i] != tCrossTableCell.tGraphIndex) {
			tCrossTableCell.fDistance	= flt_max;
			tCrossTableCell.tGraphIndex = u16(-1);
			for ( ; I != E; I++)
				if (float((*I)[i])*level_graph().header().cell_size() < tCrossTableCell.fDistance) {
					tCrossTableCell.fDistance	= float((*I)[i])*level_graph().header().cell_size();
					tCrossTableCell.tGraphIndex = GameGraph::_GRAPH_ID(I - B);
				}
		}
		/**/
		tCrossTableCell.tGraphIndex = (GameGraph::_GRAPH_ID)m_results[i];
		VERIFY						(graph().header().vertex_count() > tCrossTableCell.tGraphIndex);
		tCrossTableCell.fDistance	= float(m_distances[tCrossTableCell.tGraphIndex][i])*level_graph().header().cell_size();
		tMemoryStream.w				(&tCrossTableCell,sizeof(tCrossTableCell));
	}
#endif

	tMemoryStream.close_chunk();
	
//	Msg						("CT:SAVE : %f",timer.GetElapsed_sec());
//	Msg						("Flushing cross table");

	string_path				file_name;
	strconcat				(file_name,*m_level_name,CROSS_TABLE_NAME_TEST);
	tMemoryStream.save_to	(file_name);
//	Msg						("CT:SAVE : %f",timer.GetElapsed_sec());

//	Msg						("Freiing cross table resources");

	m_marks.clear			();
	m_mark_stack.clear		();
	m_distances.clear		();
	m_current_fringe.clear	();
	m_next_fringe.clear		();

//	Msg						("CT:SAVE : %f",timer.GetElapsed_sec());
	Progress				(start + amount);
}

void CGameGraphBuilder::build_cross_table	(const float &start, const float &amount)
{
	Progress				(start);
	
	Msg						("Building cross table");

	CTimer					timer;
	timer.Start				();

	fill_marks				(start + 0.000000f*amount,0.003125f*amount);
//	Msg						("CT : %f",timer.GetElapsed_sec());
	fill_distances			(start + 0.003125f*amount,0.034035f*amount);
//	Msg						("CT : %f",timer.GetElapsed_sec());
	iterate_distances		(start + 0.037159f*amount,0.955690f*amount);
//	Msg						("CT : %f",timer.GetElapsed_sec());
	save_cross_table		(start + 0.992850f*amount,0.007148f*amount);
//	Msg						("CT : %f",timer.GetElapsed_sec());
	load_cross_table		(start + 0.999998f*amount,0.000002f*amount);
//	Msg						("CT : %f",timer.GetElapsed_sec());
	
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

void CGameGraphBuilder::fill_neighbours		(const u32 &game_vertex_id)
{
	m_marks.assign						(level_graph().header().vertex_count(),false);
	m_current_fringe.clear				();

	u32									level_vertex_id = graph().vertex(game_vertex_id)->data().level_vertex_id();

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
			
			if (m_marks[next_level_vertex_id])
				continue;

			GameGraph::_GRAPH_ID		next_game_vertex_id = cross().vertex(next_level_vertex_id).game_vertex_id();
			if (next_game_vertex_id != (GameGraph::_GRAPH_ID)game_vertex_id) {
				if	(
						std::find(
							m_current_fringe.begin(),
							m_current_fringe.end(),
							next_game_vertex_id
						)
						==
						m_current_fringe.end()
					)
					m_current_fringe.push_back	(next_game_vertex_id);
				continue;
			}

			m_mark_stack.push_back		(next_level_vertex_id);
		}
	}
}

float CGameGraphBuilder::path_distance		(const u32 &game_vertex_id0, const u32 &game_vertex_id1)
{
//	return					(graph().vertex(game_vertex_id0)->data().level_point().distance_to(graph().vertex(game_vertex_id1)->data().level_point()));

	VERIFY					(m_graph_engine);

	graph_type::CVertex		&vertex0 = *graph().vertex(game_vertex_id0);
	graph_type::CVertex		&vertex1 = *graph().vertex(game_vertex_id1);

	typedef GraphEngineSpace::CStraightLineParams	CStraightLineParams;
	CStraightLineParams		parameters(vertex0.data().level_point(),vertex1.data().level_point());

//	float					pure_distance = vertex0.data().level_point().distance_to_xz(vertex1.data().level_point());
	float					pure_distance = vertex0.data().level_point().distance_to(vertex1.data().level_point());
	VERIFY					(pure_distance < parameters.max_range);

	u32						level_vertex_id = level_graph().check_position_in_direction(vertex0.data().level_vertex_id(),vertex0.data().level_point(),vertex1.data().level_point());
	if (level_graph().valid_vertex_id(level_vertex_id))
		return				(pure_distance);

	bool					successfull = 
		m_graph_engine->search(
			level_graph(),
			vertex0.data().level_vertex_id(),
			vertex1.data().level_vertex_id(),
			&m_path,
			parameters
		);

	if (successfull)
		return				(parameters.m_distance);

	Msg						("Cannot build path from [%d] to [%d]",game_vertex_id0,game_vertex_id1);
	Msg						("Cannot build path from [%f][%f][%f] to [%f][%f][%f]",VPUSH(vertex0.data().level_point()),VPUSH(vertex1.data().level_point()));
	R_ASSERT2				(false,"Cannot build path, check AI map");
	return					(flt_max);
}

void CGameGraphBuilder::generate_edges		(const u32 &game_vertex_id)
{
	graph_type::CVertex		*vertex = graph().vertex(game_vertex_id);

	xr_vector<u32>::const_iterator	I = m_current_fringe.begin();
	xr_vector<u32>::const_iterator	E = m_current_fringe.end();
	for ( ; I != E; ++I) {
		VERIFY				(!vertex->edge(*I));
		float				distance = path_distance(game_vertex_id,*I);
		graph().add_edge	(game_vertex_id,*I,distance);
	}
}

void CGameGraphBuilder::generate_edges		(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Generating edges");
	
	graph_type::const_vertex_iterator	I = graph().vertices().begin();
	graph_type::const_vertex_iterator	E = graph().vertices().end();
	for ( ; I != E; ++I) {
		fill_neighbours		((*I).second->vertex_id());
		generate_edges		((*I).second->vertex_id());
	}

	Msg						("%d edges built",graph().edge_count());

	Progress				(start + amount);
}

void CGameGraphBuilder::connectivity_check	(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Checking graph connectivity");


	Progress				(start + amount);
}

void CGameGraphBuilder::create_tripples		(const float &start, const float &amount)
{
	graph_type::const_vertex_iterator	I = graph().vertices().begin();
	graph_type::const_vertex_iterator	E = graph().vertices().end();
	for ( ; I != E; ++I) {
		graph_type::const_iterator	i = (*I).second->edges().begin();
		graph_type::const_iterator	e = (*I).second->edges().end();
		for ( ; i != e; ++i) {
			if ((*i).vertex_id() < (*I).first)
				continue;

			const graph_type::CEdge	*edge = graph().vertex((*i).vertex_id())->edge((*I).first);

			m_tripples.push_back	(
				std::make_pair(
					_min(
						(*i).weight(),
						edge ? edge->weight() : (*i).weight()
					),
					std::make_pair(
						(*I).first,
						(*i).vertex_id()
					)
				)
			);
		}
	}

	std::sort				(m_tripples.begin(),m_tripples.end(),sort_predicate<TRIPPLE>);
	std::reverse			(m_tripples.begin(),m_tripples.end());
}

void CGameGraphBuilder::process_tripple		(const TRIPPLE &tripple)
{
	const graph_type::CVertex	&vertex0 = *graph().vertex(tripple.second.first);
	const graph_type::CVertex	&vertex1 = *graph().vertex(tripple.second.second);

	graph_type::const_iterator	I = vertex0.edges().begin();
	graph_type::const_iterator	E = vertex0.edges().end();
	for ( ; I != E; ++I) {
		if ((*I).vertex_id() == tripple.second.second)
			continue;

		const graph_type::CEdge	*edge;

		edge					= vertex1.edge((*I).vertex_id());
		if (edge) {
			VERIFY				(_min((*I).weight(),edge->weight()) <= tripple.first);
			if (vertex0.edge(tripple.second.second))
				graph().remove_edge	(tripple.second.first,tripple.second.second);
			if (vertex1.edge(tripple.second.first))
				graph().remove_edge	(tripple.second.second,tripple.second.first);
			continue;
		}

		edge					= graph().vertex((*I).vertex_id())->edge(tripple.second.second);
		if (edge) {
			VERIFY				(_min((*I).weight(),edge->weight()) <= tripple.first);
			if (vertex0.edge(tripple.second.second))
				graph().remove_edge	(tripple.second.first,tripple.second.second);
			if (vertex1.edge(tripple.second.first))
				graph().remove_edge	(tripple.second.second,tripple.second.first);
			continue;
		}
	}
}

void CGameGraphBuilder::optimize_graph		(const float &start, const float &amount)
{
	Progress					(start);

	Msg							("Optimizing graph");

	Msg							("edges before optimization : %d",graph().edge_count());

	create_tripples				(start + .00f, amount*.50f);

	TRIPPLES::const_iterator	I = m_tripples.begin();
	TRIPPLES::const_iterator	E = m_tripples.end();
	for ( ; I != E; ++I)
		process_tripple			(*I);

	Msg							("edges after optimization : %d",graph().edge_count());

	Progress					(start + amount);
}

void CGameGraphBuilder::save_graph			(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Saving graph");

	// header
	CMemoryWriter			writer;
	CGameGraph::CHeader		header;
	header.dwVersion		= XRAI_CURRENT_VERSION;
	header.dwLevelCount		= 1;
	header.dwVertexCount	= graph().vertices().size();
	header.dwEdgeCount		= graph().edge_count();
	header.dwDeathPointCount= 0;
	header.m_guid			= m_graph_guid;

	writer.w_u32			(header.dwVersion);
	writer.w_u32			(header.dwLevelCount);
	writer.w_u32			(header.dwVertexCount);
	writer.w_u32			(header.dwEdgeCount);
	writer.w_u32			(header.dwDeathPointCount);

	// levels
	CGameGraph::SLevel		level;
	level.m_offset.set		(0,0,0);
	level.m_id				= 0;
	level.m_name			= m_level_name;
	level.m_section			= "";
	level.m_guid			= level_graph().header().guid();
	header.tpLevels.insert	(std::make_pair(level.m_id,level));

	{
		GameGraph::LEVEL_MAP::iterator	I = header.tpLevels.begin();
		GameGraph::LEVEL_MAP::iterator	E = header.tpLevels.end();
		for ( ; I != E; I++) {
			writer.w_stringZ	((*I).second.name());
			writer.w_fvector3	((*I).second.offset());
			writer.w			(&(*I).second.m_id,sizeof((*I).second.m_id));
			writer.w_stringZ	((*I).second.section());
			writer.w			(&(*I).second.m_guid,sizeof((*I).second.m_guid));
		}
	}

	{
		u32								edge_offset = graph().vertices().size()*sizeof(CGameGraph::CVertex);

		graph_type::const_vertex_iterator	I = graph().vertices().begin();
		graph_type::const_vertex_iterator	E = graph().vertices().end();
		for ( ; I != E; ++I) {
			CGameGraph::CVertex		&vertex = (*I).second->data();

			VERIFY					((*I).second->edges().size() < 256);
			vertex.tNeighbourCount	= (u8)(*I).second->edges().size();
			vertex.dwEdgeOffset		= edge_offset;
			edge_offset				+= vertex.tNeighbourCount*sizeof(CGameGraph::CEdge);

			writer.w				(&vertex,sizeof(CGameGraph::CVertex));
		}
	}
	
	{
		graph_type::const_vertex_iterator	I = graph().vertices().begin();
		graph_type::const_vertex_iterator	E = graph().vertices().end();
		for ( ; I != E; ++I) {
			graph_type::const_iterator	i = (*I).second->edges().begin();
			graph_type::const_iterator	e = (*I).second->edges().end();
			for ( ; i != e; ++i) {
				writer.w_u32				((*i).vertex_id());	
				writer.w_float				((*i).weight());	
			}
		}
	}

	string_path					file_name;
	strconcat					(file_name,*m_level_name,GAME_LEVEL_GRAPH_TEST);
	writer.save_to				(file_name);
	Msg							("%d bytes saved",int(writer.size()));

	Progress					(start + amount);
}

void CGameGraphBuilder::build_graph			(const float &start, const float &amount)
{
	Progress				(start);

	Msg						("Building graph");

	CTimer					timer;
	timer.Start				();

	m_graph_engine			= xr_new<CGraphEngine>(level_graph().header().vertex_count());
	Progress				(start + 0.000000f*amount + amount*0.077419f);
//	Msg						("BG : %f",timer.GetElapsed_sec());

	generate_edges			(start + 0.077419f*amount, amount*0.914833f);
//	Msg						("BG : %f",timer.GetElapsed_sec());

	xr_delete				(m_graph_engine);
	Progress				(start + 0.992253f*amount + amount*0.002526f);
//	Msg						("BG : %f",timer.GetElapsed_sec());

	connectivity_check		(start + 0.994779f*amount, amount*0.000035f);
//	Msg						("BG : %f",timer.GetElapsed_sec());
	optimize_graph			(start + 0.994814f*amount, amount*0.000524f);
//	Msg						("BG : %f",timer.GetElapsed_sec());
	save_graph				(start + 0.995338f*amount, amount*0.004662f);
//	Msg						("BG : %f",timer.GetElapsed_sec());

	Progress				(start + amount);
}

void CGameGraphBuilder::build_graph			(LPCSTR level_name)
{
	Phase					("Building level game graph");
	Msg						("level \"%s\"",level_name);

	m_level_name			= level_name;
	
	CTimer					timer;
	timer.Start				();

	create_graph			(0.000000f,0.000118f);
//	Msg						("%f",timer.GetElapsed_sec());
	load_level_graph		(0.000118f,0.000187f);
//	Msg						("%f",timer.GetElapsed_sec());
	load_graph_points		(0.000305f,0.025693f);
//	Msg						("%f",timer.GetElapsed_sec());
	remove_incoherent_points(0.025998f,0.000004f);
//	Msg						("%f",timer.GetElapsed_sec());
	build_cross_table		(0.026003f,0.951288f);
//	Msg						("%f",timer.GetElapsed_sec());
	build_graph				(0.977291f,0.022709f);
//	Msg						("%f",timer.GetElapsed_sec());

	Msg						("Level graph is generated successfully");
}

typedef GameGraph::CVertex						vertex_type;
typedef CGraphAbstract<vertex_type,float,u32>	graph_type;

graph_type *get_graph						(LPCSTR level_name, LPCSTR graph_name)
{
	string_path				file_name;
	strconcat				(file_name,level_name,graph_name);

	CGameGraph				game_graph(file_name);
	graph_type				*graph = xr_new<graph_type>();
	
	for (u32 i=0, n=game_graph.header().vertex_count(); i<n; ++i)
		graph->add_vertex	(*game_graph.vertex(i),i);

	for (u32 i=0, n=game_graph.header().vertex_count(); i<n; ++i) {
		CGameGraph::const_iterator	I,E;
		game_graph.begin			(i,I,E);
		for ( ; I != E; ++I)
			graph->add_edge	(i,game_graph.value(i,I),game_graph.edge_weight(I));
	}

	return					(graph);
}

void compare_graphs							(LPCSTR level_name)
{
	graph_type				*graph0 = get_graph(level_name,GAME_LEVEL_GRAPH);
	graph_type				*graph1 = get_graph(level_name,GAME_LEVEL_GRAPH_TEST);

	VERIFY					(graph0->vertices().size() == graph1->vertices().size());
	for (u32 i=0, n=graph0->vertices().size(); i<n; ++i) {
		if (graph0->vertex(i)->data().level_vertex_id() != graph1->vertex(i)->data().level_vertex_id())
			Msg				("0 to 1 : [%d] [%d] [%d]",i,graph0->vertex(i)->data().level_vertex_id(),graph1->vertex(i)->data().level_vertex_id());

		VERIFY				(graph0->vertex(i)->data().level_vertex_id() == graph1->vertex(i)->data().level_vertex_id());

		{
			graph_type::const_iterator	I = graph0->vertex(i)->edges().begin();
			graph_type::const_iterator	E = graph0->vertex(i)->edges().end();
			for ( ; I != E; ++I) {
				if (graph1->vertex(i)->edge((*I).vertex_id())) {
//					if (!fsimilar((*I).weight(),graph1->vertex(i)->edge((*I).vertex_id())->weight(),EPS_L))
//						Msg	("0 to 1 : [%d]->[%d] = %f : [%d]->[%d] = %f",i,(*I).vertex_id(),(*I).weight(),i,(*I).vertex_id(),graph1->vertex(i)->edge((*I).vertex_id())->weight());
					continue;
				}

				Msg			("0 to 1 : [%d]->[%d] = %f : no edge",i,(*I).vertex_id(),(*I).weight());
			}
		}

		{
			graph_type::const_iterator	I = graph1->vertex(i)->edges().begin();
			graph_type::const_iterator	E = graph1->vertex(i)->edges().end();
			for ( ; I != E; ++I) {
				if (graph0->vertex(i)->edge((*I).vertex_id()))
					continue;

				Msg			("1 to 0 : [%d]->[%d] = %f : no edge",i,(*I).vertex_id(),(*I).weight());
			}
		}
	}

	xr_delete				(graph0);
	xr_delete				(graph1);
}
