////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_state_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager state inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateManagerAbstract CStateManagerState<_Object>

TEMPLATE_SPECIALIZATION
CStateManagerAbstract::CStateManagerState	()
{
	Init					();
}

TEMPLATE_SPECIALIZATION
CStateManagerAbstract::~CStateManagerState	()
{
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::Init			()
{
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::Load			(LPCSTR section)
{
	CStateBase<_Object>::Load		(section);
	CSStateManagerAbstract::Load	(section);
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data().m_state->Load(section);
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::reinit			(_Object *object, u32 state_id)
{
	CStateBase<_Object>::reinit		(object);
	CSStateManagerAbstract::reinit	(state_id);
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data().m_state->reinit(object);
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::reload			(LPCSTR section)
{
	CStateBase<_Object>::reload		(section);
	CSStateManagerAbstract::reload	(section);
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data().m_state->reload(section);
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::update			(u32 time_delta)
{
	execute							();
	CSStateManagerAbstract::update	(time_delta);
	CSStateManagerAbstract			*state_manager_abstract = dynamic_cast<CSStateManagerAbstract*>(&current_state());
	if (state_manager_abstract)
		state_manager_abstract->update(time_delta);
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::initialize		()
{
	CStateBase<_Object>::initialize	();
	current_state().initialize		();
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::finalize		()
{
	CStateBase<_Object>::finalize	();
	current_state().finalize		();
}

TEMPLATE_SPECIALIZATION
bool CStateManagerAbstract::completed		()
{
	return							(current_state().completed());
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::execute	()
{
	if (current_vertex_id() == dest_vertex_id()) {
		current_state().execute		();
		return;
	}

	if (path().size() < 2)
		return;

	for (;;) {
		if (!state(path().front()).completed()) {
			state(path().front()).finalize();
			return;
		}
		go_path			();
		state(path().front()).initialize();
		if (path().size() < 2) {
			go_path			();
			break;
		}
	}
	current_state().execute();
}

TEMPLATE_SPECIALIZATION
IC	xr_vector<u32> &CStateManagerAbstract::sequence()
{
	return					(path());
}

TEMPLATE_SPECIALIZATION
IC	const xr_vector<u32> &CStateManagerAbstract::sequence() const
{
	return					(path());
}

TEMPLATE_SPECIALIZATION
IC	CStateBase<_Object>	&CStateManagerAbstract::current_state	()
{
	VERIFY					(graph().vertex(current_vertex_id()));
	return					(*graph().vertex(current_vertex_id())->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	const CStateBase<_Object>	&CStateManagerAbstract::current_state	() const
{
	VERIFY					(graph().vertex(current_vertex_id()));
	return					(*graph().vertex(current_vertex_id())->data().m_state);
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateManagerAbstract
