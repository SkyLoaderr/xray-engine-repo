////////////////////////////////////////////////////////////////////////////
// script_game_object_trader.сpp :	функции для торговли и торговцев
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"

#include "script_task.h"
#include "script_zone.h"
#include "ai/trader/ai_trader.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_task_registry.h"
#include "alife_trader_registry.h"

#include "ai/stalker/ai_stalker.h"
#include "weapon.h"
#include "cover_evaluators.h"
#include "cover_point.h"
#include "cover_manager.h"

void CScriptGameObject::SetCallback(const luabind::functor<void> &tpZoneCallback, bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = smart_cast<CScriptZone*>(m_tpGameObject);
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
	else if (l_tpScriptZone) l_tpScriptZone->set_callback(tpZoneCallback,bOnEnter);
	else l_tpTrader->set_callback(tpZoneCallback,bOnEnter);
}

void CScriptGameObject::SetCallback(const luabind::object &object, LPCSTR method, bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = smart_cast<CScriptZone*>(m_tpGameObject);
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
	else if (l_tpScriptZone) l_tpScriptZone->set_callback(object,method,bOnEnter);
	else l_tpTrader->set_callback(object,method,bOnEnter);
}

void CScriptGameObject::ClearCallback(bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = smart_cast<CScriptZone*>(m_tpGameObject);
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone : cannot access class member set_callback!");
	else if (l_tpScriptZone)l_tpScriptZone->clear_callback(bOnEnter);
	else l_tpTrader->clear_callback(bOnEnter);
}



void CScriptGameObject::SetTradeCallback(const luabind::functor<void> &tpTradeCallback) {
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
	else l_tpTrader->set_trade_callback(tpTradeCallback);
}

void CScriptGameObject::SetTradeCallback(const luabind::object &object, LPCSTR method) {
	CAI_Trader	*l_tpTrader	= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
	else l_tpTrader->set_trade_callback(object, method);
}


void CScriptGameObject::ClearTradeCallback() {
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member clear_trade_callback!");
	else l_tpTrader->clear_trade_callback();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CScriptGameObject::is_body_turning		() const
{
	CMovementManager	*movement_manager = smart_cast<CMovementManager*>(m_tpGameObject);
	if (!movement_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member is_turning!");
		return			(false);
	}
	CStalkerMovementManager	*stalker_movement_manager = smart_cast<CStalkerMovementManager*>(m_tpGameObject);
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
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player) {
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
		return					(0);
	}
	else
		return					(sound_player->add(prefix,max_count,type,priority,mask,internal_type,bone_name));
}

u32	CScriptGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name, LPCSTR head_anim)
{
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player) {
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
		return					(0);
	}
	else
		return					(sound_player->add(prefix,max_count,type,priority,mask,internal_type, bone_name, head_anim));
}

u32	CScriptGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type)
{
	return						(add_sound(prefix,max_count,type,priority,mask,internal_type,"bip01_head"));
}

void CScriptGameObject::remove_sound	(u32 internal_type)
{
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
	else
		sound_player->remove	(internal_type);
}

void CScriptGameObject::set_sound_mask	(u32 sound_mask)
{
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member set_sound_mask!");
	else {
		CEntityAlive			*entity_alive = smart_cast<CEntityAlive*>(sound_player);
		if (entity_alive) {
			VERIFY2				(entity_alive->g_Alive(),"Stalkers talk after death??? Say why??");
		}
		sound_player->set_sound_mask(sound_mask);
	}
}

void CScriptGameObject::play_sound		(u32 internal_type)
{
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time)
{
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time)
{
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time,min_start_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time)
{
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time,min_start_time,max_stop_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time)
{
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time,min_start_time,max_stop_time,min_stop_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id)
{
	CSoundPlayer				*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time,min_start_time,max_stop_time,min_stop_time,id);
}

int  CScriptGameObject::active_sound_count		(bool only_playing)
{
	CSoundPlayer	*sound_player = smart_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member active_sound_count!");
		return								(-1);
	}
	else
		return								(sound_player->active_sound_count(only_playing));
}

