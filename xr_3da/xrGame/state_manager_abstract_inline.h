////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_abstract_inline.h
//	Created 	: 12.01.2004
//  Modified 	: 12.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager abstract inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename T\
>

#define CAbstractStateManager CStateManagerAbstract<T>

TEMPLATE_SPECIALIZATION
CAbstractStateManager::CStateManagerAbstract		()
{
	Init					();
}

TEMPLATE_SPECIALIZATION
CAbstractStateManager::~CStateManagerAbstract		()
{
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::Init			()
{
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::Load			(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::reinit			(const u32 start_vertex_id)
{
	inherited::reinit		(start_vertex_id);
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::reload			(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::add_state		(T *state, u32 state_id, u32 priority)
{
	VERIFY					(!graph().vertex(state_id));
	graph().add_vertex		(CState(state,priority),state_id);
	VERIFY					(graph().vertex(state_id));
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractStateManager::add_transition	(u32 state_id0, u32 state_id1, float weight)
{
	graph().add_edge		(state_id0,state_id1,weight);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractStateManager::add_transition	(u32 state_id0, u32 state_id1, float weight0, float weight1)
{
	graph().add_edge		(state_id0,state_id1,weight0,weight1);
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::remove			(u32 state_id)
{
	VERIFY					(graph().vertex(state_id));
	graph().vertex(state_id)->data().destroy();
	graph().vertex(state_id)->destroy();
	graph().remove_vertex	(state_id);
	VERIFY					(!graph().vertex(state_id));
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::update					(u32 time_delta)
{
	inherited::update		(time_delta);
}

TEMPLATE_SPECIALIZATION
IC	T	&CAbstractStateManager::state		(const u32 state_id)
{
	VERIFY					(graph().vertex(state_id));
	VERIFY					(graph().vertex(state_id)->data().m_state);
	return					(*graph().vertex(state_id)->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	T	&CAbstractStateManager::current_state	()
{
	VERIFY					(graph().vertex(current_vertex_id()));
	return					(*graph().vertex(current_vertex_id())->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	const T	&CAbstractStateManager::current_state	() const
{
	VERIFY					(graph().vertex(current_vertex_id()));
	return					(*graph().vertex(current_vertex_id())->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	T	&CAbstractStateManager::dest_state	()
{
	VERIFY					(graph().vertex(dest_vertex_id()));
	return					(*graph().vertex(dest_vertex_id())->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	const T	&CAbstractStateManager::dest_state	() const
{
	VERIFY					(graph().vertex(dest_vertex_id()));
	return					(*graph().vertex(dest_vertex_id())->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractStateManager::set_dest_state	(const u32 dest_state_id)
{
	set_dest_vertex_id		(dest_state_id);
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractObjectManager
