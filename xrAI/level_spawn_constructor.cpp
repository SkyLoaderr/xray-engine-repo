////////////////////////////////////////////////////////////////////////////
//	Module 		: level_spawn_constructor.cpp
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Level spawn constructor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_spawn_constructor.h"
#include "game_level_cross_table.h"
#include "level_graph.h"
#include "graph_engine.h"
#include "xrmessages.h"
#include "xrServer_Objects_ALife_All.h"
#include "xrSE_Factory_import_export.h"
#include "clsid_game.h"
#include "game_base_space.h"
#include "game_spawn_constructor.h"

CLevelSpawnConstructor::~CLevelSpawnConstructor					()
{
	GRAPH_POINT_STORAGE::iterator	I = m_graph_points.begin();
	GRAPH_POINT_STORAGE::iterator	E = m_graph_points.end();
	for ( ; I != E; ++I)
		F_entity_Destroy			((CSE_Abstract*&)(*I));

	VERIFY					(!m_level_graph);
	VERIFY					(!m_cross_table);
	VERIFY					(!m_graph_engine);
}

IC	const CGameGraph &CLevelSpawnConstructor::game_graph		() const
{
	return					(m_game_spawn_constructor->game_graph());
}

IC	CLevelSpawnConstructor::LEVEL_CHANGER_STORAGE &CLevelSpawnConstructor::level_changers() const
{
	return					(m_game_spawn_constructor->level_changers());
}

IC	u32	CLevelSpawnConstructor::level_id						(shared_str level_name) const
{
	return					(m_game_spawn_constructor->level_id(*level_name));
}

IC	const CLevelGraph &CLevelSpawnConstructor::level_graph		() const
{
	return					(*m_level_graph);
}

IC	const CGameLevelCrossTable &CLevelSpawnConstructor::cross_table	() const
{
	return					(*m_cross_table);
}

IC	CGraphEngine &CLevelSpawnConstructor::graph_engine			() const
{
	return					(*m_graph_engine);
}

void CLevelSpawnConstructor::init								()
{
	// loading level graph
	string256				file_name;
	FS.update_path			(file_name,"$game_levels$",*m_level.caLevelName);
	strcat					(file_name,"\\");
	m_level_graph			= xr_new<CLevelGraph>(file_name);
	
	// loading cross table 
	strcat					(file_name,CROSS_TABLE_NAME);
	m_cross_table			= xr_new<CGameLevelCrossTable>(file_name);
}

CSE_Abstract *CLevelSpawnConstructor::create_object						(IReader *chunk)
{
	NET_Packet				net_packet;
	net_packet.B.count		= chunk->length();
	chunk->r				(net_packet.B.data,net_packet.B.count);
	chunk->close			();
	u16						ID;
	net_packet.r_begin		(ID);
	R_ASSERT2				(M_SPAWN==ID,"ID doesn't match to the spawn-point ID!");
	string64				section_name;
	net_packet.r_stringZ	(section_name);
	CSE_Abstract			*abstract = F_entity_Create(section_name);
	if (!abstract) {
		string256			temp;
		sprintf				(temp,"Can't create entity '%s' !\n",abstract->s_name_replace);
		R_ASSERT2			(abstract,temp);
	}
	abstract->Spawn_Read	(net_packet);
	return					(abstract);
}

void CLevelSpawnConstructor::add_graph_point					(CSE_Abstract			*abstract)
{
	CSE_ALifeGraphPoint		*graph_point = smart_cast<CSE_ALifeGraphPoint*>(abstract);
	R_ASSERT				(graph_point);
	m_graph_points.push_back(graph_point);
}

void CLevelSpawnConstructor::add_spawn_group					(CSE_Abstract			*abstract)
{
	CSE_SpawnGroup			*spawn_group = smart_cast<CSE_SpawnGroup*>(abstract);
	R_ASSERT				(spawn_group);
	m_spawn_groups.insert	(std::make_pair(spawn_group->s_name_replace,spawn_group));
	if (xr_strlen(spawn_group->m_spawn_control))
		add_group_object	(spawn_group,spawn_group->m_spawn_control);
	add_free_object			(abstract);
}