int  CScriptGameObject::active_sound_count		()
{
	return									(active_sound_count(false));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void CScriptGameObject::set_sight		(SightManager::ESightType sight_type, const Fvector *vector3d, u32 dwLookOverDelay)
{
	CSightManager				*sight_manager = smart_cast<CSightManager*>(m_tpGameObject);
	if (!sight_manager)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSightManager : cannot access class member set_sight!");
	else
		sight_manager->setup	(sight_type,vector3d);
}

void CScriptGameObject::set_sight		(SightManager::ESightType sight_type, bool torso_look, bool path)
{
	CSightManager				*sight_manager = smart_cast<CSightManager*>(m_tpGameObject);
	if (!sight_manager)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSightManager : cannot access class member set_sight!");
	else
		sight_manager->setup	(sight_type,torso_look,path);
}

void CScriptGameObject::set_sight		(SightManager::ESightType sight_type, const Fvector &vector3d, bool torso_look = false)
{
	CSightManager				*sight_manager = smart_cast<CSightManager*>(m_tpGameObject);
	if (!sight_manager)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSightManager : cannot access class member set_sight!");
	else
		sight_manager->setup	(sight_type,vector3d,torso_look);
}

void CScriptGameObject::set_sight		(SightManager::ESightType sight_type, const Fvector *vector3d)
{
	CSightManager				*sight_manager = smart_cast<CSightManager*>(m_tpGameObject);
	if (!sight_manager)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSightManager : cannot access class member set_sight!");
	else
		sight_manager->setup	(sight_type,vector3d);
}

void CScriptGameObject::set_sight		(CScriptGameObject *object_to_look, bool torso_look)
{
	CSightManager				*sight_manager = smart_cast<CSightManager*>(m_tpGameObject);
	if (!sight_manager)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSightManager : cannot access class member set_sight!");
	else
		sight_manager->setup	(object_to_look->m_tpGameObject,torso_look);
}

void CScriptGameObject::set_sight		(CScriptGameObject *object_to_look, bool torso_look, LPCSTR bone_name)
{
	CSightManager				*sight_manager = smart_cast<CSightManager*>(m_tpGameObject);
	if (!sight_manager)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSightManager : cannot access class member set_sight!");
	else
		sight_manager->setup	(object_to_look->m_tpGameObject,torso_look,bone_name);
}

void CScriptGameObject::set_sight		(const CMemoryInfo *memory_object, bool	torso_look)
{
	CSightManager				*sight_manager = smart_cast<CSightManager*>(m_tpGameObject);
	if (!sight_manager)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSightManager : cannot access class member set_sight!");
	else
		sight_manager->setup	(memory_object,torso_look);
}

bool CScriptGameObject::CheckTypeVisibility(const char *section_name)
{
	CCustomMonster		*l_tpCustomMonster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (l_tpCustomMonster)
		return			(l_tpCustomMonster->CheckTypeVisibility(section_name));
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member CheckTypeVisibility!");
		return			(false);
	}
}

// CAI_Stalker
void CScriptGameObject::UseObject(const CScriptGameObject *tpLuaGameObject)
{
	CCustomMonster		*l_tpCustomMonster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (l_tpCustomMonster)
		l_tpCustomMonster->UseObject(tpLuaGameObject->m_tpGameObject);
	else
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member UseObject!");
}

CScriptGameObject *CScriptGameObject::GetCurrentWeapon() const
{
	CAI_Stalker		*l_tpStalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject		*current_weapon = l_tpStalker->GetCurrentWeapon();
	return			(current_weapon ? current_weapon->lua_game_object() : 0);
}

CScriptGameObject *CScriptGameObject::GetCurrentEquipment() const
{
	CAI_Stalker		*l_tpStalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject		*current_equipment = l_tpStalker->GetCurrentEquipment();
	return			(current_equipment ? current_equipment->lua_game_object() : 0);
}

CScriptGameObject *CScriptGameObject::GetFood() const
{
	CAI_Stalker		*l_tpStalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject		*food = l_tpStalker->GetFood();
	return			(food ? food->lua_game_object() : 0);
}

CScriptGameObject *CScriptGameObject::GetMedikit() const
{
	CAI_Stalker		*l_tpStalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject		*medikit = l_tpStalker->GetMedikit();
	return			(medikit ? medikit->lua_game_object() : 0);
}

CScriptGameObject *CScriptGameObject::GetEnemy() const
{
	CCustomMonster		*l_tpCustomMonster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (l_tpCustomMonster) {
		if (l_tpCustomMonster->GetCurrentEnemy() && !l_tpCustomMonster->GetCurrentEnemy()->getDestroy()) return (l_tpCustomMonster->GetCurrentEnemy()->lua_game_object());
		else return (0);
	} else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member GetEnemy!");
		return			(0);
	}
}

CScriptGameObject *CScriptGameObject::GetCorpse() const
{
	CCustomMonster		*l_tpCustomMonster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (l_tpCustomMonster)
		if (l_tpCustomMonster->GetCurrentCorpse() && !l_tpCustomMonster->GetCurrentCorpse()->getDestroy()) return (l_tpCustomMonster->GetCurrentCorpse()->lua_game_object());
		else return (0);
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member GetCorpse!");
		return			(0);
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void  CScriptGameObject::set_body_state			(EBodyState body_state)
{
	VERIFY						((body_state == eBodyStateStand) || (body_state == eBodyStateCrouch));
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_body_state	(body_state);
}

void  CScriptGameObject::set_movement_type		(EMovementType movement_type)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_movement_type	(movement_type);
}

void  CScriptGameObject::set_mental_state		(EMentalState mental_state)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_mental_state	(mental_state);
}

