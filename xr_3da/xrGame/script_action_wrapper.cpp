////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_wrapper.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_wrapper.h"

void CScriptActionWrapper::reinit				(CLuaGameObject *object, CPropertyStorage *storage, bool clear_all)
{
	call_member<void>			("reinit",object,storage,clear_all);
}

void CScriptActionWrapper::reinit_static		(CScriptAction *action, CLuaGameObject *object, CPropertyStorage *storage, bool clear_all)
{
	action->CScriptAction::reinit		(object,storage,clear_all);
}

void CScriptActionWrapper::initialize			()
{
	call_member<void>			("initialize");
}

void CScriptActionWrapper::initialize_static	(CScriptAction *action)
{
	action->CScriptAction::initialize	();
}

void CScriptActionWrapper::execute				()
{
	call_member<void>			("execute");
}

void CScriptActionWrapper::execute_static		(CScriptAction *action)
{
	action->CScriptAction::execute		();
}

void CScriptActionWrapper::finalize				()
{
	call_member<void>			("finalize");
}

void CScriptActionWrapper::finalize_static		(CScriptAction *action)
{
	action->CScriptAction::finalize		();
}

CScriptActionWrapper::_edge_value_type CScriptActionWrapper::weight	(const CSConditionState &condition0, const CSConditionState &condition1)
{
	return								(call_member<_edge_value_type>("weight",condition0,condition1));
}

CScriptActionWrapper::_edge_value_type CScriptActionWrapper::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
	return								(const_cast<CScriptActionWrapper*>(this)->weight(condition0,condition1));
}

CScriptActionWrapper::_edge_value_type CScriptActionWrapper::weight_static	(CScriptAction *action, const CSConditionState &condition0, const CSConditionState &condition1)
{
	return								(action->CScriptAction::weight(condition0,condition1));
}
