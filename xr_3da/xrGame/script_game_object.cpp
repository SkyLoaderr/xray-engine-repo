////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "script_entity_action.h"
#include "ai_space.h"
#include "script_engine.h"
#include "script_entity.h"
#include "physicsshellholder.h"
#include "helicopter.h"
#include "inventoryowner.h"
#include "movement_manager.h"
#include "entity_alive.h"
#include "weaponmagazined.h"
#include "xrmessages.h"
#include "inventory.h"
#include "script_ini_file.h"
#include "script_callback.h"
#include "../skeletoncustom.h"
#include "HangingLamp.h"
#include "patrol_path_manager.h"
#include "ai_object_location.h"
#include "custommonster.h"
#include "entitycondition.h"
#include "space_restrictor.h"
#include "script_callback_ex.h"

class CScriptBinderObject;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BIND_FUNCTION10	(&object(),	CScriptGameObject::Position,			CGameObject,	Position,			Fvector,						Fvector());
BIND_FUNCTION10	(&object(),	CScriptGameObject::Direction,			CGameObject,	Direction,			Fvector,						Fvector());
BIND_FUNCTION10	(&object(),	CScriptGameObject::Mass,		CPhysicsShellHolder,	GetMass,			float,							float(-1));
BIND_FUNCTION10	(&object(),	CScriptGameObject::ID,					CGameObject,	ID,					u32,							u32(-1));
BIND_FUNCTION10	(&object(),	CScriptGameObject::getVisible,			CGameObject,	getVisible,			BOOL,							FALSE);
BIND_FUNCTION01	(&object(),	CScriptGameObject::setVisible,			CGameObject,	setVisible,			BOOL,							BOOL);
BIND_FUNCTION10	(&object(),	CScriptGameObject::getEnabled,			CGameObject,	getEnabled,			BOOL,							FALSE);
BIND_FUNCTION01	(&object(),	CScriptGameObject::setEnabled,			CGameObject,	setEnabled,			BOOL,							BOOL);
BIND_FUNCTION10	(&object(),	CScriptGameObject::DeathTime,			CEntity,		GetLevelDeathTime,	u32,							0);
BIND_FUNCTION10	(&object(),	CScriptGameObject::Armor,				CEntity,		g_Armor,			float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::MaxHealth,			CEntity,		g_MaxHealth,		float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::Accuracy,			CInventoryOwner,GetWeaponAccuracy,	float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::Team,				CEntity,		g_Team,				int,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::Squad,				CEntity,		g_Squad,			int,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::Group,				CEntity,		g_Group,			int,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetFOV,				CEntityAlive,	ffGetFov,			float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetRange,			CEntityAlive,	ffGetRange,			float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetHealth,			CEntityAlive,	conditions().GetHealth,			float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetPower,			CEntityAlive,	conditions().GetPower,			float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetSatiety,			CEntityAlive,	conditions().GetSatiety,			float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetRadiation,		CEntityAlive,	conditions().GetRadiation,		float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetCircumspection,	CEntityAlive,	conditions().GetCircumspection,	float,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetMorale,			CEntityAlive,	conditions().GetEntityMorale,	float,							-1);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetHealth,			CEntityAlive,	conditions().ChangeHealth,		float,							float);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetPower,			CEntityAlive,	conditions().ChangePower,		float,							float);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetSatiety,			CEntityAlive,	conditions().ChangeSatiety,		float,							float);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetRadiation,		CEntityAlive,	conditions().ChangeRadiation,	float,							float);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetCircumspection,	CEntityAlive,	conditions().ChangeCircumspection,float,							float);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetMorale,			CEntityAlive,	conditions().ChangeEntityMorale,	float,							float);
BIND_FUNCTION02	(&object(),	CScriptGameObject::SetScriptControl,	CScriptEntity,	SetScriptControl,	bool,								LPCSTR,					bool,					shared_str);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetScriptControl,	CScriptEntity,	GetScriptControl,	bool,								false);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetScriptControlName,CScriptEntity,GetScriptControlName,LPCSTR,							"");
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetEnemyStrength,	CScriptEntity,	get_enemy_strength,	int,								0);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetActionCount,		CScriptEntity,	GetActionCount,		u32,					0);

