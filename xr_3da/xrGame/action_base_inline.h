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
IC	CBaseAction::CActionBase		(const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects, _object_type *object, LPCSTR action_name) :
	inherited			(conditions,effects)
{
	init				(object,action_name);
}

TEMPLATE_SPECIALIZATION
IC	CBaseAction::CActionBase		(_object_type *object, LPCSTR action_name)
{
	init				(object,action_name);
}

TEMPLATE_SPECIALIZATION
CBaseAction::~CActionBase		()
{
}

TEMPLATE_SPECIALIZATION
void CBaseAction::init			(_object_type *object, LPCSTR action_name)
{
	m_object			= object;
#ifdef LOG_ACTION
	m_action_name		= action_name;
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateConstructed);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::Load			(LPCSTR section)
{
#ifdef LOG_ACTION
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateLoaded);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::reinit		(_object_type *object, bool clear_all)
{
	VERIFY				(object);
	m_object			= object;
	m_inertia_time		= 0;
#ifdef LOG_ACTION
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateReinitialized);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::reload			(LPCSTR section)
{
#ifdef LOG_ACTION
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateReloaded);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::initialize		()
{
#ifdef LOG_ACTION
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateInitialized);
#endif
	m_start_level_time	= Level().timeServer();
	m_start_game_time	= Level().GetGameTime();
}

TEMPLATE_SPECIALIZATION
void CBaseAction::execute		()
{
#ifdef LOG_ACTION
	if (xr_strlen(m_action_name))
		debug_log		(eActionStateExecuted);
#endif
}

TEMPLATE_SPECIALIZATION
void CBaseAction::finalize		()
{
#ifdef LOG_ACTION
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

#ifdef LOG_ACTION
TEMPLATE_SPECIALIZATION
IC	void CBaseAction::debug_log			(const EActionStates state_state) const
{
	switch (state_state) {
		case eActionStateConstructed : {
			Msg			("[%6d] action %s is constructed",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateLoaded : {
			Msg			("[%6d] action %s is loaded",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateReinitialized : {
			Msg			("[%6d] action %s is reinitialized",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateReloaded : {
			Msg			("[%6d] action %s is reloaded",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateInitialized : {
			Msg			("[%6d] action %s is initialized",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateExecuted : {
			Msg			("[%6d] action %s is executed",Level().timeServer(),m_action_name);
			break;
		}
		case eActionStateFinalized : {
			Msg			("[%6d] action %s is finalized",Level().timeServer(),m_action_name);
			break;
		}
		default : NODEFAULT;
	}
}
#endif

#undef TEMPLATE_SPECIALIZATION
#undef CBaseAction