void CLevelSpawnConstructor::add_story_object					(CSE_ALifeDynamicObject *dynamic_object)
{
	m_game_spawn_constructor->add_story_object	(dynamic_object->m_story_id,dynamic_object,m_level.name());
}

void CLevelSpawnConstructor::add_level_changer					(CSE_Abstract			*abstract)
{
	CSE_ALifeLevelChanger	*level_changer = smart_cast<CSE_ALifeLevelChanger*>(abstract);
	R_ASSERT				(level_changer);
	m_game_spawn_constructor->add_level_changer	(level_changer);
}

void CLevelSpawnConstructor::add_free_object					(CSE_Abstract			*abstract)
{
	m_game_spawn_constructor->add_object		(abstract);
}

void CLevelSpawnConstructor::add_group_object					(CSE_Abstract			*abstract, shared_str group_section, bool)
{
	SPAWN_GRPOUP_OBJECTS::iterator	I = m_spawn_objects.find(group_section);
	if (I == m_spawn_objects.end()) {
		xr_vector<CSE_Abstract*>	*temp = xr_new<GROUP_OBJECTS>();
		temp->clear					();
		temp->push_back				(abstract);
		m_spawn_objects.insert		(std::make_pair(group_section,temp));
	}
	else
		(*I).second->push_back		(abstract);
}

void CLevelSpawnConstructor::add_group_object					(CSE_Abstract			*abstract, shared_str group_section)
{
	string256					temp;
	for (u32 i=0, n=_GetItemCount(*group_section); i<n; ++i)
		add_group_object		(abstract,_GetItem(*group_section,i,temp),true);
}

void CLevelSpawnConstructor::load_objects						()
{
	// loading spawn points
	string256					file_name;
	FS.update_path				(file_name,"$game_levels$",*m_level.caLevelName);
	strcat						(file_name,"\\level.spawn");
	IReader						*level_spawn = FS.r_open(file_name);
	IReader						*chunk = 0;
	for (int id = 0; 0 != (chunk = level_spawn->open_chunk(id)); ++id) {
		CSE_Abstract			*abstract = create_object(chunk);
		if (abstract->m_tClassID == CLSID_AI_GRAPH) {
			add_graph_point		(abstract);
			continue;
		}

		if (abstract->m_tClassID == CLSID_AI_SPAWN_GROUP) {
			add_spawn_group		(abstract);
			continue;
		}

		if ((abstract->s_gameid != GAME_SINGLE) && (abstract->s_gameid != GAME_ANY)) {
			F_entity_Destroy	(abstract);
			continue;
		}

		CSE_ALifeObject			*alife_object = smart_cast<CSE_ALifeObject*>(abstract);
		if (!alife_object) {
			F_entity_Destroy	(abstract);
			continue;
		}

		CSE_ALifeCreatureActor	*actor = smart_cast<CSE_ALifeCreatureActor*>(alife_object);
		if (actor) {
			R_ASSERT3			(!m_actor,"Too many actors on the level ",m_level.name());
			m_actor				= actor;
		}

		m_spawns.push_back		(alife_object);

		CSE_ALifeDynamicObject	*dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(alife_object);
		if (dynamic_object)
			add_story_object	(dynamic_object);

		if (abstract->m_tClassID == CLSID_LEVEL_CHANGER)
			add_level_changer	(abstract);

		if (xr_strlen(alife_object->m_spawn_control))
			add_group_object	(alife_object,alife_object->m_spawn_control);

		add_free_object			(alife_object);
	}
	
	FS.r_close					(level_spawn);

	R_ASSERT2					(!m_spawns.empty(),"There are no spawn-points!");
}

