////////////////////////////////////////////////////////////////////////////
//	Module 		: action_base_inline.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Base action inline function
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _object_type>
#define CBaseAction				CActionBase<_object_type>

TEMPLATE_SPECIALIZATION
CBaseAction::CActionBase		(LPCSTR action_name)
{
	init				(action_name);
}

TEMPLATE_SPECIALIZATION
CBaseAction::~CActionBase		()
{
}

TEMPLATE_SPECIALIZATION
void CBaseAction::init			(LPCSTR action_name)
{
	m_object			= 0;
#ifdef LOG_STATE
	m_action_name		= action_name;
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateConstructed);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::Load			(LPCSTR section)
{
#ifdef LOG_STATE
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateLoaded);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::reinit		(_object_type *object)
{
	VERIFY				(object);
	m_object			= object;
	m_inertia_time		= 0;
#ifdef LOG_STATE
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateReinitialized);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::reload			(LPCSTR section)
{
#ifdef LOG_STATE
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateReloaded);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::initialize		()
{
#ifdef LOG_STATE
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateInitialized);
#endif
	m_start_level_time	= Level().timeServer();
	m_start_game_time	= Level().GetGameTime();
}

TEMPLATE_SPECIALIZATION
void CBaseAction::execute		()
{
#ifdef LOG_STATE
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateExecuted);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::finalize		()
{
#ifdef LOG_STATE
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateFinalized);
#endif
}

TEMPLATE_SPECIALIZATION
bool CBaseAction::completed		() const
{
	return				(m_start_level_time + m_inertia_time <= Level().timeServer());
}

TEMPLATE_SPECIALIZATION
IC	u32	CBaseAction::start_level_time			() const
{
	return				(m_start_level_time);
}

TEMPLATE_SPECIALIZATION
IC	ALife::_TIME_ID	CBaseAction::start_game_time	() const
{
	return				(m_start_game_time);
}

TEMPLATE_SPECIALIZATION
IC	u32	CBaseAction::inertia_time				() const
{
	return				(m_inertia_time);
}

TEMPLATE_SPECIALIZATION
IC	void CBaseAction::set_inertia_time			(u32 inertia_time)
{
	m_inertia_time		= inertia_time;
}


TEMPLATE_SPECIALIZATION
IC	u32	CBaseAction::priority			() const
{
	return				(m_priority);
}

#ifdef LOG_STATE
TEMPLATE_SPECIALIZATION
IC	void CBaseAction::debug_log			(const EStateActions state_state) const
{
	switch (state_state) {
		case eActionStateConstructed : {
			Msg			("[%6d] state %s is constructed",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateLoaded : {
			Msg			("[%6d] state %s is loaded",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateReinitialized : {
			Msg			("[%6d] state %s is reinitialized",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateReloaded : {
			Msg			("[%6d] state %s is reloaded",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateInitialized : {
			Msg			("[%6d] state %s is initialized",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateExecuted : {
			Msg			("[%6d] state %s is executed",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateFinalized : {
			Msg			("[%6d] state %s is finalized",Level().timeServer(),m_action_name);
			break;
		}
		default : NODEFAULT;
	}
}
#endif

#undef TEMPLATE_SPECIALIZATION
#undef CBaseAction