void  CScriptGameObject::set_path_type			(MovementManager::EPathType path_type)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_path_type	(path_type);
}

void  CScriptGameObject::set_detail_path_type	(DetailPathManager::EDetailPathType detail_path_type)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_detail_path_type	(detail_path_type);
}

u32 CScriptGameObject::GetRank		()
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetRank!");
		return					(ALife::eStalkerRankNone);
	}
	else
		return					(stalker->GetRank());
}

void CScriptGameObject::set_desired_position	()
{
	CAI_Stalker							*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log							(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_desired_position	(0);
}

void CScriptGameObject::set_desired_position	(const Fvector *desired_position)
{
	CAI_Stalker							*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log							(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_desired_position	(desired_position);
}

void  CScriptGameObject::set_desired_direction	()
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_desired_direction	(0);
}

void  CScriptGameObject::set_desired_direction	(const Fvector *desired_direction)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_desired_direction	(desired_direction);
}

void  CScriptGameObject::set_node_evaluator		(CAbstractVertexEvaluator *node_evaluator)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_node_evaluator	(node_evaluator);
}

void  CScriptGameObject::set_node_evaluator		()
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_node_evaluator	(0);
}

void  CScriptGameObject::set_path_evaluator		()
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_path_evaluator	(0);
}

void CScriptGameObject::set_path_evaluator		(CAbstractVertexEvaluator *path_evaluator)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_path_evaluator	(path_evaluator);
}

void CScriptGameObject::set_patrol_path		(LPCSTR path_name, const PatrolPathManager::EPatrolStartType patrol_start_type, const PatrolPathManager::EPatrolRouteType patrol_route_type, bool random)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_path		(path_name,patrol_start_type,patrol_route_type,random);
}

void CScriptGameObject::set_dest_level_vertex_id(u32 level_vertex_id)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member set_dest_level_vertex_id!");
	else {

		if (!ai().level_graph().valid_vertex_id(level_vertex_id)) {
#ifdef DEBUG
			ai().script_engine().script_log				(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : invalid vertex id being setup by action %s!",stalker->CMotivationActionManagerStalker::current_action().m_action_name);
#endif
			return;
		}
		stalker->set_level_dest_vertex	(level_vertex_id);
	}
}

LPCSTR CScriptGameObject::GetPatrolPathName()
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		CScriptMonster	*script_monster = smart_cast<CScriptMonster*>(m_tpGameObject);
		if (!script_monster) {
			ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member GetPatrolPathName!");
			return		("");
		}
		else
			return		(script_monster->GetPatrolPathName());
	}
	else
		return			(*stalker->path_name());
}

void CScriptGameObject::add_animation			(LPCSTR animation, bool hand_usage)
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member add_animation!");
		return;
	}
	stalker->add_animation(animation,hand_usage);
}

void CScriptGameObject::clear_animations		()
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member clear_animations!");
		return;
	}
	stalker->clear_animations();
}

int	CScriptGameObject::animation_count		() const
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member clear_animations!");
		return			(-1);
	}
	return				((int)stalker->m_script_animations.size());
}

const CCoverPoint *CScriptGameObject::best_cover	(const Fvector &position, const Fvector &enemy_position, float radius, float min_enemy_distance, float max_enemy_distance)
{
	CAI_Stalker		*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member best_cover!");
		return		(0);
	}
	stalker->m_ce_best->setup(enemy_position,min_enemy_distance,max_enemy_distance,0.f);
	CCoverPoint		*point = ai().cover_manager().best_cover(position,radius,*stalker->m_ce_best);
	return			(point);
}

const CCoverPoint *CScriptGameObject::safe_cover	(const Fvector &position, float radius, float min_distance)
{
	CAI_Stalker		*stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member best_cover!");
		return		(0);
	}
	stalker->m_ce_safe->setup(min_distance);
	CCoverPoint		*point = ai().cover_manager().best_cover(position,radius,*stalker->m_ce_safe);
	return			(point);
}

const xr_vector<CVisibleObject>	&CScriptGameObject::memory_visible_objects	() const
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member memory_visible_objects!");
		NODEFAULT;
	}
	return			(monster->memory_visible_objects());
}

const xr_vector<CSoundObject>	&CScriptGameObject::memory_sound_objects	() const
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member memory_sound_objects!");
		NODEFAULT;
	}
	return			(monster->sound_objects());
}

const xr_vector<CHitObject>		&CScriptGameObject::memory_hit_objects		() const
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member memory_hit_objects!");
		NODEFAULT;
	}
	return			(monster->hit_objects());
}

void CScriptGameObject::ChangeTeam(u8 team, u8 squad, u8 group)
{
	CCustomMonster				*custom_monster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (!custom_monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CCustomMonster: cannot access class member ChangeTeam!");
	else
		custom_monster->ChangeTeam(team,squad,group);
}
