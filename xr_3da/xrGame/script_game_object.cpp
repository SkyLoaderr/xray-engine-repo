////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"
#include "script_entity_action.h"
#include "ai_space.h"
#include "script_engine.h"
#include "ai/script/ai_script_monster.h"
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

class CScriptBinderObject;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::Position,			CGameObject,	Position,			Fvector,						Fvector());
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::Direction,			CGameObject,	Direction,			Fvector,						Fvector());
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::Mass,		CPhysicsShellHolder,	GetMass,			float,							float(-1));
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::ID,					CGameObject,	ID,					u32,							u32(-1));
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::getVisible,			CGameObject,	getVisible,			BOOL,							FALSE);
BIND_FUNCTION01	(m_tpGameObject,	CScriptGameObject::setVisible,			CGameObject,	setVisible,			BOOL,							BOOL);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::getEnabled,			CGameObject,	getEnabled,			BOOL,							FALSE);
BIND_FUNCTION01	(m_tpGameObject,	CScriptGameObject::setEnabled,			CGameObject,	setEnabled,			BOOL,							BOOL);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::DeathTime,			CEntity,		GetLevelDeathTime,	u32,							0);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::Armor,				CEntity,		g_Armor,			float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::MaxHealth,			CEntity,		g_MaxHealth,		float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::Accuracy,			CInventoryOwner,GetWeaponAccuracy,	float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::Team,				CEntity,		g_Team,				int,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::Squad,				CEntity,		g_Squad,			int,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::Group,				CEntity,		g_Group,			int,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetFOV,				CEntityAlive,	ffGetFov,			float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetRange,			CEntityAlive,	ffGetRange,			float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetHealth,			CEntityAlive,	GetHealth,			float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetPower,			CEntityAlive,	GetPower,			float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetSatiety,			CEntityAlive,	GetSatiety,			float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetRadiation,		CEntityAlive,	GetRadiation,		float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetCircumspection,	CEntityAlive,	GetCircumspection,	float,							-1);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetMorale,			CEntityAlive,	GetEntityMorale,	float,							-1);
BIND_FUNCTION01	(m_tpGameObject,	CScriptGameObject::SetHealth,			CEntityAlive,	ChangeHealth,		float,							float);
BIND_FUNCTION01	(m_tpGameObject,	CScriptGameObject::SetPower,			CEntityAlive,	ChangePower,		float,							float);
BIND_FUNCTION01	(m_tpGameObject,	CScriptGameObject::SetSatiety,			CEntityAlive,	ChangeSatiety,		float,							float);
BIND_FUNCTION01	(m_tpGameObject,	CScriptGameObject::SetRadiation,		CEntityAlive,	ChangeRadiation,	float,							float);
BIND_FUNCTION01	(m_tpGameObject,	CScriptGameObject::SetCircumspection,	CEntityAlive,	ChangeCircumspection,float,							float);
BIND_FUNCTION01	(m_tpGameObject,	CScriptGameObject::SetMorale,			CEntityAlive,	ChangeEntityMorale,	float,							float);
BIND_FUNCTION02	(m_tpGameObject,	CScriptGameObject::SetScriptControl,	CScriptMonster,	SetScriptControl,	bool,								LPCSTR,					bool,					shared_str);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetScriptControl,	CScriptMonster,	GetScriptControl,	bool,								false);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetScriptControlName,CScriptMonster,GetScriptControlName,LPCSTR,							"");
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetEnemyStrength,	CScriptMonster,	get_enemy_strength,	int,								0);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetActionCount,		CScriptMonster,	GetActionCount,		u32,					0);

u32	CScriptGameObject::level_vertex_id		() const
{
	return						(m_tpGameObject->level_vertex_id());
}

