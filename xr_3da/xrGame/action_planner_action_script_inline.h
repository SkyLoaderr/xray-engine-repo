////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_action_script.h
//	Created 	: 07.07.2004
//  Modified 	: 07.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner action with script support
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION		template <typename _object_type>
#define CSActionPlannerActionScript	CActionPlannerActionScript<_object_type>

TEMPLATE_SPECIALIZATION
IC	CSActionPlannerActionScript::CActionPlannerActionScript		(const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects, _object_type *object, LPCSTR action_name) :
	inherited			(conditions,effects,object ? object->lua_game_object() : 0,action_name)
{
	m_object			= object;
}

TEMPLATE_SPECIALIZATION
IC	CSActionPlannerActionScript::CActionPlannerActionScript		(_object_type *object, LPCSTR action_name) :
	inherited			(object ? object->lua_game_object() : 0,action_name)
{
	m_object			= object;
}

TEMPLATE_SPECIALIZATION
CSActionPlannerActionScript::~CActionPlannerActionScript		()
{
}

TEMPLATE_SPECIALIZATION
void CSActionPlannerActionScript::reinit		(_object_type *object, CPropertyStorage *storage, bool clear_all)
{
	VERIFY				(object);
	VERIFY				(m_object);
}

TEMPLATE_SPECIALIZATION
void CSActionPlannerActionScript::reinit		(CScriptGameObject *object, CPropertyStorage *storage, bool clear_all)
{
	VERIFY				(object);
	inherited::reinit	(object,storage,clear_all);
	m_object			= smart_cast<_object_type*>(object->object());
	VERIFY				(m_object);
	reinit				(m_object,storage,clear_all);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSActionPlannerActionScript