////////////////////////////////////////////////////////////////////////////
//	Module 		: level_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_path_manager.h"
#include "level_graph.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class 
	CBasePathManager<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> :
	public CAbstractPathManager<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	>,
	virtual public CAI_ObjectLocation
{
	typedef CAbstractPathManager<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> inherited;
private:
	friend class CMovementManager;

protected:
	IC			bool	actual						() const;
	IC			void	build_path					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id, bool use_selector_path = false);
	IC	virtual	void	before_search				(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	IC	virtual	void	after_search				();
	IC	virtual	bool	check_vertex				(const _vertex_id_type vertex_id) const;

public:
	IC			void	reinit						(const CLevelGraph *graph = 0);
};

#include "level_path_manager_inline.h"
