////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_action_wrapper.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script motivation action wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action.h"
#include "script_space.h"

typedef CMotivationAction<CLuaGameObject> CScriptMotivationAction;

class CScriptMotivationActionWrapper : public CScriptMotivationAction {
public:
	typedef	CScriptMotivationAction	inherited;

protected:
	typedef CLuaGameObject			_object_type;

public:
	luabind::weak_ref		m_lua_instance;

public:
	IC						CScriptMotivationActionWrapper	(luabind::weak_ref lua_instance, const CGraphEngine::CWorldState &goal);
	virtual					~CScriptMotivationActionWrapper	();
	virtual void			reinit							(_object_type *object);
	static	void			reinit_static					(inherited *motivation, CLuaGameObject *object);
	virtual void			Load							(LPCSTR section);
	static	void			Load_static						(inherited *motivation, LPCSTR section);
	virtual void			reload							(LPCSTR section);
	static	void			reload_static					(inherited *motivation, LPCSTR section);
	virtual float			evaluate						(u32 sub_motivation_id);
	static	float			evaluate_static					(inherited *motivation, u32 sub_motivation_id);
};

#include "script_motivation_action_wrapper_inline.h"