IC	void CLevelSpawnConstructor::normalize_probability			(CSE_ALifeAnomalousZone *zone)
{
	float						accumulator = 0.f;
	for (int ii=0; ii<zone->m_wItemCount; ii++)
		accumulator				+= zone->m_faWeights[ii];

	accumulator					*= zone->m_fBirthProbability;

	for (int ii=0; ii<zone->m_wItemCount; ii++)
		zone->m_faWeights[ii]	/= accumulator;
}

IC	void CLevelSpawnConstructor::free_group_objects					()
{
	SPAWN_GRPOUP_OBJECTS::iterator	I = m_spawn_objects.begin();
	SPAWN_GRPOUP_OBJECTS::iterator	E = m_spawn_objects.end();
	for ( ; I != E; I++)
		xr_delete		((*I).second);
}

void CLevelSpawnConstructor::fill_spawn_groups					()
{
	SPAWN_GRPOUP_OBJECTS::iterator				I = m_spawn_objects.begin();
	SPAWN_GRPOUP_OBJECTS::iterator				E = m_spawn_objects.end();
	
	for ( ; I != E; I++) {
		R_ASSERT								(xr_strlen(*(*I).first));
		R_ASSERT								((*I).second);
		SPAWN_GROUPS::iterator					J = m_spawn_groups.find((*I).first);
		if (J == m_spawn_groups.end())
			clMsg								("! ERROR (spawn group not found!) : %s",*(*I).first);
		R_ASSERT3								(J != m_spawn_groups.end(),"Specified group control not found!",(*(*I).second)[0]->s_name_replace);
		
		GROUP_OBJECTS::iterator					i = (*I).second->begin();
		GROUP_OBJECTS::iterator					e = (*I).second->end();
		for ( ; i != e; i++) {
			m_game_spawn_constructor->add_edge	((*J).second->m_tSpawnID,(*i)->m_tSpawnID,(*i)->m_spawn_probability);
			CSE_ALifeAnomalousZone				*zone = smart_cast<CSE_ALifeAnomalousZone*>(*i);
			if (zone)
				normalize_probability			(zone);
		}
	}
	
	free_group_objects							();
}

void CLevelSpawnConstructor::correct_objects					()
{
	u32						m_level_graph_vertex_id = u32(-1);
	u32						dwStart = game_graph().header().vertex_count(), dwFinish = game_graph().header().vertex_count(), dwCount = 0;
	for (u32 i=0; i<game_graph().header().vertex_count(); ++i)
		if (game_graph().vertex(i)->level_id() == m_level.id()) {
			if (m_level_graph_vertex_id == u32(-1))
				m_level_graph_vertex_id = i;
			dwCount++;
		}
	
	for (int i=0; i<(int)game_graph().header().vertex_count(); i++)
		if (game_graph().vertex(i)->level_id() == m_level.id()) {
			if (dwStart > (u32)i)
				dwStart = (u32)i;
		}
		else {
			if ((dwStart <= (u32)i) && (dwFinish > (u32)i)) {
				dwFinish = i;
				break;
			}
		}
	if (dwStart >= dwFinish) {
		string4096			S;
		sprintf				(S,"There are no graph vertices in the game graph for the level '%s' !\n",*m_level.name());
		R_ASSERT2			(dwStart < dwFinish,S);
	}

	for (int i=0; i<(int)m_spawns.size(); i++) {
		if (!m_spawns[i]->used_ai_locations()) {
			m_spawns[i]->m_tGraphID = (ALife::_GRAPH_ID)m_level_graph_vertex_id;
			m_spawns[i]->m_fDistance = 0.f;
			m_spawns[i]->m_tNodeID = game_graph().vertex(m_level_graph_vertex_id)->level_vertex_id();
			continue;
		}
		m_spawns[i]->m_tNodeID = level_graph().vertex(u32(-1),m_spawns[i]->o_Position);
		VERIFY				(level_graph().valid_vertex_id(m_spawns[i]->m_tNodeID));
		if (m_spawns[i]->used_ai_locations() && !level_graph().inside(level_graph().vertex(m_spawns[i]->m_tNodeID),m_spawns[i]->o_Position)) {
			Fvector			new_position = level_graph().vertex_position(m_spawns[i]->m_tNodeID);
			clMsg			("[%s][%s][%s] : position changed from [%f][%f][%f] -> [%f][%f][%f]",m_level.name(),*m_spawns[i]->s_name,m_spawns[i]->s_name_replace,VPUSH(m_spawns[i]->o_Position),VPUSH(new_position));
			m_spawns[i]->o_Position	= new_position;
		}
		u32 dwBest = cross_table().vertex(m_spawns[i]->m_tNodeID).game_vertex_id();
		VERIFY				(game_graph().vertex(dwBest)->level_id() == m_level.id());
		float fCurrentBestDistance = cross_table().vertex(m_spawns[i]->m_tNodeID).distance();
		if (dwBest == u32(-1)) {
			string4096 S1;
			char *S = S1;
			S += sprintf(S,"Can't find a corresponding GRAPH VERTEX for the spawn-point %s\n",m_spawns[i]->s_name_replace);
			S += sprintf(S,"Level ID    : %d\n",m_level.id());
			S += sprintf(S,"Spawn index : %d\n",i);
			S += sprintf(S,"Spawn node  : %d\n",m_spawns[i]->m_tNodeID);
			S += sprintf(S,"Spawn point : [%7.2f][%7.2f][%7.2f]\n",m_spawns[i]->o_Position.x,m_spawns[i]->o_Position.y,m_spawns[i]->o_Position.z);
			R_ASSERT2(dwBest != -1,S1);
		}
		m_spawns[i]->m_tGraphID		= (ALife::_GRAPH_ID)dwBest;
		m_spawns[i]->m_fDistance	= fCurrentBestDistance;
	}
}

