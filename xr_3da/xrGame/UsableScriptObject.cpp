#include "stdafx.h"
#include "UsableScriptObject.h"
#include "script_space.h"
#include "script_callback.h"
#include "GameObject.h"
#include "script_game_object.h"

using namespace luabind;

CUsableScriptObject::CUsableScriptObject()
{
	callback = NULL;
	m_bNonscriptUsable = true;
	set_tip_text_default();
}

CUsableScriptObject::~CUsableScriptObject()
{
	clear_callback();
}

void CUsableScriptObject::set_callback(const luabind::object &lua_object, LPCSTR method)
{
	clear_callback				();
	callback					=xr_new<CScriptCallback>();
	callback->set				(lua_object, method);
}

void CUsableScriptObject::set_callback(const luabind::functor<void> &lua_function)
{
	clear_callback				();
	callback					=xr_new<CScriptCallback>();
	callback->set				(lua_function);
}
void CUsableScriptObject::clear_callback()
{
	if(callback)
	{
		callback->clear				()			;
		xr_delete					(callback)	;
	}
}
bool CUsableScriptObject::use(CGameObject* who_use)
{
	VERIFY(who_use);
	CGameObject* pThis = smart_cast<CGameObject*>(this); VERIFY(pThis);

	if(!callback)	return false;
	SCRIPT_CALLBACK_EXECUTE_2(*callback, pThis->lua_game_object(),who_use->lua_game_object());
	return true;
}

LPCSTR CUsableScriptObject::tip_text	()
{
	return *m_sTipText;
}
void CUsableScriptObject::set_tip_text	(LPCSTR new_text) 
{
	m_sTipText = new_text;
}
void CUsableScriptObject::set_tip_text_default () 
{
	m_sTipText = NULL;
}

bool CUsableScriptObject::nonscript_usable		()
{
	return m_bNonscriptUsable;
}
void CUsableScriptObject::set_nonscript_usable	(bool usable)
{
	m_bNonscriptUsable = usable;
}