u32	CScriptGameObject::level_vertex_id		() const
{
	return						(object().ai_location().level_vertex_id());
}

CScriptIniFile *CScriptGameObject::spawn_ini			() const
{
	return			((CScriptIniFile*)object().spawn_ini());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::ResetActionQueue()
{
	CScriptEntity		*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member ResetActionQueue!");
	else
		l_tpScriptMonster->ClearActionQueue();
}

CScriptEntityAction	*CScriptGameObject::GetCurrentAction	() const
{
	CScriptEntity		*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member GetCurrentAction!");
	else
		if (l_tpScriptMonster->GetCurrentAction())
			return		(xr_new<CScriptEntityAction>(l_tpScriptMonster->GetCurrentAction()));
	return				(0);
}

void CScriptGameObject::AddAction	(const CScriptEntityAction *tpEntityAction, bool bHighPriority)
{
	CScriptEntity		*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member AddAction!");
	else
		l_tpScriptMonster->AddAction(tpEntityAction, bHighPriority);
}

const CScriptEntityAction *CScriptGameObject::GetActionByIndex(u32 action_index)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member GetActionByIndex!");
		return			(0);
	}
	else
		return			(l_tpScriptMonster->GetActionByIndex(action_index));
}

void CScriptGameObject::SetCallback(const luabind::object &lua_object, LPCSTR method, const ScriptEntity::EActionType tActionType)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member set_callback!");
	else
		l_tpScriptMonster->set_callback(lua_object,method,tActionType);
}

void CScriptGameObject::SetCallback(const luabind::functor<void> &lua_function, const ScriptEntity::EActionType tActionType)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member set_callback!");
	else
		l_tpScriptMonster->set_callback(lua_function,tActionType);
}

void CScriptGameObject::ClearCallback(const ScriptEntity::EActionType tActionType)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member clear_callback!");
	else
		l_tpScriptMonster->clear_callback(tActionType);
}

void CScriptGameObject::SetSoundCallback(const luabind::object &lua_object, LPCSTR method)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member set_sound_callback!");
	else
		l_tpScriptMonster->set_sound_callback(lua_object,method);
}

void CScriptGameObject::SetSoundCallback(const luabind::functor<void> &lua_function)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member set_sound_callback!");
	else
		l_tpScriptMonster->set_sound_callback(lua_function);
}

void CScriptGameObject::ClearSoundCallback(bool member_callback)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member clear_hit_callback!");
	else
		l_tpScriptMonster->clear_sound_callback(member_callback);
}
//////////////////////////////////////////////////////////////////////////
// Hit Callbacks 
void CScriptGameObject::SetHitCallback(const luabind::object &lua_object, LPCSTR method)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member set_hit_callback!");
	else
		l_tpScriptMonster->set_hit_callback(lua_object,method);
}

void CScriptGameObject::SetHitCallback(const luabind::functor<void> &lua_function)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member set_hit_callback!");
	else
		l_tpScriptMonster->set_hit_callback(lua_function);
}

void CScriptGameObject::ClearHitCallback(bool member_callback)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member clear_hit_callback!");
	else
		l_tpScriptMonster->clear_hit_callback(member_callback);
}

//////////////////////////////////////////////////////////////////////////
// Death Callbacks 
void CScriptGameObject::SetDeathCallback(const luabind::object &lua_object, LPCSTR method)
{
	CEntityAlive *entity = smart_cast<CEntityAlive*>(&object());
	if (!entity)
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CEntityAlive : cannot access class member set_death_callback!");
	else
		entity->set_death_callback(lua_object,method);
}

void CScriptGameObject::SetDeathCallback(const luabind::functor<void> &lua_function)
{
	CEntityAlive *entity = smart_cast<CEntityAlive*>(&object());
	if (!entity)
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CEntityAlive : cannot access class member set_death_callback!");
	else
		entity->set_death_callback(lua_function);
}