void CLevelSpawnConstructor::generate_artefact_spawn_positions	()
{
	// create graph engine
	m_graph_engine			= xr_new<CGraphEngine>(m_level_graph->header().vertex_count());

	xr_vector<u32>			l_tpaStack;
	l_tpaStack.reserve		(1024);
	SPAWN_STORAGE::iterator	B = m_spawns.begin(), I = B;
	SPAWN_STORAGE::iterator	E = m_spawns.end();
	for ( ; I != E; I++) {
		CSE_ALifeAnomalousZone *zone = smart_cast<CSE_ALifeAnomalousZone*>(*I);
		if (!zone)
			continue;

		graph_engine().search(level_graph(),zone->m_tNodeID,zone->m_tNodeID,&l_tpaStack,SFlooder<float,u32,u32>((float)zone->m_fRadius,u32(-1),u32(-1)));

		if (zone->m_wArtefactSpawnCount >= l_tpaStack.size()) {
			zone->m_wArtefactSpawnCount	= (u16)l_tpaStack.size();
			zone->m_dwStartIndex			= m_level_points.size();
			m_level_points.resize							(zone->m_dwStartIndex + zone->m_wArtefactSpawnCount);
			xr_vector<CGameGraph::CLevelPoint>::iterator	I = m_level_points.begin() + zone->m_dwStartIndex;
			xr_vector<CGameGraph::CLevelPoint>::iterator	E = m_level_points.end();
			xr_vector<u32>::iterator						i = l_tpaStack.begin();
			for ( ; I != E; I++, i++) {
				(*I).tNodeID	= *i;
				(*I).tPoint		= level_graph().vertex_position(*i);
				(*I).fDistance	= cross_table().vertex(*i).distance();
			}
		}
		else {
			random_shuffle									(l_tpaStack.begin(),l_tpaStack.end());
			zone->m_dwStartIndex			= m_level_points.size();
			m_level_points.resize							(zone->m_dwStartIndex + zone->m_wArtefactSpawnCount);
			xr_vector<CGameGraph::CLevelPoint>::iterator	I = m_level_points.begin() + zone->m_dwStartIndex;
			xr_vector<CGameGraph::CLevelPoint>::iterator	E = m_level_points.end();
			xr_vector<u32>::iterator						i = l_tpaStack.begin();
			for ( ; I != E; I++, i++) {
				(*I).tNodeID	= *i;
				(*I).tPoint		= level_graph().vertex_position(*i);
				(*I).fDistance	= cross_table().vertex(*i).distance();
			}
		}
		l_tpaStack.clear		();
	}
}

