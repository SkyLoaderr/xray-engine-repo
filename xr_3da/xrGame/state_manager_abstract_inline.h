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
	while (!m_states.empty())
		remove				(m_states.begin()->first);
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
void CAbstractStateManager::reinit			()
{
	m_cur_state				= u32(-1);
	m_dest_state			= u32(-1);
	m_actuality				= true;
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::reload			(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::add				(T *state, u32 state_id, u32 priority)
{
	xr_map<u32,CState>::const_iterator	I = m_states.find(state_id);
	VERIFY								(m_states.end() == I);
	std::pair<xr_map<u32,CState>::iterator,bool>	J = m_states.insert(std::make_pair(state_id,CState(0,priority)));
	J.first->second.m_state				= state;
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::remove			(u32 state_id)
{
	xr_map<u32,CState>::iterator	I = m_states.find(state_id);
	VERIFY							(m_states.end() != I);
	m_states.erase					(I);
}

TEMPLATE_SPECIALIZATION
IC	u32	CAbstractStateManager::current_state		() const
{
	return					(m_cur_state);
}

TEMPLATE_SPECIALIZATION
IC	u32	CAbstractStateManager::dest_state			() const
{
	return					(m_dest_state);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractStateManager::set_dest_state		(u32 state_id)
{
	m_actuality							= m_actuality && (m_dest_state == state_id);
	xr_map<u32,CState>::const_iterator	I = m_states.find(state_id);
	VERIFY								(m_states.end() != I);
	m_dest_state						= state_id;
}

TEMPLATE_SPECIALIZATION
void CAbstractStateManager::update					(u32 time_delta)
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractObjectManager
