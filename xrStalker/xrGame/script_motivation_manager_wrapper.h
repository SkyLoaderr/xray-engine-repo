////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_manager_wrapper.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script manager manager wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_manager.h"
#include "script_space.h"

typedef CMotivationManager<CLuaGameObject> CScriptMotivationManager;

class CScriptMotivationManagerWrapper : public CScriptMotivationManager {
protected:
	typedef CLuaGameObject				_object_type;
	typedef	CScriptMotivationManager	inherited;

public:
	luabind::object			m_lua_instance;

public:
	IC						CScriptMotivationManagerWrapper	(const luabind::object &lua_instance);
	virtual					~CScriptMotivationManagerWrapper();
	virtual void			reinit							(_object_type *object, bool clear_all = false);
	static	void			reinit_static					(CScriptMotivationManager *manager, CLuaGameObject *object, bool clear_all = false);
	virtual void			Load							(LPCSTR section);
	static	void			Load_static						(CScriptMotivationManager *manager, LPCSTR section);
	virtual void			reload							(LPCSTR section);
	static	void			reload_static					(CScriptMotivationManager *manager, LPCSTR section);
	virtual void			update							(u32 time_delta);
	static	void			update_static					(CScriptMotivationManager *manager, u32 time_delta);
};

#include "script_motivation_manager_wrapper_inline.h"