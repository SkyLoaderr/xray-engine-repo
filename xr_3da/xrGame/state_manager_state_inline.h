////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_state_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager state inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	xr_vector<u32> &CStateManagerState::sequence()
{
	return					(path());
}

IC	const xr_vector<u32> &CStateManagerState::sequence() const
{
	return					(path());
}

IC	CStateBase	&CStateManagerState::current_state	()
{
	VERIFY					(graph().vertex(current_vertex_id()));
	return					(*graph().vertex(current_vertex_id())->data().m_state);
}

IC	const CStateBase	&CStateManagerState::current_state	() const
{
	VERIFY					(graph().vertex(current_vertex_id()));
	return					(*graph().vertex(current_vertex_id())->data().m_state);
}
