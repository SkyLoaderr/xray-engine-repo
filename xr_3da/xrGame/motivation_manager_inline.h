////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_manager_inline.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<typename _motivation_type>
#define CSMotivationManager		CMotivationManager<_motivation_type>

TEMPLATE_SPECIALIZATION
IC	CSMotivationManager::CMotivationManager		()
{
	init					();
}

TEMPLATE_SPECIALIZATION
IC	CSMotivationManager::~CMotivationManager	()
{
	xr_delete				(m_graph)
}

TEMPLATE_SPECIALIZATION
IC		void					init				();
TEMPLATE_SPECIALIZATION
{
	m_graph					= xr_new<CSGraphAbstract>();
}

virtual void					reinit				();
TEMPLATE_SPECIALIZATION
{
	m_graph					= 0;
}

virtual void					Load				(LPCSTR section);
TEMPLATE_SPECIALIZATION
{
}

virtual void					reload				(LPCSTR section);
{
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::add_motivation	(u32 motivation_id, _motivation_type *motivation);
{
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::remove_motivation	(u32 motivation_id);
{
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::add_connection	(u32 motivation_id, u32 sub_motivation_id);
{
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::remove_connection	(u32 motivation_id, u32 sub_motivation_id);
{
}

TEMPLATE_SPECIALIZATION
IC	const _motivation_type *CSMotivationManager::motivation	(u32 motivation_id) const;
{
	return				(&m_graph->vertex(m_selected_id)->data());
}

TEMPLATE_SPECIALIZATION
IC	const _motivation_type *CSMotivationManager::selected	() const;
{
	xr_map<u32,float>::const_iterator	I = m_actions.find(m_selected_id);
	VERIFY				(m_actions.end() != I);
	return				(motivation(m_selected_id));
}

TEMPLATE_SPECIALIZATION
IC	u32						selected_id			() const;
{
	return				(m_selected_id);
}

TEMPLATE_SPECIALIZATION
void CSMotivationManager::update				(u32 time_delta)
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMotivationManager
