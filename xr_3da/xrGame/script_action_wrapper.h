////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_wrapper.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"
#include "script_space.h"

class CLuaGameObject;

typedef CActionBase<CLuaGameObject> CScriptAction;

class CScriptActionWrapper : public CScriptAction, public luabind::wrap_base {
public:
	IC							CScriptActionWrapper(CLuaGameObject *object = 0, LPCSTR action_name = "");
	virtual void				reinit				(CLuaGameObject *object, CPropertyStorage *storage, bool clear_all);
	static	void				reinit_static		(CScriptAction *action, CLuaGameObject *object, CPropertyStorage *storage, bool clear_all);
	virtual void				initialize			();
	static	void				initialize_static	(CScriptAction *action);
	virtual void				execute				();
	static	void				execute_static		(CScriptAction *action);
	virtual void				finalize			();
	static	void				finalize_static		(CScriptAction *action);
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1);
	virtual _edge_value_type	weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
	static	_edge_value_type	weight_static		(CScriptAction *action, const CSConditionState &condition0, const CSConditionState &condition1);
};

#include "script_action_wrapper_inline.h"