////////////////////////////////////////////////////////////////////////////
//	Module 		: state_base_inline.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Base state inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBaseAbstract CStateBase<_Object>

TEMPLATE_SPECIALIZATION
CStateBaseAbstract::CStateBase			()
{
	Init				();
}

TEMPLATE_SPECIALIZATION
CStateBaseAbstract::~CStateBase			()
{
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::Init			()
{
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::Load			(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::reinit			(_Object *object)
{
	VERIFY				(object);
	m_object			= object;
	m_inertia_time		= 0;
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::reload			(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::initialize		()
{
	m_start_level_time	= Level().timeServer();
	m_start_game_time	= Level().GetGameTime();
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::execute		()
{
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::finalize		()
{
	m_inertia_time		= 0;
}

TEMPLATE_SPECIALIZATION
bool CStateBaseAbstract::completed		() const
{
	return				(m_start_level_time + m_inertia_time <= Level().timeServer());
}

TEMPLATE_SPECIALIZATION
IC	u32	CStateBaseAbstract::start_level_time			() const
{
	return				(m_start_level_time);
}

TEMPLATE_SPECIALIZATION
IC	ALife::_TIME_ID	CStateBaseAbstract::start_game_time	() const
{
	return				(m_start_game_time);
}

TEMPLATE_SPECIALIZATION
IC	u32	CStateBaseAbstract::inertia_time				() const
{
	return				(m_inertia_time);
}

TEMPLATE_SPECIALIZATION
IC	void CStateBaseAbstract::set_inertia_time			(u32 inertia_time)
{
	m_inertia_time		= inertia_time;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBaseAbstract
