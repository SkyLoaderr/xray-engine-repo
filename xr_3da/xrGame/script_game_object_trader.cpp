////////////////////////////////////////////////////////////////////////////
// script_game_object_trader.�pp :	������� ��� �������� � ���������
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"
#include "script_game_object_impl.h"

#include "script_task.h"
#include "script_zone.h"
#include "ai/trader/ai_trader.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_task_registry.h"
#include "alife_trader_registry.h"

#include "ai/stalker/ai_stalker.h"
//#include "weapon.h"

class CWeapon;

void CScriptGameObject::SetCallback(const luabind::functor<void> &tpZoneCallback, bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = smart_cast<CScriptZone*>(object());
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>(object());

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
	else if (l_tpScriptZone) l_tpScriptZone->set_callback(tpZoneCallback,bOnEnter);
	else l_tpTrader->set_callback(tpZoneCallback,bOnEnter);
}

void CScriptGameObject::SetCallback(const luabind::object &lua_object, LPCSTR method, bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = smart_cast<CScriptZone*>(object());
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>(object());

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
	else if (l_tpScriptZone) l_tpScriptZone->set_callback(lua_object,method,bOnEnter);
	else l_tpTrader->set_callback(lua_object,method,bOnEnter);
}

void CScriptGameObject::ClearCallback(bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = smart_cast<CScriptZone*>(object());
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>(object());

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone : cannot access class member set_callback!");
	else if (l_tpScriptZone)l_tpScriptZone->clear_callback(bOnEnter);
	else l_tpTrader->clear_callback(bOnEnter);
}



void CScriptGameObject::SetTradeCallback(const luabind::functor<void> &tpTradeCallback) {
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(object());

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
	else l_tpTrader->set_trade_callback(tpTradeCallback);
}

void CScriptGameObject::SetTradeCallback(const luabind::object &lua_object, LPCSTR method) {
	CAI_Trader	*l_tpTrader	= smart_cast<CAI_Trader*>	(object());

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
	else l_tpTrader->set_trade_callback(lua_object, method);
}


void CScriptGameObject::ClearTradeCallback() {
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(object());

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member clear_trade_callback!");
	else l_tpTrader->clear_trade_callback();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CScriptGameObject::is_body_turning		() const
{
	CMovementManager	*movement_manager = smart_cast<CMovementManager*>(object());
	if (!movement_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member is_turning!");
		return			(false);
	}
	CStalkerMovementManager	*stalker_movement_manager = smart_cast<CStalkerMovementManager*>(object());
	if (!stalker_movement_manager)
		return			(!fsimilar(movement_manager->body_orientation().target.yaw,movement_manager->body_orientation().current.yaw));
	else
		return			(!fsimilar(stalker_movement_manager->head_orientation().target.yaw,stalker_movement_manager->head_orientation().current.yaw) || !fsimilar(movement_manager->body_orientation().target.yaw,movement_manager->body_orientation().current.yaw));
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

u32	CScriptGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster) {
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
		return					(0);
	}
	else
		return					(monster->CSoundPlayer::add(prefix,max_count,type,priority,mask,internal_type,bone_name));
}

u32	CScriptGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name, LPCSTR head_anim)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster) {
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
		return					(0);
	}
	else
		return					(monster->CSoundPlayer::add(prefix,max_count,type,priority,mask,internal_type, bone_name, head_anim));
}

u32	CScriptGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type)
{
	return						(add_sound(prefix,max_count,type,priority,mask,internal_type,"bip01_head"));
}

void CScriptGameObject::remove_sound	(u32 internal_type)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
	else
		monster->CSoundPlayer::remove	(internal_type);
}

void CScriptGameObject::set_sound_mask	(u32 sound_mask)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member set_sound_mask!");
	else {
		CEntityAlive			*entity_alive = smart_cast<CEntityAlive*>(monster);
		if (entity_alive) {
			VERIFY2				(entity_alive->g_Alive(),"Stalkers talk after death??? Say why??");
		}
		monster->CSoundPlayer::set_sound_mask(sound_mask);
	}
}

void CScriptGameObject::play_sound		(u32 internal_type)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->CSoundPlayer::play		(internal_type);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->CSoundPlayer::play		(internal_type,max_start_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->CSoundPlayer::play		(internal_type,max_start_time,min_start_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->CSoundPlayer::play		(internal_type,max_start_time,min_start_time,max_stop_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->CSoundPlayer::play		(internal_type,max_start_time,min_start_time,max_stop_time,min_stop_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->CSoundPlayer::play		(internal_type,max_start_time,min_start_time,max_stop_time,min_stop_time,id);
}

int  CScriptGameObject::active_sound_count		(bool only_playing)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member active_sound_count!");
		return								(-1);
	}
	else
		return								(monster->CSoundPlayer::active_sound_count(only_playing));
}

int  CScriptGameObject::active_sound_count		()
{
	return									(active_sound_count(false));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
