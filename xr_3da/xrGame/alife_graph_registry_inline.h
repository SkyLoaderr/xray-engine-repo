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
		Msg						("[LSS] adding object [%s][%d] to graph point %d",object->s_name_replace,object->ID,game_vertex_id);
	}
#endif
	if (!object->m_bOnline && object->used_ai_locations() && object->interactive()) {
		ALife::D_OBJECT_PAIR_IT	I = m_objects[game_vertex_id].objects().find(object->ID);
		R_ASSERT3				(m_objects[game_vertex_id].objects().end() == I,"Specified object has already found on the given graph point!",object->s_name_replace);
		m_objects[game_vertex_id].objects().insert(mk_pair(object->ID,object));
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
			Msg					("[LSS] removing object [%s][%d] from graph point %d",object->s_name_replace,object->ID,game_vertex_id);
		}
	#endif
		ALife::D_OBJECT_PAIR_IT	I = m_objects[game_vertex_id].objects().find(object->ID), J;
		R_ASSERT3				(m_objects[game_vertex_id].objects().end() != I,"Specified object not found on the given graph point!",object->s_name_replace);
		m_objects[game_vertex_id].objects().erase(I);
	}	
	if (update && m_level)
		level().remove			(object,ai().game_graph().vertex(game_vertex_id)->level_id() != level().level_id());
}

IC	void CALifeGraphRegistry::change	(CSE_ALifeDynamicObject *object, ALife::_GRAPH_ID tGraphPointID, ALife::_GRAPH_ID tNextGraphPointID)
{
	VERIFY3						(object->used_ai_locations()/** && (object->interactive() || object->m_bOnline)/**/,object->s_name,object->s_name_replace);
	remove						(object,tGraphPointID);
	add							(object,tNextGraphPointID);
	object->m_tGraphID			= tNextGraphPointID;
	object->o_Position			= ai().game_graph().vertex(object->m_tGraphID)->level_point();
	object->m_tNodeID			= ai().game_graph().vertex(object->m_tGraphID)->level_vertex_id();
}

IC	void CALifeGraphRegistry::remove	(CALifeEvent *event, ALife::_GRAPH_ID game_vertex_id)
{
	ALife::EVENT_PAIR_IT		I = m_objects[game_vertex_id].events().find(event->m_tEventID);
	R_ASSERT2					(m_objects[game_vertex_id].events().end() != I,"Specified object not found on the given graph point!");
	m_objects[game_vertex_id].events().erase(I);
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] removing event [%d] from graph point %d",event->m_tEventID,game_vertex_id);
	}
#endif
}

IC	void CALifeGraphRegistry::add	(CALifeEvent *event, ALife::_GRAPH_ID game_vertex_id)
{
	ALife::EVENT_PAIR_IT		I = m_objects[game_vertex_id].events().find(event->m_tEventID);
	R_ASSERT2					(m_objects[game_vertex_id].events().end() == I,"Specified object has been already found on the given graph point!");
	m_objects[game_vertex_id].events().insert(mk_pair(event->m_tEventID,event));
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

IC	void CALifeGraphRegistry::attach	(CSE_Abstract &object, CSE_ALifeInventoryItem *item, ALife::_GRAPH_ID game_vertex_id, bool alife_query)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] Attaching item [%s][%d] to [%s][%d]",item->base()->s_name_replace,item->base()->ID,object.s_name_replace,object.ID);
	}
#endif
	if (alife_query)
		remove					(dynamic_cast<CSE_ALifeDynamicObject*>(item),game_vertex_id);
	else
		level().remove			(dynamic_cast<CSE_ALifeDynamicObject*>(item));

	CSE_ALifeTraderAbstract		*trader = dynamic_cast<CSE_ALifeTraderAbstract*>(&object);
	R_ASSERT2					(!alife_query || trader,"Cannot attach an item to a non-trader object");

	VERIFY						(alife_query || !dynamic_cast<CSE_ALifeDynamicObject*>(&object) || (ai().game_graph().vertex(dynamic_cast<CSE_ALifeDynamicObject*>(&object)->m_tGraphID)->level_id() == level().level_id()));
	if (trader)
		trader->attach			(item,alife_query);
}

IC	void CALifeGraphRegistry::detach	(CSE_Abstract &object, CSE_ALifeInventoryItem *item, ALife::_GRAPH_ID game_vertex_id, bool alife_query)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] Detaching item [%s][%d] from [%s][%d]",item->base()->s_name_replace,item->base()->ID,object.s_name_replace,object.ID);
	}
#endif
	if (alife_query)
		add						(dynamic_cast<CSE_ALifeDynamicObject*>(item),game_vertex_id);
	else {
		CSE_ALifeDynamicObject	*object = dynamic_cast<CSE_ALifeDynamicObject*>(item);
		VERIFY					(object);
		object->m_tGraphID		= game_vertex_id;
		level().add 			(object);
	}

	CSE_ALifeTraderAbstract		*trader = dynamic_cast<CSE_ALifeTraderAbstract*>(&object);
	R_ASSERT2					(!alife_query || trader,"Cannot detach an item from non-trader object");
	
	VERIFY						(alife_query || !dynamic_cast<CSE_ALifeDynamicObject*>(&object) || (ai().game_graph().vertex(dynamic_cast<CSE_ALifeDynamicObject*>(&object)->m_tGraphID)->level_id() == level().level_id()));

	if (trader)
		trader->detach			(item,0,alife_query);
	else
		R_ASSERT2				(std::find(object.children.begin(),object.children.end(),item->base()->ID) != object.children.end(),"Can't detach an item which is not on my own");
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