void CLevelSpawnConstructor::fill_level_changers				()
{
	for (u32 i=0, n=(u32)level_changers().size(); i<n; ++i) {
		if (level_id(level_changers()[i]->m_caLevelToChange) != m_level.id())
			continue;

		bool found = false;
		GRAPH_POINT_STORAGE::const_iterator I = m_graph_points.begin();
		GRAPH_POINT_STORAGE::const_iterator E = m_graph_points.end();
		for ( ; I != E; ++I)
			if (!xr_strcmp(*level_changers()[i]->m_caLevelPointToChange,(*I)->s_name_replace)) {
				bool ok = false;
				for (u32 ii=0, nn = game_graph().header().vertex_count(); ii<nn; ++ii) {
					if ((game_graph().vertex(ii)->level_id() != m_level.id()) || !game_graph().vertex(ii)->level_point().similar((*I)->o_Position,.001f))
						continue;
					level_changers()[i]->m_tNextGraphID		= (ALife::_GRAPH_ID)ii;
					level_changers()[i]->m_tNextPosition	= (*I)->o_Position;
					level_changers()[i]->m_tAngles			= (*I)->o_Angle;
					level_changers()[i]->m_dwNextNodeID		= game_graph().vertex(ii)->level_vertex_id();
					ok										= true;
					break;
				}

				R_ASSERT3					(ok,"Cannot find a correspndance between graph and graph points from level editor! Rebuild graph for the level ",*level_changers()[i]->m_caLevelToChange);

				level_changers().erase		(level_changers().begin() + i);
				--i;
				--n;
				found		= true;
				break;
			}

		if (!found) {
			clMsg			("Graph point %s not found (level changer %s)",*level_changers()[i]->m_caLevelPointToChange,level_changers()[i]->s_name_replace);
			VERIFY			(false);
		}
	}
}

void CLevelSpawnConstructor::update_artefact_spawn_positions	()
{
	u32									level_point_count = m_game_spawn_constructor->level_point_count();
	SPAWN_STORAGE::iterator				I = m_spawns.begin();
	SPAWN_STORAGE::iterator				E = m_spawns.end();
	for ( ; I != E; ++I) {
		CSE_Abstract					*abstract = *I;
		CSE_ALifeObject					*alife_object = smart_cast<CSE_ALifeObject*>(abstract);
//		R_ASSERT3						(level_graph().valid_vertex_id(alife_object->m_tNodeID),"Invalid node for object ",alife_object->s_name_replace);
		R_ASSERT2						(alife_object,"Non-ALife object!");
		VERIFY							(game_graph().vertex(alife_object->m_tGraphID)->level_id() == m_level.id());
		alife_object->m_spawn_control	= "";
		CSE_ALifeAnomalousZone			*zone = smart_cast<CSE_ALifeAnomalousZone*>(abstract);
		if (zone) {
			zone->m_dwStartIndex		= level_point_count;
			level_point_count			+= zone->m_wArtefactSpawnCount;
		}
	}

	m_game_spawn_constructor->add_level_points	(m_level_points);
}

void CLevelSpawnConstructor::Execute							()
{
	load_objects						();
	fill_spawn_groups					();
	
	init								();
	
	correct_objects						();
	generate_artefact_spawn_positions	();

	xr_delete							(m_level_graph);
	xr_delete							(m_cross_table);
	xr_delete							(m_graph_engine);
}

void CLevelSpawnConstructor::update								()
{
	fill_level_changers					();
	update_artefact_spawn_positions		();
}