CScriptIniFile *CScriptGameObject::spawn_ini			() const
{
	return			((CScriptIniFile*)m_tpGameObject->spawn_ini());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CScriptEntityAction	*CScriptGameObject::GetCurrentAction	() const
{
	CScriptMonster		*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member GetCurrentAction!");
	else
		if (l_tpScriptMonster->GetCurrentAction())
			return		(xr_new<CScriptEntityAction>(l_tpScriptMonster->GetCurrentAction()));
	return				(0);
}

void CScriptGameObject::AddAction	(const CScriptEntityAction *tpEntityAction, bool bHighPriority)
{
	CScriptMonster		*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member AddAction!");
	else
		l_tpScriptMonster->AddAction(tpEntityAction, bHighPriority);
}

const CScriptEntityAction *CScriptGameObject::GetActionByIndex(u32 action_index)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member GetActionByIndex!");
		return			(0);
	}
	else
		return			(l_tpScriptMonster->GetActionByIndex(action_index));
}

void CScriptGameObject::SetCallback(const luabind::object &lua_object, LPCSTR method, const ScriptMonster::EActionType tActionType)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_callback!");
	else
		l_tpScriptMonster->set_callback(lua_object,method,tActionType);
}

void CScriptGameObject::SetCallback(const luabind::functor<void> &lua_function, const ScriptMonster::EActionType tActionType)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_callback!");
	else
		l_tpScriptMonster->set_callback(lua_function,tActionType);
}

void CScriptGameObject::ClearCallback(const ScriptMonster::EActionType tActionType)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member clear_callback!");
	else
		l_tpScriptMonster->clear_callback(tActionType);
}

void CScriptGameObject::SetSoundCallback(const luabind::object &lua_object, LPCSTR method)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_sound_callback!");
	else
		l_tpScriptMonster->set_sound_callback(lua_object,method);
}

void CScriptGameObject::SetSoundCallback(const luabind::functor<void> &lua_function)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_sound_callback!");
	else
		l_tpScriptMonster->set_sound_callback(lua_function);
}

void CScriptGameObject::ClearSoundCallback(bool member_callback)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member clear_hit_callback!");
	else
		l_tpScriptMonster->clear_sound_callback(member_callback);
}

void CScriptGameObject::SetHitCallback(const luabind::object &lua_object, LPCSTR method)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_hit_callback!");
	else
		l_tpScriptMonster->set_hit_callback(lua_object,method);
}

void CScriptGameObject::SetHitCallback(const luabind::functor<void> &lua_function)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_hit_callback!");
	else
		l_tpScriptMonster->set_hit_callback(lua_function);
}

