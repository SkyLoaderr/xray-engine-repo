////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_search_engine.h
//	Created 	: 21.03.2002
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Graph search engine
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph.h"
#include "level_graph.h"
#include "data_storage_priority_queue.h"
#include "data_storage_heap.h"
#include "data_storage_list.h"
#include "boost\\f_heap.hpp"
#include "boost\\p_heap.hpp"
#include "path_manager.h"
#include "a_star.h"
#include "ai_space.h"

class CGraphSearchEngine {
protected:
	// distance type definition
	typedef float _dist_type;

	// data storage type definition
//	typedef CDataStorageUL				<							_dist_type,u32,u32,true,24,8> CDataStorage;
//	typedef CDataStorageDLUL			<							_dist_type,u32,u32,true,24,8> CDataStorage;
//	typedef CDataStorageSL				<							_dist_type,u32,u32,true,24,8> CDataStorage;
//	typedef CDataStorageDLSL			<							_dist_type,u32,u32,true,24,8> CDataStorage;
//	typedef CDataStorageBinaryHeap		<							_dist_type,u32,u32,true,24,8> CDataStorage;
//	typedef CDataStorageBinaryHeapList	<4,							_dist_type,u32,u32,true,24,8> CDataStorage;
//	typedef CDataStorageMultiBinaryHeap	<4,							_dist_type,u32,u32,true,24,8> CDataStorage;
//	typedef CDataStorageCheapList		<32,true,true,				_dist_type,u32,u32,true,24,8> CDataStorage;
	typedef CDataStorageBucketList		<8*1024,false,				_dist_type,u32,u32,true,24,8> CDataStorage;
//	typedef CDataStoragePriorityQueue	<boost::fibonacci_heap,		_dist_type,u32,u32,true,24,8> CDataStorage;
//	typedef CDataStoragePriorityQueue	<boost::pairing_heap,		_dist_type,u32,u32,true,24,8> CDataStorage;
	
	// path managers type definition
	typedef CPathManager<CGameGraph, CDataStorage,_dist_type,u32,u32>			CPathManagerGame;
	typedef CPathManager<CLevelGraph,CDataStorage,_dist_type,u32,u32>			CPathManagerLevelMinTime;
	typedef CPathManager<CLevelGraph,CDataStorage,_dist_type,u32,u32>			CPathManagerLevelCoverLight;
	typedef CPathManager<CLevelGraph,CDataStorage,_dist_type,u32,u32>			CPathManagerLevelPositionDetour;

	// a-star type definition
	typedef CAStar<CDataStorage,CPathManagerGame,u32,_dist_type>				CAStarGame;
	typedef CAStar<CDataStorage,CPathManagerLevelMinTime,u32,_dist_type>		CAStarLevelMinTime;
	typedef CAStar<CDataStorage,CPathManagerLevelCoverLight,u32,_dist_type>		CAStarLevelCoverLight;
	typedef CAStar<CDataStorage,CPathManagerLevelPositionDetour,u32,_dist_type>	CAStarLevelPositionDetour;

	CDataStorage					*m_data_storage;

	CPathManagerGame				m_game_path;
	CPathManagerLevelMinTime		m_level_path_min_time;
	CPathManagerLevelCoverLight		m_level_path_cover_light;
	CPathManagerLevelPositionDetour	m_level_path_position_detour;

	CAStarGame						m_game_search;
	CAStarLevelMinTime				m_level_search_min_time;
	CAStarLevelCoverLight			m_level_search_cover_light;
	CAStarLevelPositionDetour		m_level_search_position_detour;

public:

					CGraphSearchEngine		(u32 max_node_count)
	{
		m_data_storage		= xr_new<CDataStorage>(max_node_count,_dist_type(0),_dist_type(2000));
	}

	virtual			~CGraphSearchEngine		()
	{
		xr_delete			(m_data_storage);
	}

	// generic pathfinding
	template <typename _Graph>
	IC		bool	build_path				(
				const _Graph	&graph, 
				u32				start_node, 
				u32				dest_node, 
				xr_vector<u32>	&node_path
			)
	{
		typedef CPathManager<_Graph, CDataStorage,_dist_type,u32,u32>	CPathManagerGeneric;
		typedef CAStar<CDataStorage,CPathManagerGeneric,u32,_dist_type> CAStarGeneric;

		CPathManagerGeneric			path_manager;
		CAStarGeneric				search;

		VERIFY						(graph->get_nod_count() <= m_data_storage->get_max_node_count());

		path_manager.setup			(
			graph,
			m_data_storage,
			&node_path,
			start_node,
			dest_node
		);
		return						(search.find(*m_data_storage,path_manager));
	}

	// this template we need only for the template class specialization puposes
	template <typename _Graph>
	IC		bool	build_path		(
				u32				start_node, 
				u32				dest_node, 
				xr_vector<u32>	&node_path, 
				bool			consider_obstacles = true
			)
	{
		Debug.fatal					("Invalid graph search engine query!");
	}

	// game graph pathfinding
	template <>
	IC		bool	build_path<CGameGraph>	(
				u32				start_node, 
				u32				dest_node, 
				xr_vector<u32>	&node_path, 
				bool			consider_obstacles
			)
	{
		m_game_path.setup			(
			&ai().game_graph(),
			m_data_storage,
			&node_path,
			start_node,
			dest_node
		);
		return						(m_game_search.find(*m_data_storage,m_game_path));
	}

	// level graph min_time pathfinding
	template <>
	IC		bool	build_path<CLevelGraph>	(
				u32				start_node, 
				u32				dest_node, 
				xr_vector<u32>	&node_path, 
				bool			consider_obstacles
			)
	{
		m_level_path_min_time.setup			(
			&ai().level_graph(),
			m_data_storage,
			&node_path,
			start_node,
			dest_node
//			consider_obstacles
		);
		return						(m_level_search_min_time.find(*m_data_storage,m_level_path_min_time));
	}

	// level graph cover_light pathfinding
	IC		bool	build_path		(
				u32				start_node, 
				u32				dest_node, 
				xr_vector<u32>	&node_path, 
				bool			consider_obstacles, 
				float			light_weight,
				float			cover_weight,
				float			distance_weight
			)
	{
		m_level_path_min_time.setup			(
			&ai().level_graph(),
			m_data_storage,
			&node_path,
			start_node,
			dest_node
//			consider_obstacles,
//			light_weight,
//			cover_weight,
//			distance_weight
		);
		return						(m_level_search_cover_light.find(*m_data_storage,m_level_path_min_time));
	}

	// level graph position_detour pathfinding
	IC		bool	build_path		(
				u32				start_node, 
				u32				dest_node, 
				xr_vector<u32>	&node_path, 
				const Fvector	&enemy_position, 
				float			enemy_distance, 
				float			enemy_view_weight,
				bool			consider_obstacles, 
				float			light_weight,
				float			cover_weight,
				float			distance_weight
			)
	{
		m_level_path_min_time.setup			(
			&ai().level_graph(),
			m_data_storage,
			&node_path,
			start_node,
			dest_node
//			enemy_position,
//			enemy_distance,
//			enemy_view_weight,
//			consider_obstacles,
//			light_weight,
//			cover_weight,
//			distance_weight
		);
		return						(m_level_search_position_detour.find(*m_data_storage,m_level_path_min_time));
	}
};