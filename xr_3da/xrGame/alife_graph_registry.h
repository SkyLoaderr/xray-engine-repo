////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_graph_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife graph registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Objects_ALife_All.h"
#include "alife_level_registry.h"
#include "alife_event.h"

class CSE_ALifeCreatureActor;

class CALifeGraphRegistry {
public:
	typedef CSafeMapIterator<
		ALife::_OBJECT_ID,
		CSE_ALifeDynamicObject,
		std::less<
			ALife::_OBJECT_ID
		>,
		false
	>							OBJECT_REGISTRY;
	
	typedef CSafeMapIterator<
		ALife::_EVENT_ID,
		CALifeEvent,
		std::less<
			ALife::_EVENT_ID
		>,
		false
	>							EVENT_REGISTRY;

public:
	class CGraphPointInfo {
	protected:
		OBJECT_REGISTRY		m_objects;
		EVENT_REGISTRY		m_events;

	public:
		IC	OBJECT_REGISTRY	&objects()
		{
			return			(m_objects);
		}
		
		IC	const OBJECT_REGISTRY	&objects() const
		{
			return			(m_objects);
		}

		IC	EVENT_REGISTRY	&events()
		{
			return			(m_events);
		}
	};

public:
	typedef xr_vector<CGraphPointInfo>	GRAPH_REGISTRY;
	typedef xr_vector<GameGraph::_GRAPH_ID>	TERRAIN_REGISTRY;

protected:
	GRAPH_REGISTRY						m_objects;
	TERRAIN_REGISTRY					m_terrain[GameGraph::LOCATION_TYPE_COUNT][GameGraph::LOCATION_COUNT];	
	CALifeLevelRegistry					*m_level;
	CSE_ALifeCreatureActor				*m_actor;
	u64									m_process_time;
	xr_vector<CSE_ALifeDynamicObject*>	m_temp;

protected:
			void						setup_current_level		();
	template <typename F, typename C>
	IC		void						iterate					(C &c, const F& f);

public:
										CALifeGraphRegistry		();
	virtual								~CALifeGraphRegistry	();
			void						update					(CSE_ALifeDynamicObject		*object);
			void						attach					(CSE_Abstract				&object,	CSE_ALifeInventoryItem	*item,			GameGraph::_GRAPH_ID	game_vertex_id,				bool alife_query = true, bool add_children = true);
			void						detach					(CSE_Abstract				&object,	CSE_ALifeInventoryItem	*item,			GameGraph::_GRAPH_ID	game_vertex_id,				bool alife_query = true, bool remove_children = true);
	IC		void						assign					(CSE_ALifeMonsterAbstract	*object);
	IC		void						add						(CALifeEvent				*event,		GameGraph::_GRAPH_ID		game_vertex_id);
	IC		void						remove					(CALifeEvent				*event,		GameGraph::_GRAPH_ID		game_vertex_id);
			void						add						(CSE_ALifeDynamicObject		*object,	GameGraph::_GRAPH_ID		game_vertex_id,	bool				bUpdateSwitchObjects = true);
			void						remove					(CSE_ALifeDynamicObject		*object,	GameGraph::_GRAPH_ID		game_vertex_id,	bool				bUpdateSwitchObjects = true);
	IC		void						change					(CSE_ALifeDynamicObject		*object,	GameGraph::_GRAPH_ID		game_vertex_id,	GameGraph::_GRAPH_ID	next_game_vertex_id);
	IC		void						change					(CALifeEvent				*event,		GameGraph::_GRAPH_ID		game_vertex_id,	GameGraph::_GRAPH_ID	next_game_vertex_id);
	IC		CALifeLevelRegistry			&level					() const;
	IC		void						set_process_time		(const u64 &process_time);
	IC		CSE_ALifeCreatureActor		*actor					() const;
	IC		const GRAPH_REGISTRY		&objects				() const;
	template <typename F>
	IC		void						iterate_objects			(GameGraph::_GRAPH_ID game_vertex_id, const F& f);
	template <typename F>
	IC		void						iterate_events			(GameGraph::_GRAPH_ID game_vertex_id, const F& f);
};

#include "alife_graph_registry_inline.h"