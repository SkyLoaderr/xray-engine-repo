////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_manager_inline.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template<\
		typename _object_type,\
		template <typename _object_type> class _motivation_type,\
		template <typename _object_type> class _motivation_action_type\
	>
#define CSMotivationManager		CMotivationManager<_object_type,_motivation_type,_motivation_action_type>

TEMPLATE_SPECIALIZATION
IC	CSMotivationManager::CMotivationManager		()
{
	init					();
}

TEMPLATE_SPECIALIZATION
IC	CSMotivationManager::~CMotivationManager	()
{
	xr_vector<CSGraphAbstract::CVertex>::iterator	I = m_graph->vertices().begin();
	xr_vector<CSGraphAbstract::CVertex>::iterator	E = m_graph->vertices().end();
	for ( ; I != E; ++I)
		xr_delete			((*I).data());
	xr_delete				(m_graph);
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::init				()
{
	m_graph					= xr_new<CSGraphAbstract>();
	m_edges.reserve			(16);
	m_actions.reserve		(100);
}

TEMPLATE_SPECIALIZATION
void CSMotivationManager::reinit				(_object_type *object, bool clear_all)
{
	if (clear_all) {
		xr_delete			(m_graph);
		m_graph				= xr_new<CSGraphAbstract>();
		m_motivations.clear	();
		m_actions.clear		();
	}
	
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data()->reinit(object);

	m_selected_id			= u32(-1);
	m_actuality				= false;
}

TEMPLATE_SPECIALIZATION
void CSMotivationManager::Load					(LPCSTR section)
{
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data()->Load(section);
}

TEMPLATE_SPECIALIZATION
void CSMotivationManager::reload				(LPCSTR section)
{
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data()->reload(section);
}

TEMPLATE_SPECIALIZATION
IC	typename CSMotivationManager::CSGraphAbstract &CSMotivationManager::graph	()
{
	return					(*m_graph);
}

TEMPLATE_SPECIALIZATION
IC	const typename CSMotivationManager::CSGraphAbstract &CSMotivationManager::graph	() const
{
	return					(*m_graph);
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::add_motivation	(u32 motivation_id, CSMotivation *motivation)
{
	VERIFY					(!graph().vertex(motivation_id));
	graph().add_vertex		(motivation,motivation_id);
	VERIFY					(graph().vertex(motivation_id));
	m_actuality				= false;
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::remove_motivation	(u32 motivation_id)
{
	VERIFY					(graph().vertex(motivation_id));
	graph().vertex(motivation_id)->data()->destroy();
	graph().vertex(motivation_id)->destroy();
	graph().remove_vertex	(motivation_id);
	VERIFY					(!graph().vertex(motivation_id));
	m_actuality				= false;
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::add_connection	(u32 motivation_id, u32 sub_motivation_id)
{
	graph().add_edge		(motivation_id,sub_motivation_id,0.f);
	m_actuality				= false;
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::remove_connection	(u32 motivation_id, u32 sub_motivation_id)
{
	graph().remove_edge		(motivation_id,sub_motivation_id);
	m_actuality				= false;
}

TEMPLATE_SPECIALIZATION
IC	typename CSMotivationManager::CSMotivation *CSMotivationManager::motivation	(u32 motivation_id) const
{
	return				(m_graph->vertex(m_selected_id)->data());
}

TEMPLATE_SPECIALIZATION
IC	typename CSMotivationManager::CSMotivationAction *CSMotivationManager::selected	() const
{
	xr_vector<CMotivationWeight>::const_iterator	I = std::find(m_actions.begin(),m_actions.end(),m_selected_id);
	VERIFY				(m_actions.end() != I);
	VERIFY				(smart_cast<CSMotivationAction*>(motivation(m_selected_id)));
	return				(static_cast<CSMotivationAction*>(motivation(m_selected_id)));
}

TEMPLATE_SPECIALIZATION
IC	u32	CSMotivationManager::selected_id		() const
{
	return				(m_selected_id);
}

TEMPLATE_SPECIALIZATION
IC	bool CSMotivationManager::actual			() const
{
	return				(m_actuality);
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::update			()
{
	if (!actual()) {
		update_motivations	();
		m_actuality			= true;
	}

	select_action			();

	selected()->update		();
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::update_motivations()
{
	m_temp.clear			();
	m_motivations.clear		();
	
	xr_vector<CSGraphAbstract::CVertex>::const_iterator	I = graph().vertices().begin();
	xr_vector<CSGraphAbstract::CVertex>::const_iterator	E = graph().vertices().end();
	for ( ; I != E; ++I) {
		xr_vector<CSGraphAbstract::CEdge>::const_iterator i = (*I).edges().begin();
		xr_vector<CSGraphAbstract::CEdge>::const_iterator e = (*I).edges().end();
		for ( ; i != e; ++i)
			m_temp.insert	((*i).vertex_id());
	}
	
	I = graph().vertices().begin();
	for ( ; I != E; ++I)
		if (m_temp.find((*I).vertex_id()) == m_temp.end())
			m_motivations.push_back	((*I).vertex_id());
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::select_action		()
{
	// clearing previous results
	m_actions.clear					();

	// propagating motivations
	{
		xr_vector<u32>::iterator	I = m_motivations.begin();
		xr_vector<u32>::iterator	E = m_motivations.end();
		for ( ; I != E; ++I)
			propagate	(*I,1.f);
	}

	// finding the best action
	{
		float						max = -1.f;
		m_selected_id				= u32(-1);
		xr_vector<CMotivationWeight>::iterator	I = m_actions.begin();
		xr_vector<CMotivationWeight>::iterator	E = m_actions.end();
		for ( ; I != E; ++I)
			if ((*I).m_motivation_weight > max) {
				m_selected_id		= (*I).m_motivation_id;
				max					= (*I).m_motivation_weight;
			}
		VERIFY						(m_selected_id != u32(-1));
	}
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationManager::propagate	(u32 motivation_id, float weight)
{
	CSGraphAbstract::CVertex	*vertex	= graph().vertex(motivation_id);
	xr_vector<CSGraphAbstract::CEdge>::const_iterator	I = vertex->edges().begin(), B = I;
	xr_vector<CSGraphAbstract::CEdge>::const_iterator	E = vertex->edges().end();

	if (I == E) {
		xr_vector<CMotivationWeight>::iterator	i = std::find(m_actions.begin(),m_actions.end(),motivation_id);
		if (m_actions.end() == i)
			m_actions.push_back			(CMotivationWeight(motivation_id,weight));
		else
			(*i).m_motivation_weight	+= weight;
		return;
	}

	float				total_value	= 0.f;
	m_edges.resize		(vertex->edges().size());
	xr_vector<float>::iterator	J = m_edges.begin();
	for ( ; I != E; ++I, ++J) {
		*J				= vertex->data()->evaluate((*I).vertex_id());
		total_value		+= *J;
	}

	if (total_value >= 1.f + EPS_L)
		total_value		= weight/total_value;
	else
		total_value		= weight;

	J					= m_edges.begin();
	for (I = B; I != E; ++I, ++J)
		if (!fis_zero((*J)*total_value))
			propagate	((*I).vertex_id(),(*J)*total_value);
}

TEMPLATE_SPECIALIZATION
void CSMotivationManager::clear	()
{
	xr_vector<CSGraphAbstract::CVertex>::iterator	I = m_graph->vertices().begin();
	xr_vector<CSGraphAbstract::CVertex>::iterator	E = m_graph->vertices().end();
	for ( ; I != E; ++I)
		xr_delete			((*I).data());
	xr_delete				(m_graph);
	m_graph					= xr_new<CSGraphAbstract>();
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMotivationManager