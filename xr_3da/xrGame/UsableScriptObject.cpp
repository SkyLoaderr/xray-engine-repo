#include "stdafx.h"
#include "UsableScriptObject.h"
#include "script_space.h"
#include "script_callback.h"
using namespace luabind;

CUsableScriptObject::CUsableScriptObject()
{
	callback=NULL;
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
bool CUsableScriptObject::use()
{
	if(!callback)	return false;
	SCRIPT_CALLBACK_EXECUTE_0(*callback);
	return true;
}