////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_graph_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife graph registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeLevelRegistry &CALifeGraphRegistry::level	() const
{
	VERIFY						(m_level);
	return						(*m_level);
}

IC	void CALifeGraphRegistry::add	(CSE_ALifeDynamicObject *object, ALife::_GRAPH_ID game_vertex_id, bool update)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] adding object [%s][%d] to graph point %d",object->name_replace(),object->ID,game_vertex_id);
	}
#endif
	if (!object->m_bOnline && object->used_ai_locations() && object->interactive()) {
		m_objects[game_vertex_id].objects().add(object->ID,object);
		object->m_tGraphID		= game_vertex_id;
	}
	else
		if (!m_level && update) {
			m_temp.push_back	(object);
			object->m_tGraphID	= game_vertex_id;
		}
	
	if (update && m_level)
		level().add				(object);
}

IC	void CALifeGraphRegistry::remove	(CSE_ALifeDynamicObject *object, ALife::_GRAPH_ID game_vertex_id, bool update)
{
	if (object->used_ai_locations() && object->interactive()) {
	#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg					("[LSS] removing object [%s][%d] from graph point %d",object->name_replace(),object->ID,game_vertex_id);
		}
	#endif
		m_objects[game_vertex_id].objects().remove(object->ID);
	}	
	if (update && m_level)
		level().remove			(object,ai().game_graph().vertex(game_vertex_id)->level_id() != level().level_id());
}

IC	void CALifeGraphRegistry::change	(CSE_ALifeDynamicObject *object, ALife::_GRAPH_ID tGraphPointID, ALife::_GRAPH_ID tNextGraphPointID)
{
	VERIFY3						(object->used_ai_locations()/** && (object->interactive() || object->m_bOnline)/**/,*object->s_name,object->name_replace());
	remove						(object,tGraphPointID);
	add							(object,tNextGraphPointID);
	object->m_tGraphID			= tNextGraphPointID;
	object->o_Position			= ai().game_graph().vertex(object->m_tGraphID)->level_point();
	object->m_tNodeID			= ai().game_graph().vertex(object->m_tGraphID)->level_vertex_id();
}

IC	void CALifeGraphRegistry::remove	(CALifeEvent *event, ALife::_GRAPH_ID game_vertex_id)
{
	m_objects[game_vertex_id].events().remove(event->m_tEventID);
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] removing event [%d] from graph point %d",event->m_tEventID,game_vertex_id);
	}
#endif
}

IC	void CALifeGraphRegistry::add	(CALifeEvent *event, ALife::_GRAPH_ID game_vertex_id)
{
	m_objects[game_vertex_id].events().add(event->m_tEventID,event);
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] adding event [%d] from graph point %d",event->m_tEventID,game_vertex_id);
	}
#endif
}

IC	void CALifeGraphRegistry::change	(CALifeEvent *event, ALife::_GRAPH_ID game_vertex_id, ALife::_GRAPH_ID next_game_vertex_id)
{
	remove						(event,game_vertex_id);
	add							(event,next_game_vertex_id);
}

IC	void CALifeGraphRegistry::assign	(CSE_ALifeMonsterAbstract *monster)
{
	monster->m_tNextGraphID		= monster->m_tPrevGraphID = monster->m_tGraphID;
	monster->m_fDistanceToPoint	= monster->m_fDistance;
	CGameGraph::const_iterator	i,e;
	ai().game_graph().begin		(monster->m_tNextGraphID,i,e);
	for ( ; i != e; ++i)
		if ((*i).distance() > monster->m_fDistance) {
			monster->m_fDistanceFromPoint	= (*i).distance() - monster->m_fDistance;
			break;
		}
}

IC	void CALifeGraphRegistry::set_process_time	(const u64 &process_time)
{
	m_process_time				= process_time;
	if (m_level)
		level().set_process_time(m_process_time);
}

IC	CSE_ALifeCreatureActor *CALifeGraphRegistry::actor	() const
{
	return						(m_actor);
}

IC	const CALifeGraphRegistry::GRAPH_REGISTRY &CALifeGraphRegistry::objects	() const
{
	return						(m_objects);
}

template <typename F>
IC	void CALifeGraphRegistry::iterate_objects			(ALife::_GRAPH_ID game_vertex_id, const F& f)
{
	iterate						(((CGraphPointInfo&)(objects()[game_vertex_id])).objects(),f);
}

template <typename F>
IC	void CALifeGraphRegistry::iterate_events			(ALife::_GRAPH_ID game_vertex_id, const F& f)
{
	iterate						(((CGraphPointInfo&)(objects()[game_vertex_id])).events(),f);
}

template <typename F, typename C>
IC	void CALifeGraphRegistry::iterate					(C &c, const F& f)
{
	c.update					(f);
}