void CScriptGameObject::ClearDeathCallback(bool member_callback)
{
	CEntityAlive *entity = smart_cast<CEntityAlive*>(&object());
	if (!entity)
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CEntityAlive : cannot access class member clear_death_callback!");
	else
		entity->clear_death_callback(member_callback);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CPhysicsShell* CScriptGameObject::get_physics_shell() const
{
	CPhysicsShellHolder* ph_shell_holder =smart_cast<CPhysicsShellHolder*>(&object());
	if(! ph_shell_holder) return NULL;
	return ph_shell_holder->PPhysicsShell();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CHelicopter* CScriptGameObject::get_helicopter	()
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(&object());
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member get_helicopter!");
		NODEFAULT;
	}
	return helicopter;
}

CHangingLamp* CScriptGameObject::get_hanging_lamp()
{
	CHangingLamp*	lamp = smart_cast<CHangingLamp*>(&object());
	if (!lamp) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : it is not a lamp!");
		NODEFAULT;
	}
	return lamp;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

LPCSTR CScriptGameObject::WhoHitName()
{
	CEntityAlive *entity_alive = smart_cast<CEntityAlive*>(&object());
	if (entity_alive)
		return			entity_alive->conditions().GetWhoHitLastTime()?(*entity_alive->conditions().GetWhoHitLastTime()->cName()):NULL;
	else 
	{
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member  WhoHitName()");
		return			NULL;
	}
}

LPCSTR CScriptGameObject::WhoHitSectionName()
{
	CEntityAlive *entity_alive = smart_cast<CEntityAlive*>(&object());
	if (entity_alive)
		return			entity_alive->conditions().GetWhoHitLastTime()?(*entity_alive->conditions().GetWhoHitLastTime()->cNameSect()):NULL;
	else 
	{
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member  WhoHitName()");
		return			NULL;
	}
}

bool CScriptGameObject::CheckObjectVisibility(const CScriptGameObject *tpLuaGameObject)
{
	CScriptEntity		*l_tpCustomMonster = smart_cast<CScriptEntity*>(&object());
	if (l_tpCustomMonster)
		return			(l_tpCustomMonster->CheckObjectVisibility(&tpLuaGameObject->object()));
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member CheckObjectVisibility!");
		return			(false);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CScriptBinderObject	*CScriptGameObject::binded_object	()
{
	return									(object().object());
}

void CScriptGameObject::bind_object			(CScriptBinderObject *game_object)
{
	object().set_object	(game_object);
}

void CScriptGameObject::set_previous_point	(int point_index)
{
	CCustomMonster		*monster = smart_cast<CCustomMonster*>(this);
	if (!monster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member set_previous_point!");
	else
		monster->movement().patrol().set_previous_point(point_index);
}

void CScriptGameObject::set_start_point	(int point_index)
{
	CCustomMonster		*monster = smart_cast<CCustomMonster*>(this);
	if (!monster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member set_start_point!");
	else
		monster->movement().patrol().set_start_point(point_index);
}

u32 CScriptGameObject::get_current_patrol_point_index()
{
	CCustomMonster		*monster = smart_cast<CCustomMonster*>(this);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot call [get_current_patrol_point_index()]!");
		return			(u32(-1));
	}
	return				(monster->movement().patrol().get_current_point_index());
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Fvector	CScriptGameObject::bone_position	(LPCSTR bone_name) const
{
	u16					bone_id;
	if (xr_strlen(bone_name))
		bone_id			= smart_cast<CKinematics*>(object().Visual())->LL_BoneID(bone_name);
	else
		bone_id			= smart_cast<CKinematics*>(object().Visual())->LL_GetBoneRoot();

	Fmatrix				matrix;
	matrix.mul_43		(object().XFORM(),smart_cast<CKinematics*>(object().Visual())->LL_GetBoneInstance(bone_id).mTransform);
	return				(matrix.c);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

u32 CScriptGameObject::GetAmmoElapsed()
{
	const CWeapon	*weapon = smart_cast<const CWeapon*>(&object());
	if (!weapon)
		return		(0);
	return			(weapon->GetAmmoElapsed());
}

void CScriptGameObject::SetAmmoElapsed(int ammo_elapsed)
{
	CWeapon	*weapon = smart_cast<CWeapon*>(&object());
	if (!weapon) return;
	weapon->SetAmmoElapsed(ammo_elapsed);
}

u32 CScriptGameObject::GetAmmoCurrent() const
{
	const CWeapon	*weapon = smart_cast<const CWeapon*>(&object());
	if (!weapon)
		return		(0);
	return			(weapon->GetAmmoCurrent());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::SetQueueSize(u32 queue_size)
{
	CWeaponMagazined		*weapon = smart_cast<CWeaponMagazined*>(&object());
	weapon->SetQueueSize	(queue_size);
}

////////////////////////////////////////////////////////////////////////////
//// Inventory Owner
////////////////////////////////////////////////////////////////////////////
void CScriptGameObject::SetPdaCallback(const luabind::functor<void> &lua_function)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(&object());
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_pda_callback!");
	else
		pInvOwner->set_pda_callback(lua_function);
}

void CScriptGameObject::SetPdaCallback(const luabind::object &instance, LPCSTR method)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(&object());
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_pda_callback!");
	else
		pInvOwner->set_pda_callback(instance,method);
}

void CScriptGameObject::ClearPdaCallback()
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(&object());
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member clear_pda_callback!");
	else
		pInvOwner->clear_pda_callback();
}

void CScriptGameObject::SetInfoCallback(const luabind::functor<void> &lua_function)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(&object());
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_info_callback!");
	else
		pInvOwner->set_info_callback(lua_function);
}

void CScriptGameObject::SetInfoCallback(const luabind::object &instance, LPCSTR method)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(&object());
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_info_callback!");
	else
		pInvOwner->set_info_callback(instance,method);
}

