#include "stdafx.h"
#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "UsableScriptObject.h"
#include "GameObject.h"
#include "script_storage_space.h"
#include "script_engine.h"
#include "ai/monsters/bloodsucker/bloodsucker.h"
#include "motivation_action_manager_stalker.h"
#include "script_sound_info.h"
#include "script_monster_hit_info.h"
#include "script_space.h"
#include "ai/stalker/ai_stalker.h"
#include "searchlight.h"
#include "script_callback_ex.h"

struct ScriptCallbackInfo{
	CScriptCallbackEx<void>		m_callback;
	s16							m_event;
	ScriptCallbackInfo():m_event(-1){}
};


void CScriptGameObject::AddEventCallback			(s16 event, const luabind::functor<void> &lua_function)
{
	ScriptCallbackInfo* c = NULL;
	c = xr_new<ScriptCallbackInfo>();
	m_callbacks.insert( mk_pair(event,c) );

	c->m_callback.set	(lua_function);
	c->m_event			= event;
}

void CScriptGameObject::AddEventCallback			(s16 event, const luabind::functor<void> &lua_function, const luabind::object &lua_object)
{
	ScriptCallbackInfo* c = NULL;
	c = xr_new<ScriptCallbackInfo>();
	m_callbacks.insert( mk_pair(event,c) );

	c->m_callback.set	(lua_function,lua_object);
	c->m_event			= event;
}

void CScriptGameObject::RemoveEventCallback			(s16 event)
{
	CALLBACK_IT it = m_callbacks.find(event);
	if(it!=m_callbacks.end()){
		xr_delete(it->second);
		m_callbacks.erase(it);
	}
}

void CScriptGameObject::SetTipText (LPCSTR tip_text)
{
	CUsableScriptObject	*l_tpUseableScriptObject = smart_cast<CUsableScriptObject*>(&object());
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not clear use callback . Reason: the object is not usable");
	else l_tpUseableScriptObject->set_tip_text(tip_text);
}
void CScriptGameObject::SetTipTextDefault ()
{
	CUsableScriptObject	*l_tpUseableScriptObject = smart_cast<CUsableScriptObject*>(&object());
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not clear use callback . Reason: the object is not usable");
	else l_tpUseableScriptObject->set_tip_text_default();
}

void CScriptGameObject::SetNonscriptUsable(bool nonscript_usable)
{
	CUsableScriptObject	*l_tpUseableScriptObject = smart_cast<CUsableScriptObject*>(&object());
	if (!l_tpUseableScriptObject)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"Can not clear use callback . Reason: the object is not usable");
	else l_tpUseableScriptObject->set_nonscript_usable(nonscript_usable);
}

//////////////////////////////////////////////////////////////////////////
//CAI_Bloodsucker
void CScriptGameObject::set_invisible(bool val)
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Bloodsucker : cannot access class member set_invisible!");
		return;
	}
	
	val ? monster->manual_activate() : monster->manual_deactivate();
}

void CScriptGameObject::set_manual_invisibility(bool val)
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Bloodsucker : cannot access class member set_manual_invisible!");
		return;
	}

	val ? monster->set_manual_switch(true) : monster->set_manual_switch(false);
}
bool CScriptGameObject::get_invisible()
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Script Object : cannot access class member get_invisible!");
		return false;
	}
	
	return monster->CEnergyHolder::is_active();
}

bool CScriptGameObject::get_manual_invisibility()
{
	CAI_Bloodsucker		*monster = smart_cast<CAI_Bloodsucker*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Script Object : cannot access class member get_manual_invisibility!");
		return false;
	}
	return monster->is_manual_control();
}

Fvector CScriptGameObject::GetCurrentDirection()
{
	CProjector	*obj = smart_cast<CProjector*>(&object());
	if (!obj) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Script Object : cannot access class member GetCurrentDirection!");
		return Fvector().set(0.f,0.f,0.f);
	}
	return obj->GetCurrentDirection();
}

CScriptGameObject::CScriptGameObject		(CGameObject *game_object)
{
	m_game_object	= game_object;
	R_ASSERT2		(m_game_object,"Null actual object passed!");
}

CScriptGameObject::~CScriptGameObject		()
{
	delete_data		(m_callbacks);
}

