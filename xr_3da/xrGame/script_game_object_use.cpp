#include "stdafx.h"
#include "script_game_object.h"
#include "UsableScriptObject.h"
#include "GameObject.h"
#include "script_storage_space.h"
#include "script_engine.h"
#include "ai/bloodsucker/ai_bloodsucker.h"
#include "motivation_action_manager.h"
#include "script_sound_info.h"
#include "script_monster_hit_info.h"

void CScriptGameObject::AddEventCallback			(s16 event, const luabind::functor<void> &lua_function)
{
	ScriptCallbackInfo* c = NULL;
	c = xr_new<ScriptCallbackInfo>();
	m_callbacks.insert( mk_pair(event,c) );

	c->m_callback.set	(lua_function);
	c->m_event			= event;
}

void CScriptGameObject::AddEventCallback			(s16 event, const luabind::object &lua_object, LPCSTR method)
{
	ScriptCallbackInfo* c = NULL;
	c = xr_new<ScriptCallbackInfo>();
	m_callbacks.insert( mk_pair(event,c) );

	c->m_callback.set	(lua_object,method);
	c->m_event			= event;
}

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

//////////////////////////////////////////////////////////////////////////
//CAI_Bloodsucker
void CScriptGameObject::set_invisible(bool val)
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Bloodsucker : cannot access class member set_invisible!");
		return;
	}
	
	val ? monster->manual_activate() : monster->manual_deactivate();
}

void CScriptGameObject::set_manual_invisibility(bool val)
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Bloodsucker : cannot access class member set_manual_invisible!");
		return;
	}

	val ? monster->set_manual_switch(true) : monster->set_manual_switch(false);
}
bool CScriptGameObject::get_invisible()
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Script Object : cannot access class member get_invisible!");
		return false;
	}
	
	return monster->CEnergyHolder::is_active();
}

bool CScriptGameObject::get_manual_invisibility()
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Script Object : cannot access class member get_manual_invisibility!");
		return false;
	}
	return monster->is_manual_control();
}

CScriptGameObject::CScriptGameObject		(CGameObject *tpGameObject)
{
	m_tpGameObject	= tpGameObject;
	R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
}

CScriptGameObject::~CScriptGameObject		()
{
		 delete_data(m_callbacks);
}



CScriptGameObject *CScriptGameObject::Parent				() const
{
	CGameObject		*l_tpGameObject = smart_cast<CGameObject*>(m_tpGameObject->H_Parent());
	if (l_tpGameObject)
		return		(l_tpGameObject->lua_game_object());
	else
		return		(0);
}

int	CScriptGameObject::clsid				() const
{
	VERIFY			(m_tpGameObject);
	return			(m_tpGameObject->clsid());
}

LPCSTR CScriptGameObject::Name				() const
{
	VERIFY			(m_tpGameObject);
	return			(*m_tpGameObject->cName());
}

ref_str CScriptGameObject::cName				() const
{
	VERIFY			(m_tpGameObject);
	return			(*m_tpGameObject->cName());
}

LPCSTR CScriptGameObject::Section				() const
{
	VERIFY			(m_tpGameObject);
	return			(*m_tpGameObject->cNameSect());
}

void CScriptGameObject::Kill					(CScriptGameObject* who)
{
	CEntity				*l_tpEntity = smart_cast<CEntity*>(m_tpGameObject);
	if (!l_tpEntity) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"%s cannot access class member Kill!",*m_tpGameObject->cName());
		return;
	}
	l_tpEntity->KillEntity	(who ? who->m_tpGameObject : 0);
}

bool CScriptGameObject::Alive					() const
{
	CEntityAlive		*l_tpEntityAlive = smart_cast<CEntityAlive*>(m_tpGameObject);
	if (!l_tpEntityAlive) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member Alive!");
		return			(false);
	}
	return				(!!l_tpEntityAlive->g_Alive());
}

ALife::ERelationType CScriptGameObject::GetRelationType	(CScriptGameObject* who)
{
	CEntityAlive		*l_tpEntityAlive1 = smart_cast<CEntityAlive*>(m_tpGameObject);
	if (!l_tpEntityAlive1) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"%s cannot access class member!",*m_tpGameObject->cName());
		return ALife::eRelationTypeDummy;
	}
	
	CEntityAlive		*l_tpEntityAlive2 = smart_cast<CEntityAlive*>(who->m_tpGameObject);
	if (!l_tpEntityAlive2) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"%s cannot access class member!",*who->m_tpGameObject->cName());
		return ALife::eRelationTypeDummy;
	}
	
	return l_tpEntityAlive1->tfGetRelationType(l_tpEntityAlive2);
}

template <typename T>
IC	T	*CScriptGameObject::motivation_action_manager()
{
	T	*manager = smart_cast<T*>(m_tpGameObject);
	if (!manager)
		ai().script_engine().script_log				(ScriptStorage::eLuaMessageTypeError,"CMotivationActionManager : cannot access class member motivation_action_manager!");
	return					(manager);
}

CScriptMotivationActionManager *script_motivation_action_manager(CScriptGameObject *obj)
{
	return							(obj->motivation_action_manager<CScriptMotivationActionManager>());
}

CScriptSoundInfo CScriptGameObject::GetSoundInfo()
{
	CScriptSoundInfo	ret_val;
	
	CAI_Biting *l_tpMonster = smart_cast<CAI_Biting *>(m_tpGameObject);
	if (l_tpMonster) {
		if (l_tpMonster->SoundMemory.IsRememberSound()) {
			SoundElem se; 
			bool bDangerous;
			l_tpMonster->SoundMemory.GetSound(se, bDangerous);
			
			const CGameObject *pO = smart_cast<const CGameObject *>(se.who);
			ret_val.set((pO) ?  pO->lua_game_object() : 0, bDangerous, se.position, se.power, int(se.time));
		}
	} else {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member GetSoundInfo!");
	}
	return			(ret_val);
}

CScriptMonsterHitInfo CScriptGameObject::GetMonsterHitInfo()
{
	CScriptMonsterHitInfo	ret_val;

	CAI_Biting *l_tpMonster = smart_cast<CAI_Biting *>(m_tpGameObject);
	if (l_tpMonster) {
		if (l_tpMonster->HitMemory.is_hit()) {
			CGameObject *pO = smart_cast<CGameObject *>(l_tpMonster->HitMemory.get_last_hit_object());
			ret_val.set((pO) ?  pO->lua_game_object() : 0, l_tpMonster->HitMemory.get_last_hit_dir(), l_tpMonster->HitMemory.get_last_hit_time());
		}
	} else {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member GetMonsterHitInfo!");
	}
	return			(ret_val);
}

