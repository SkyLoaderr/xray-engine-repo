#include "stdafx.h"
#include "script_game_object.h"
#include "UsableScriptObject.h"
#include "GameObject.h"
#include "script_storage_space.h"
#include "script_engine.h"
void CScriptGameObject::SetUseCallback(const luabind::functor<void> &tpUseCallback)
{
	CUsableScriptObject	*l_tpUseableScriptObject = smart_cast<CUsableScriptObject*>(m_tpGameObject);
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not set callback on use. Reason: the object is not usable");
	else l_tpUseableScriptObject->set_callback(tpUseCallback);
	
}

void CScriptGameObject::SetUseCallback(const luabind::object &object, LPCSTR method)
{
	CUsableScriptObject	*l_tpUseableScriptObject = smart_cast<CUsableScriptObject*>(m_tpGameObject);
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not set callback on use. Reason: the object is not usable");
	else l_tpUseableScriptObject->set_callback(object,method);
}

void CScriptGameObject::ClearUseCallback()
{
	CUsableScriptObject	*l_tpUseableScriptObject = smart_cast<CUsableScriptObject*>(m_tpGameObject);
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not clear use callback . Reason: the object is not usable");
	else l_tpUseableScriptObject->clear_callback();
}

void CScriptGameObject::SetTipText (LPCSTR tip_text)
{
	CUsableScriptObject	*l_tpUseableScriptObject = smart_cast<CUsableScriptObject*>(m_tpGameObject);
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not clear use callback . Reason: the object is not usable");
	else l_tpUseableScriptObject->set_tip_text(tip_text);
}
void CScriptGameObject::SetTipTextDefault ()
{
	CUsableScriptObject	*l_tpUseableScriptObject = smart_cast<CUsableScriptObject*>(m_tpGameObject);
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not clear use callback . Reason: the object is not usable");
	else l_tpUseableScriptObject->set_tip_text_default();
}

void CScriptGameObject::SetNonscriptUsable(bool nonscript_usable)
{
	CUsableScriptObject	*l_tpUseableScriptObject = smart_cast<CUsableScriptObject*>(m_tpGameObject);
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not clear use callback . Reason: the object is not usable");
	else l_tpUseableScriptObject->set_nonscript_usable(nonscript_usable);
}