CScriptGameObject *CScriptGameObject::Parent				() const
{
	CGameObject		*l_tpGameObject = smart_cast<CGameObject*>(object().H_Parent());
	if (l_tpGameObject)
		return		(l_tpGameObject->lua_game_object());
	else
		return		(0);
}

int	CScriptGameObject::clsid				() const
{
	return			(object().clsid());
}

LPCSTR CScriptGameObject::Name				() const
{
	return			(*object().cName());
}

shared_str CScriptGameObject::cName				() const
{
	return			(object().cName());
}

LPCSTR CScriptGameObject::Section				() const
{
	return			(*object().cNameSect());
}

void CScriptGameObject::Kill					(CScriptGameObject* who)
{
	CEntity				*l_tpEntity = smart_cast<CEntity*>(&object());
	if (!l_tpEntity) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"%s cannot access class member Kill!",*object().cName());
		return;
	}
	l_tpEntity->KillEntity	(who ? &who->object() : 0);
}

bool CScriptGameObject::Alive					() const
{
	CEntityAlive		*l_tpEntityAlive = smart_cast<CEntityAlive*>(&object());
	if (!l_tpEntityAlive) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member Alive!");
		return			(false);
	}
	return				(!!l_tpEntityAlive->g_Alive());
}

ALife::ERelationType CScriptGameObject::GetRelationType	(CScriptGameObject* who)
{
	CEntityAlive		*l_tpEntityAlive1 = smart_cast<CEntityAlive*>(&object());
	if (!l_tpEntityAlive1) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"%s cannot access class member GetRelationType!",*object().cName());
		return ALife::eRelationTypeDummy;
	}
	
	CEntityAlive		*l_tpEntityAlive2 = smart_cast<CEntityAlive*>(&who->object());
	if (!l_tpEntityAlive2) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"%s cannot apply GetRelationType method for non-alive object!",*who->object().cName());
		return ALife::eRelationTypeDummy;
	}
	
	return l_tpEntityAlive1->tfGetRelationType(l_tpEntityAlive2);
}

template <typename T>
IC	T	*CScriptGameObject::motivation_action_manager()
{
	CAI_Stalker	*manager = smart_cast<CAI_Stalker*>(&object());
	if (!manager)
		ai().script_engine().script_log				(ScriptStorage::eLuaMessageTypeError,"CMotivationActionManager : cannot access class member motivation_action_manager!");
	return					(&manager->brain());
}

CScriptMotivationActionManager *script_motivation_action_manager(CScriptGameObject *obj)
{
	return					(obj->motivation_action_manager<CScriptMotivationActionManager>());
}

CScriptSoundInfo CScriptGameObject::GetSoundInfo()
{
	CScriptSoundInfo	ret_val;

	CBaseMonster *l_tpMonster = smart_cast<CBaseMonster *>(&object());
	if (l_tpMonster) {
		if (l_tpMonster->SoundMemory.IsRememberSound()) {
			SoundElem se; 
			bool bDangerous;
			l_tpMonster->SoundMemory.GetSound(se, bDangerous);

			const CGameObject *pO = smart_cast<const CGameObject *>(se.who);
			ret_val.set((pO && !pO->getDestroy()) ?  pO->lua_game_object() : 0, bDangerous, se.position, se.power, int(se.time));
		}
	} else {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member GetSoundInfo!");
	}
	return			(ret_val);
}

CScriptMonsterHitInfo CScriptGameObject::GetMonsterHitInfo()
{
	CScriptMonsterHitInfo	ret_val;

	CBaseMonster *l_tpMonster = smart_cast<CBaseMonster *>(&object());
	if (l_tpMonster) {
		if (l_tpMonster->HitMemory.is_hit()) {
			CGameObject *pO = smart_cast<CGameObject *>(l_tpMonster->HitMemory.get_last_hit_object());
			ret_val.set((pO && !pO->getDestroy()) ?  pO->lua_game_object() : 0, l_tpMonster->HitMemory.get_last_hit_dir(), l_tpMonster->HitMemory.get_last_hit_time());
		}
	} else {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member GetMonsterHitInfo!");
	}
	return			(ret_val);
}

void CScriptGameObject::OnEventRaised(s16 event, NET_Packet& P)
{
	P.read_start();
	CALLBACK_IT it = m_callbacks.find(event);
	if(it==m_callbacks.end())
		return;
	((*it).second->m_callback)(&P);
}

