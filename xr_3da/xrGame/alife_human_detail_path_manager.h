////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_detail_path_manager.h
//	Created 	: 01.11.2005
//  Modified 	: 01.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human detail path manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"
#include "alife_space.h"
#include "script_export_space.h"

class CSE_ALifeHumanAbstract;
class CALifeSmartTerrainTask;

class CALifeHumanDetailPathManager {
public:
	typedef CSE_ALifeHumanAbstract	object_type;
	typedef xr_vector<u32>			PATH;

private:
	struct parameters {
		GameGraph::_GRAPH_ID		m_game_vertex_id;
		u32							m_level_vertex_id;
	};

private:
	object_type			*m_object;
	ALife::_TIME_ID		m_last_update_time;
	parameters			m_destination;
	float				m_walked_distance;
	float				m_speed;

private:
	PATH				m_path;						
	// this is INVERTED path, i.e. 
	// start vertex is the last one
	// destination vertex is the first one.
	// this is useful, since iterating back
	// on this vector during path following
	// we just repeatedly remove the last 
	// vertex, and this operation is 
	// efficiently implemented in std::vector

private:
			void		actualize					();
			void		follow_path					(const ALife::_TIME_ID &time_delta);
			void		update						(const ALife::_TIME_ID &time_delta);

public:
						CALifeHumanDetailPathManager(object_type *object);
	IC		object_type	&object						() const;

public:
			void		target						(const GameGraph::_GRAPH_ID &game_vertex_id, const u32 &level_vertex_id);
			void		target						(const GameGraph::_GRAPH_ID &game_vertex_id);
			void		target						(const CALifeSmartTerrainTask &task);

public:
			void		update						();
	IC		void		speed						(const float &speed);

public:
	IC		const float	&speed						() const;
			bool		completed					() const;
			bool		actual						() const;
			bool		failed						() const;
	IC		const PATH	&path						() const;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
#undef script_type_list
add_to_type_list(CALifeHumanDetailPathManager)
#define script_type_list save_type_list(CALifeHumanDetailPathManager)

#include "alife_human_detail_path_manager_inline.h"