////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_manager_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Abstract graph manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_space.h"
#include "graph_engine.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Data,\
	typename _edge_weight_type,\
	typename _vertex_id_type,\
	typename _vertex_index_type\
>

#define CAbstractGraphManager CGraphManagerAbstract<_Data,_edge_weight_type,_vertex_id_type,_vertex_index_type>

TEMPLATE_SPECIALIZATION
IC	CAbstractGraphManager::CGraphManagerAbstract	()
{
	Init					();
}

TEMPLATE_SPECIALIZATION
CAbstractGraphManager::~CGraphManagerAbstract		()
{
}

TEMPLATE_SPECIALIZATION
void CAbstractGraphManager::Init					()
{
}

TEMPLATE_SPECIALIZATION
void CAbstractGraphManager::reinit					(const _vertex_id_type start_vertex_id)
{
	VERIFY					(graph().vertex(start_vertex_id));
	m_current_vertex_id		= m_dest_vertex_id = start_vertex_id;
	m_path.clear			();
	m_actuality				= true;
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraphManager::set_dest_vertex_id	(const _vertex_id_type dest_vertex_id)
{
	m_actuality				= m_actuality && (m_dest_vertex_id == dest_vertex_id);
	m_dest_vertex_id		= dest_vertex_id;
}

TEMPLATE_SPECIALIZATION
IC	const _vertex_id_type &CAbstractGraphManager::dest_vertex_id() const
{
	return					(m_dest_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC	const _vertex_id_type &CAbstractGraphManager::current_vertex_id() const
{
	return					(m_current_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractGraphManager::actual	() const
{
	return					(m_actuality);
}

TEMPLATE_SPECIALIZATION
void CAbstractGraphManager::update	(u32 time_delta)
{
	if (m_actuality)
		return;
	m_actuality				= true;
	m_path.clear			();
	bool					successfull = ai().graph_engine().search(m_graph,current_vertex_id(),dest_vertex_id(),&m_path,CGraphEngine::CBaseParameters());
	VERIFY					(successfull);
}

TEMPLATE_SPECIALIZATION
IC	const xr_vector<_vertex_id_type> &CAbstractGraphManager::path() const
{
	return					(m_path);
}

TEMPLATE_SPECIALIZATION
IC	xr_vector<_vertex_id_type> &CAbstractGraphManager::path()
{
	return					(m_path);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraphManager::go_path()
{
	VERIFY					(!path().empty());
	m_path.erase			(m_path.begin());
	if (!m_path.empty())
		m_current_vertex_id	= m_path.front();
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractGraphManager