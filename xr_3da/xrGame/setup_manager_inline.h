////////////////////////////////////////////////////////////////////////////
//	Module 		: setup_manager_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Setup manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _action_type,\
		typename _object_type,\
		typename _action_id_type\
	>

#define CSSetupManager CSetupManager<_action_type,_object_type,_action_id_type>

TEMPLATE_SPECIALIZATION
IC	CSSetupManager::CSetupManager					()
{
	init					();
}

TEMPLATE_SPECIALIZATION
CSSetupManager::~CSetupManager						()
{
	clear					();
}

TEMPLATE_SPECIALIZATION
IC	void CSSetupManager::init						()
{
	m_object				= 0;
}

TEMPLATE_SPECIALIZATION
void CSSetupManager::reinit							(_object_type *object)
{
	VERIFY					(object);
	clear					();
	m_current_action_id		= _action_id_type(-1);
	m_object				= object;
	m_actuality				= false;
}

TEMPLATE_SPECIALIZATION
IC	_action_type &CSSetupManager::action			(const _action_id_type &action_id) const
{
	xr_map<_action_id_type,_action_type*>::const_iterator	I = m_actions.find(action_id);
	VERIFY					(I != m_actions.end());
	return					(*(*I).second);
}

TEMPLATE_SPECIALIZATION
IC	_action_type &CSSetupManager::current_action	() const
{
	return					(action(current_action_id()));
}

TEMPLATE_SPECIALIZATION
IC	const _action_id_type &CSSetupManager::current_action_id	() const
{
	return					(m_current_action_id);
}

TEMPLATE_SPECIALIZATION
IC	void CSSetupManager::clear						()
{
	m_actuality				= false;
	xr_map<_action_id_type,_action_type*>::iterator	I = m_actions.begin();
	xr_map<_action_id_type,_action_type*>::iterator	E = m_actions.end();
	for ( ; I != E; ++I)
		xr_delete			((*I).second);

	m_actions.clear			();
}

TEMPLATE_SPECIALIZATION
IC	void CSSetupManager::add_action					(const _action_id_type &action_id, _action_type *action)
{
	m_actuality				= false;
	VERIFY					(action);
	VERIFY					(m_actions.find(action_id) == m_actions.end());
	action->set_object		(m_object);
	m_actions.insert		(std::make_pair(action_id,action));
}

TEMPLATE_SPECIALIZATION
void CSSetupManager::update							(u32 time_delta)
{
	if (m_actions.empty())
		return;
	select_action			();
	current_action().execute();
}

TEMPLATE_SPECIALIZATION
IC	void CSSetupManager::select_action				()
{
	if (!m_actuality || current_action().completed()) {
		m_actuality			= true;
		if (m_actions.size() == 1) {
			m_current_action_id = (*m_actions.begin()).first;
			(*m_actions.begin()).second->initialize();
			return;
		}

		float				m_total_weight = 0.f;
		xr_map<_action_id_type,_action_type*>::const_iterator	I = m_actions.begin();
		xr_map<_action_id_type,_action_type*>::const_iterator	E = m_actions.end();
		for ( ; I != E; ++I)
			if (((*I).first != m_current_action_id) && (*I).second->applicable())
				m_total_weight += (*I).second->weight();
		VERIFY				(!fis_zero(m_total_weight));

		float				m_random = ::Random.randF(m_total_weight);
		m_total_weight		= 0.f;
		I					= m_actions.begin();
		for ( ; I != E; ++I) {
			if (((*I).first != m_current_action_id) && (*I).second->applicable())
				m_total_weight += (*I).second->weight();
			else
				continue;
			if (m_total_weight > m_random) {
				if (m_actions.find(m_current_action_id) != m_actions.end())
					current_action().finalize();
				m_current_action_id = (*I).first;
				(*I).second->initialize();
				break;
			}
		}
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CSSetupManager