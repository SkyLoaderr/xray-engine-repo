#include "stdafx.h"
#include "script_game_object.h"
#include "UsableScriptObject.h"
#include "GameObject.h"
#include "script_storage_space.h"
#include "script_engine.h"
void CScriptGameObject::SetUseCallback(const luabind::functor<void> &tpUseCallback)
{
	CUsableScriptObject	*l_tpUseableScriptObject = dynamic_cast<CUsableScriptObject*>(m_tpGameObject);
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not set callback on use. Reason: the object is not usable");
	else l_tpUseableScriptObject->set_callback(tpUseCallback);
	
}

void CScriptGameObject::SetUseCallback(const luabind::object &object, LPCSTR method)
{
	CUsableScriptObject	*l_tpUseableScriptObject = dynamic_cast<CUsableScriptObject*>(m_tpGameObject);
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not set callback on use. Reason: the object is not usable");
	else l_tpUseableScriptObject->set_callback(object,method);
}

void CScriptGameObject::ClearUseCallback()
{
	CUsableScriptObject	*l_tpUseableScriptObject = dynamic_cast<CUsableScriptObject*>(m_tpGameObject);
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not clear use callback . Reason: the object is not usable");
	else l_tpUseableScriptObject->clear_callback();
}


