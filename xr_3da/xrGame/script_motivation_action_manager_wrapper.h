////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_action_manager_wrapper.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script manager action manager wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action_manager.h"
#include "script_space.h"

typedef CMotivationActionManager<CLuaGameObject> CScriptMotivationActionManager;

class CScriptMotivationActionManagerWrapper : public CScriptMotivationActionManager {
protected:
	typedef CLuaGameObject	_object_type;
	typedef	CScriptMotivationActionManager	inherited;

public:
	luabind::weak_ref		m_lua_instance;

public:
	IC						CScriptMotivationActionManagerWrapper	(luabind::weak_ref lua_instance);
	virtual					~CScriptMotivationActionManagerWrapper	();
	virtual void			reinit									(_object_type *object, bool clear_all = false);
	static	void			reinit_static							(CScriptMotivationActionManager *manager, CLuaGameObject *object, bool clear_all = false);
	virtual void			Load									(LPCSTR section);
	static	void			Load_static								(CScriptMotivationActionManager *manager, LPCSTR section);
	virtual void			reload									(LPCSTR section);
	static	void			reload_static							(CScriptMotivationActionManager *manager, LPCSTR section);
	virtual void			update									(u32 time_delta);
	static	void			update_static							(CScriptMotivationActionManager *manager, u32 time_delta);
};

#include "script_motivation_action_manager_wrapper_inline.h"