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
#include "memory_manager.h"
#include "script_hit.h"
#include "xrmessages.h"
#include "inventory.h"
#include "explosive.h"
#include "object_handler.h"
#include "script_ini_file.h"
#include "script_zone.h"
#include "script_callback.h"

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
	CWeaponMagazined	*weapon = smart_cast<CWeaponMagazined*>(m_tpGameObject);
	if (!weapon)
		return;
	weapon->SetQueueSize(queue_size);
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

const CHitObject *CScriptGameObject::GetBestHit	() const
{
	const CHitMemoryManager	*hit_memory_manager = smart_cast<const CHitMemoryManager*>(m_tpGameObject);
	if (!hit_memory_manager)
		return				(0);
	return					(hit_memory_manager->hit());
}

const CSoundObject *CScriptGameObject::GetBestSound	() const
{
	const CSoundMemoryManager	*sound_memory_manager = smart_cast<const CSoundMemoryManager*>(m_tpGameObject);
	if (!sound_memory_manager)
		return				(0);
	return					(sound_memory_manager->sound());
}

CScriptGameObject *CScriptGameObject::GetBestEnemy()
{
	const CEnemyManager		*enemy_manager = smart_cast<const CEnemyManager*>(m_tpGameObject);
	if (!enemy_manager || !enemy_manager->selected())
		return				(0);
	return					(smart_cast<const CGameObject*>(enemy_manager->selected())->lua_game_object());
}

CScriptGameObject *CScriptGameObject::GetBestItem()
{
	const CItemManager		*item_manager = smart_cast<const CItemManager*>(m_tpGameObject);
	if (!item_manager || !item_manager->selected())
		return				(0);
	return					(smart_cast<const CGameObject*>(item_manager->selected())->lua_game_object());
}

MemorySpace::CMemoryInfo *CScriptGameObject::memory(const CScriptGameObject &lua_game_object)
{
	CMemoryManager	*memory_manager = smart_cast<CMemoryManager*>(m_tpGameObject);
	if (!memory_manager) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member memory!");
		return			(0);
	}
	else
		return			(xr_new<MemorySpace::CMemoryInfo>(memory_manager->memory(lua_game_object.m_tpGameObject)));
}

void CScriptGameObject::enable_memory_object	(CScriptGameObject *object, bool enable)
{
	CMemoryManager	*memory_manager = smart_cast<CMemoryManager*>(m_tpGameObject);
	if (!memory_manager)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member enable_memory_object!");
	else
		memory_manager->enable				(object->m_tpGameObject,enable);
}

const xr_vector<CNotYetVisibleObject> &CScriptGameObject::not_yet_visible_objects() const
{
	CMemoryManager	*manager = smart_cast<CMemoryManager*>(m_tpGameObject);
	if (!manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member not_yet_visible_objects!");
		NODEFAULT;
	}
	return					(manager->not_yet_visible_objects());
}

float CScriptGameObject::visibility_threshold	() const
{
	CMemoryManager	*manager = smart_cast<CMemoryManager*>(m_tpGameObject);
	if (!manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member visibility_threshold!");
		NODEFAULT;
	}
	return					(manager->visibility_threshold());
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

u32	CScriptGameObject::GetInventoryObjectCount() const
{
	CInventoryOwner		*l_tpInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner)
		return			(l_tpInventoryOwner->inventory().dwfGetObjectCount());
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member obj_count!");
		return			(0);
	}
}

CScriptGameObject	*CScriptGameObject::GetActiveItem()
{
	CInventoryOwner		*l_tpInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner)
		if (l_tpInventoryOwner->inventory().ActiveItem())
			return		(l_tpInventoryOwner->inventory().ActiveItem()->lua_game_object());
		else
			return		(0);
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member activge_item!");
		return			(0);
	}
}

