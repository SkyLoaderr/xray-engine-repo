////////////////////////////////////////////////////////////////////////////
//	Module 		: abstract_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Abstract path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "restricted_object.h"

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class CAbstractPathManager {
private:
	const _Graph								*m_graph;
	_VertexEvaluator							*m_evaluator;

protected:
	_index_type									m_current_index;
	_index_type									m_intermediate_index;
	_vertex_id_type								m_dest_vertex_id;
	bool										m_actuality;
	bool										m_failed;
	xr_vector<_vertex_id_type>					m_path;
	CRestrictedObject							*m_object;

protected:
	IC	_vertex_id_type	intermediate_vertex_id		() const;
	IC			u32		intermediate_index			() const;

	IC	const xr_vector<_vertex_id_type> &path		() const;

	IC			void	build_path					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	IC	const _VertexEvaluator	*evaluator			() const;
	IC			void	make_inactual				();
	IC	virtual	void	before_search				(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	IC	virtual	void	after_search				();
	IC	virtual	bool	check_vertex				(const _vertex_id_type vertex_id) const;

public:
	IC					CAbstractPathManager		(CRestrictedObject *object);
	IC	virtual			~CAbstractPathManager		();
	IC			void	reinit						(const _Graph *graph = 0);
	IC			bool	actual						(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id) const;
	IC			void	set_evaluator				(_VertexEvaluator *evaluator);
	IC			void	set_dest_vertex				(const _vertex_id_type vertex_id);
	IC	_vertex_id_type	dest_vertex_id				() const;
	IC	virtual	bool	completed					() const;
	IC			bool	failed						() const;
	IC	virtual	void	select_intermediate_vertex	();
	IC	CRestrictedObject &object					() const;

	friend class CMovementManager;
};

#include "abstract_path_manager_inline.h"

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class CBasePathManager :
	public CAbstractPathManager<
		_Graph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> 
{
};