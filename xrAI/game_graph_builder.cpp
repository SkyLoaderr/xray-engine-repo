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

CGameGraphBuilder::CGameGraphBuilder		()
{
	m_level_graph			= 0;
	m_graph					= 0;
}

CGameGraphBuilder::~CGameGraphBuilder		()
{
	xr_delete				(m_level_graph);
	xr_delete				(m_graph);
}

void CGameGraphBuilder::load_level_graph	()
{
	Phase					("Loading AI map");
	Progress				(0.f);
	VERIFY					(!m_level_graph);
	m_level_graph			= xr_new<CLevelGraph>(*m_level_name);
	Progress				(1.f);
	Msg						("%d nodes loaded",level_graph().header().vertex_count());
}

void CGameGraphBuilder::load_graph_point	(NET_Packet &net_packet)
{
	string256				section_id;
	u16						id;
	net_packet.r_begin		(id);
	R_ASSERT				(M_SPAWN == id);
	net_packet.r_stringZ	(section_id);
	CSE_Abstract			*entity = F_entity_Create(section_id);
	if (!entity) {
		Msg					("Cannot create entity from section %s, skipping",section_id);
		return;
	}

	entity->Spawn_Read		(net_packet);

	CSE_ALifeGraphPoint		*graph_point = smart_cast<CSE_ALifeGraphPoint*>(entity);
	if (!graph_point) {
		F_entity_Destroy	(entity);
		return;
	}

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

void CGameGraphBuilder::load_graph_points	()
{
	Phase					("Loading graph points");
	Progress				(0.f);
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
	Progress				(1.f);
	Msg						("%d graph points loaded",graph().vertices().size());
}

void CGameGraphBuilder::build_graph			(LPCSTR level_name)
{
	Msg						("Building level graph for the level \"%s\"",level_name);

	m_level_name			= level_name;
	
	VERIFY					(!m_graph);
	m_graph					= xr_new<graph_type>();

	load_level_graph		();
	load_graph_points		();
}
