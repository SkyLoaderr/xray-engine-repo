////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_params.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Path managers parameter structures
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace PathManagers {
	template <
		typename _dist_type,
		typename _index_type,
		typename _iteration_type
	>
	struct SBaseParameters {
		_index_type		max_visited_node_count;
		_dist_type		max_range;
		_iteration_type	max_iteration_count;

		IC	SBaseParameters(
				_index_type		max_visited_node_count = _index_type(-1),
				_dist_type		max_range = _dist_type(6000),
				_iteration_type	max_iteration_count = _iteration_type(-1)
			) :
			max_visited_node_count(max_visited_node_count),
			max_range(max_range),
			max_iteration_count(max_iteration_count)
		{
		}
	};

	template <
		typename _dist_type,
		typename _index_type,
		typename _iteration_type
	>
	struct SObstacleParams  : public SBaseParameters<
		_dist_type,
		_index_type,
		_iteration_type
	> {
		bool	avoid_dynamic_obstacles;

		IC	SObstacleParams (
				_index_type		max_visited_node_count = _index_type(-1),
				_dist_type		max_range = _dist_type(6000),
				_iteration_type	max_iteration_count = _iteration_type(-1),
				bool			avoid_dynamic_obstacles = true
			) :
			SBaseParameters(
				max_visited_node_count,
				max_range,
				max_iteration_count
			),
			avoid_dynamic_obstacles(avoid_dynamic_obstacles)
		{
		}
	};

	template <
		typename _dist_type,
		typename _index_type,
		typename _iteration_type
	>
	struct SObstaclesLightCover : public SObstacleParams<
		_dist_type,
		_index_type,
		_iteration_type
	> {
		float	light_weight;
		float	cover_weight;
		float	distance_weight;

		IC SObstaclesLightCover(
				_index_type		max_visited_node_count = _index_type(-1),
				_dist_type		max_range = _dist_type(6000),
				_iteration_type	max_iteration_count = _iteration_type(-1),
				bool			avoid_dynamic_obstacles = true,
				float			light_weight = 10.f,
				float			cover_weight = 20.f,
				float			distance_weight = 40.f
			) :
			SObstacleParams (
				max_visited_node_count,
				max_range,
				max_iteration_count,
				avoid_dynamic_obstacles
			),
			light_weight(light_weight),
			cover_weight(cover_weight),
			distance_weight(distance_weight)
		{
		}
	};

	template <
		typename _dist_type,
		typename _index_type,
		typename _iteration_type
	>
	struct SObstaclesLightCoverEnemy : public SObstaclesLightCover<
		_dist_type,
		_index_type,
		_iteration_type
	> {
		const Fvector			&enemy_position;
		float					enemy_distance;
		float					enemy_view_weight;

		IC	SObstaclesLightCoverEnemy (
				_index_type		max_visited_node_count = _index_type(-1),
				_dist_type		max_range = _dist_type(6000),
				_iteration_type	max_iteration_count = _iteration_type(-1),
				bool			avoid_dynamic_obstacles = true,
				float			light_weight = 10.f,
				float			cover_weight = 20.f,
				float			distance_weight = 40.f,
				const Fvector	&enemy_position = Fvector().set(0,0,0),
				float			enemy_distance = 30.f,
				float			enemy_view_weight = 100.f
			) : 
			SObstaclesLightCover(
				max_visited_node_count,
				max_range,
				max_iteration_count,
				avoid_dynamic_obstacles,
				light_weight,
				cover_weight,
				distance_weight
			),
			enemy_position(enemy_position), 
			enemy_distance(enemy_distance), 
			enemy_view_weight(enemy_view_weight)
		{
		}
	};
};

