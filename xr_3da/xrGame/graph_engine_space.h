////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_engine_space.h
//	Created 	: 21.03.2002
//  Modified 	: 26.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Graph engine
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _condition_type,
	typename _value_type
>
class COperatorConditionAbstract;

template <
	typename _world_property
>
class CConditionState;

template <
	typename _world_property,
	typename _edge_value_type
>
class COperatorAbstract;

namespace GraphEngineSpace {
	typedef float		_dist_type;
	typedef u32			_index_type;
	typedef u32			_iteration_type;
	typedef u16			_solver_dist_type;
	typedef u32			_solver_condition_type;
	typedef bool		_solver_value_type;

	typedef xr_map<
				_solver_condition_type,
				_solver_value_type
			>			CSolverConditionStorage;

	typedef COperatorConditionAbstract<
				_solver_condition_type,
				_solver_value_type
			>			CWorldProperty;

	typedef CConditionState<
				CWorldProperty
			>			CWorldState;

	typedef COperatorAbstract<
				CWorldProperty,
				_solver_dist_type
			>			CWorldOperator;

	typedef CWorldState	_solver_index_type;
	typedef u32			_solver_edge_type;
};