void CScriptGameObject::ClearHitCallback(bool member_callback)
{
	CScriptMonster	*l_tpScriptMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (!l_tpScriptMonster)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member clear_hit_callback!");
	else
		l_tpScriptMonster->clear_hit_callback(member_callback);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CPhysicsShell* CScriptGameObject::get_physics_shell() const
{
	CPhysicsShellHolder* ph_shell_holder =smart_cast<CPhysicsShellHolder*>(m_tpGameObject);
	if(! ph_shell_holder) return NULL;
	return ph_shell_holder->PPhysicsShell();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CHelicopter* CScriptGameObject::get_helicopter	()
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member get_helicopter!");
		NODEFAULT;
	}
	return helicopter;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

LPCSTR CScriptGameObject::WhoHitName()
{
	CEntityCondition *pEntityCondition = 
						smart_cast<CEntityCondition*>(m_tpGameObject);
	
	if (pEntityCondition)
		return			pEntityCondition->GetWhoHitLastTime()?(*pEntityCondition->GetWhoHitLastTime()->cName()):NULL;
	else 
	{
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member  WhoHitName()");
		return			NULL;
	}
}

LPCSTR CScriptGameObject::WhoHitSectionName()
{
	CEntityCondition *pEntityCondition = 
						smart_cast<CEntityCondition*>(m_tpGameObject);
	
	if (pEntityCondition)
		return			pEntityCondition->GetWhoHitLastTime()?(*pEntityCondition->GetWhoHitLastTime()->cNameSect()):NULL;
	else 
	{
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member  WhoHitName()");
		return			NULL;
	}
}

bool CScriptGameObject::CheckObjectVisibility(const CScriptGameObject *tpLuaGameObject)
{
	CScriptMonster		*l_tpCustomMonster = smart_cast<CScriptMonster*>(m_tpGameObject);
	if (l_tpCustomMonster)
		return			(l_tpCustomMonster->CheckObjectVisibility(tpLuaGameObject->m_tpGameObject));
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
	return									(m_tpGameObject->object());
}

void CScriptGameObject::bind_object			(CScriptBinderObject *object)
{
	m_tpGameObject->set_object	(object);
}

void CScriptGameObject::set_previous_point	(int point_index)
{
	CMovementManager	*movement_manager = smart_cast<CMovementManager*>(m_tpGameObject);
	if (!movement_manager)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member set_previous_point!");
	else
		movement_manager->set_previous_point(point_index);
}

void CScriptGameObject::set_start_point	(int point_index)
{
	CMovementManager	*movement_manager = smart_cast<CMovementManager*>(m_tpGameObject);
	if (!movement_manager)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member set_start_point!");
	else
		movement_manager->set_start_point(point_index);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Fvector	CScriptGameObject::bone_position	(LPCSTR bone_name) const
{
	u16					bone_id;
	if (xr_strlen(bone_name))
		bone_id			= smart_cast<CKinematics*>(m_tpGameObject->Visual())->LL_BoneID(bone_name);
	else
		bone_id			= smart_cast<CKinematics*>(m_tpGameObject->Visual())->LL_GetBoneRoot();

	Fmatrix				matrix;
	matrix.mul_43		(m_tpGameObject->XFORM(),smart_cast<CKinematics*>(m_tpGameObject->Visual())->LL_GetBoneInstance(bone_id).mTransform);
	return				(matrix.c);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

u32 CScriptGameObject::GetAmmoElapsed()
{
	const CWeapon	*weapon = smart_cast<const CWeapon*>(m_tpGameObject);
	if (!weapon)
		return		(0);
	return			(weapon->GetAmmoElapsed());
}

void CScriptGameObject::SetAmmoElapsed(int ammo_elapsed)
{
	CWeapon	*weapon = smart_cast<CWeapon*>(m_tpGameObject);
	if (!weapon) return;
	weapon->SetAmmoElapsed(ammo_elapsed);
}

u32 CScriptGameObject::GetAmmoCurrent() const
{
	const CWeapon	*weapon = smart_cast<const CWeapon*>(m_tpGameObject);
	if (!weapon)
		return		(0);
	return			(weapon->GetAmmoCurrent());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::SetQueueSize(u32 queue_size)
{
	CWeaponMagazined		*weapon = smart_cast<CWeaponMagazined*>(m_tpGameObject);
	weapon->SetQueueSize	(queue_size);
}

////////////////////////////////////////////////////////////////////////////
//// Inventory Owner
////////////////////////////////////////////////////////////////////////////
void CScriptGameObject::SetPdaCallback(const luabind::functor<void> &lua_function)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_pda_callback!");
	else
		pInvOwner->set_pda_callback(lua_function);
}

void CScriptGameObject::SetPdaCallback(const luabind::object &instance, LPCSTR method)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_pda_callback!");
	else
		pInvOwner->set_pda_callback(instance,method);
}

void CScriptGameObject::ClearPdaCallback()
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member clear_pda_callback!");
	else
		pInvOwner->clear_pda_callback();
}

void CScriptGameObject::SetInfoCallback(const luabind::functor<void> &lua_function)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_info_callback!");
	else
		pInvOwner->set_info_callback(lua_function);
}

void CScriptGameObject::SetInfoCallback(const luabind::object &instance, LPCSTR method)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_info_callback!");
	else
		pInvOwner->set_info_callback(instance,method);
}

void CScriptGameObject::ClearInfoCallback()
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (!pInvOwner)
		ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member clear_info_callback!");
	else
		pInvOwner->clear_info_callback();
}

u32	CScriptGameObject::Cost			() const
{
	CInventoryItem		*inventory_item = smart_cast<CInventoryItem*>(m_tpGameObject);
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member Cost!");
		return			(false);
	}
	return				(inventory_item->Cost());
}

float CScriptGameObject::GetCondition	() const
{
	CInventoryItem		*inventory_item = smart_cast<CInventoryItem*>(m_tpGameObject);
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member Cost!");
		return			(false);
	}
	return				(inventory_item->GetCondition());
}