void CScriptGameObject::ClearInfoCallback()
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(&object());
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member clear_info_callback!");
	else
		pInvOwner->clear_info_callback();
}

u32	CScriptGameObject::Cost			() const
{
	CInventoryItem		*inventory_item = smart_cast<CInventoryItem*>(&object());
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member Cost!");
		return			(false);
	}
	return				(inventory_item->Cost());
}

float CScriptGameObject::GetCondition	() const
{
	CInventoryItem		*inventory_item = smart_cast<CInventoryItem*>(&object());
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member Cost!");
		return			(false);
	}
	return				(inventory_item->GetCondition());
}

void CScriptGameObject::eat				(CScriptGameObject *item)
{
	if(!item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member eat!");
		return;
	}

	CInventoryItem		*inventory_item = smart_cast<CInventoryItem*>(&item->object());
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member eat!");
		return;
	}

	CInventoryOwner		*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member eat!");
		return;
	}
	
	inventory_owner->inventory().Eat(inventory_item);
}

bool CScriptGameObject::inside					(const Fvector &position, float epsilon) const
{
	CSpaceRestrictor		*space_restrictor = smart_cast<CSpaceRestrictor*>(&object());
	if (!space_restrictor) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSpaceRestrictor : cannot access class member inside!");
		return			(false);
	}
	return				(space_restrictor->inside(position,epsilon));
}

bool CScriptGameObject::inside					(const Fvector &position) const
{
	return				(inside(position,EPS_L));
}

void CScriptGameObject::set_patrol_extrapolate_callback(const luabind::functor<bool> &functor)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member set_patrol_extrapolate_callback!");
		return;
	}
	CScriptCallbackEx<bool>	callback;
	callback.set			(functor);
	monster->movement().patrol().set_extrapolate_callback(callback);
}

void CScriptGameObject::set_patrol_extrapolate_callback(const luabind::functor<bool> &functor, const luabind::object &object)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&this->object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member set_patrol_extrapolate_callback!");
		return;
	}
	CScriptCallbackEx<bool>	callback;
	callback.set			(functor,object);
	monster->movement().patrol().set_extrapolate_callback(callback);
}

void CScriptGameObject::set_patrol_extrapolate_callback()
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&this->object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member set_patrol_extrapolate_callback!");
		return;
	}
	monster->movement().patrol().set_extrapolate_callback();
}