CScriptGameObject	*CScriptGameObject::GetObjectByName	(LPCSTR caObjectName) const
{
	CInventoryOwner		*l_tpInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner) {
		CInventoryItem	*l_tpInventoryItem = l_tpInventoryOwner->inventory().GetItemFromInventory(caObjectName);
		CGameObject		*l_tpGameObject = smart_cast<CGameObject*>(l_tpInventoryItem);
		if (!l_tpGameObject)
			return		(0);
		else
			return		(l_tpGameObject->lua_game_object());
	}
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member object!");
		return			(0);
	}
}

CScriptGameObject	*CScriptGameObject::GetObjectByIndex	(int iIndex) const
{
	CInventoryOwner		*l_tpInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner) {
		CInventoryItem	*l_tpInventoryItem = l_tpInventoryOwner->inventory().tpfGetObjectByIndex(iIndex);
		CGameObject		*l_tpGameObject = smart_cast<CGameObject*>(l_tpInventoryItem);
		if (!l_tpGameObject)
			return		(0);
		else
			return		(l_tpGameObject->lua_game_object());
	}
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member object!");
		return			(0);	
	}
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void CScriptGameObject::Hit(CScriptHit *tpLuaHit)
{
	CScriptHit		&tLuaHit = *tpLuaHit;
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_HIT,m_tpGameObject->ID());
	P.w_u16			(u16(tLuaHit.m_tpDraftsman->ID()));
	P.w_u16			(0);
	P.w_dir			(tLuaHit.m_tDirection);
	P.w_float		(tLuaHit.m_fPower);
	CKinematics		*V = smart_cast<CKinematics*>(m_tpGameObject->Visual());
	VERIFY			(V);
	if (xr_strlen	(tLuaHit.m_caBoneName))
		P.w_s16		(V->LL_BoneID(tLuaHit.m_caBoneName));
	else
		P.w_s16		(s16(0));
	P.w_vec3		(Fvector().set(0,0,0));
	P.w_float		(tLuaHit.m_fImpulse);
	P.w_u16			(u16(tLuaHit.m_tHitType));
	m_tpGameObject->u_EventSend(P);
}

#pragma todo("Dima to Dima : find out why user defined conversion operators work incorrect")

CScriptGameObject::operator CObject*()
{
	return			(m_tpGameObject);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CScriptGameObject *CScriptGameObject::best_weapon()
{
	CObjectHandler	*object_handler = smart_cast<CObjectHandler*>(m_tpGameObject);
	if (!object_handler) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member best_weapon!");
		return			(0);
	}
	else {
		CGameObject		*game_object = object_handler->best_weapon();
		return			(game_object ? game_object->lua_game_object() : 0);
	}
}

void CScriptGameObject::set_item		(MonsterSpace::EObjectAction object_action)
{
	CObjectHandler			*object_handler = smart_cast<CObjectHandler*>(m_tpGameObject);
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action);
}

void CScriptGameObject::set_item		(MonsterSpace::EObjectAction object_action, CScriptGameObject *lua_game_object)
{
	CObjectHandler			*object_handler = smart_cast<CObjectHandler*>(m_tpGameObject);
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action,lua_game_object ? lua_game_object->object() : 0);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::explode	(u32 level_time)
{
	CExplosive			*explosive = smart_cast<CExplosive*>(m_tpGameObject);
	if (m_tpGameObject->H_Parent())
	{
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CExplosive : cannot explode object wiht parent!");
		return;
	}
	
	if (!explosive)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CExplosive : cannot access class member explode!");
	else {
		Fvector normal;
		explosive->FindNormal(normal);
		explosive->SetCurrentParentID(m_tpGameObject->ID());
		explosive->GenExplodeEvent(m_tpGameObject->Position(), normal);
	}
}

bool CScriptGameObject::active_zone_contact		(u16 id)
{
	CScriptZone		*script_zone = smart_cast<CScriptZone*>(m_tpGameObject);
	if (!script_zone) {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CScriptZone : cannot access class member active_zone_contact!");
		return		(false);
	}
	return			(script_zone->active_contact(id));
}
