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
#include "script_zone.h"
#include "ai/trader/ai_trader.h"
#include "weapon.h"
#include "WeaponMagazined.h"
#include "enemy_manager.h"
#include "item_manager.h"
#include "hit_memory_manager.h"
#include "sound_memory_manager.h"
#include "explosive.h"
#include "missile.h"
#include "script_binder.h"
#include "motivation_action_manager.h"
#include "object_handler.h"
#include "script_sound_info.h"
#include "script_monster_hit_info.h"
#include "physicsshellholder.h"
#include "script_export_space.h"
#include "script_task.h"
#include "script_hit.h"
#include "inventory_item.h"
#include "weapon.h"
#include "ParticlesObject.h"
#include "PDA.h"
#include "inventory.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/biting/ai_biting.h"
#include "cover_point.h"
#include "cover_evaluators.h"
#include "script_ini_file.h"
#include "xrmessages.h"
#include "AI_PhraseDialogManager.h"
#include "character_info.h"
#include "helicopter.h"
#include "ai/bloodsucker/ai_bloodsucker.h"
#include "cover_manager.h"
#include "attachable_item.h"
#include "helicopter.h"


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

BIND_FUNCTION02	(m_tpGameObject,	CScriptGameObject::SetScriptControl,	CScriptMonster,	SetScriptControl,	bool,								LPCSTR,					bool,					ref_str);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetScriptControl,	CScriptMonster,	GetScriptControl,	bool,								false);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetScriptControlName,CScriptMonster,GetScriptControlName,LPCSTR,							"");
//BIND_FUNCTION02	(m_tpGameObject,	CScriptGameObject::AddAction,			CScriptMonster,	AddAction,			const CScriptEntityAction *,				bool,					const CScriptEntityAction *,	bool);
//BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetCurrentAction,	CScriptMonster,	GetCurrentAction,	const CScriptEntityAction *,				0);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetEnemyStrength,	CScriptMonster,	get_enemy_strength,	int,								0);
BIND_FUNCTION10	(m_tpGameObject,	CScriptGameObject::GetActionCount,		CScriptMonster,	GetActionCount,		u32,					0);
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
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

// CCustomMonster
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

// CAI_Stalker
void CScriptGameObject::UseObject(const CScriptGameObject *tpLuaGameObject)
{
	CCustomMonster		*l_tpCustomMonster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (l_tpCustomMonster)
		l_tpCustomMonster->UseObject(tpLuaGameObject->m_tpGameObject);
	else
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member UseObject!");
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

void CScriptGameObject::SetQueueSize(u32 queue_size)
{
	CWeaponMagazined	*weapon = smart_cast<CWeaponMagazined*>(m_tpGameObject);
	if (!weapon)
		return;
	weapon->SetQueueSize(queue_size);
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
// Inventory Owner
//////////////////////////////////////////////////////////////////////////
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

void CScriptGameObject::bind_object			(CScriptBinderObject *object)
{
	m_tpGameObject->set_object	(object);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::Hit(CScriptHit &tLuaHit)
{
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_HIT,m_tpGameObject->ID());
	P.w_u16			(u16(tLuaHit.m_tpDraftsman->ID()));
	P.w_u16			(0);
	P.w_dir			(tLuaHit.m_tDirection);
	P.w_float		(tLuaHit.m_fPower);
	CKinematics		*V = PKinematics(m_tpGameObject->Visual());
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

#pragma todo("Dima to Dima : find out why user defined conversion operators work incorrect")

CScriptGameObject::operator CObject*()
{
	return			(m_tpGameObject);
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

	CAI_Biting *l_tpMonster = smart_cast<CAI_Biting *>(m_tpGameObject);
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

u32	CScriptGameObject::level_vertex_id		() const
{
	return						(m_tpGameObject->level_vertex_id());
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

CScriptBinderObject	*CScriptGameObject::binded_object	()
{
	return			(m_tpGameObject->object());
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

void CScriptGameObject::enable_memory_object	(CScriptGameObject *object, bool enable)
{
	CMemoryManager	*memory_manager = smart_cast<CMemoryManager*>(m_tpGameObject);
	if (!memory_manager)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member enable_memory_object!");
	else
		memory_manager->enable				(object->m_tpGameObject,enable);
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

CScriptIniFile *CScriptGameObject::spawn_ini			() const
{
	return			((CScriptIniFile*)m_tpGameObject->spawn_ini());
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
/*
void CScriptGameObject::air_attack (CScriptGameObject * object)
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member do_hunt!");
		NODEFAULT;
	}
	helicopter->doHunt(object->m_tpGameObject);
}

void CScriptGameObject::air_attack_wait (CScriptGameObject* object, float dist, float t)
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member air_attack_wait!");
		NODEFAULT;
	}
	helicopter->doHunt2(object->m_tpGameObject,dist, t);
}

bool CScriptGameObject::air_attack_active ()
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member air_attack_active!");
		NODEFAULT;
		return true;
	}
	return !!helicopter->isOnAttack();
}

void CScriptGameObject::heli_goto_stay_point (float time)
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member heli_goto_stay_point!");
		NODEFAULT;
	}
	helicopter->gotoStayPoint(time);
}

void CScriptGameObject::heli_goto_stay_point (Fvector& pos, float time)
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member heli_goto_stay_point!");
		NODEFAULT;
	}
	helicopter->gotoStayPoint(time, &pos);
}

void CScriptGameObject::heli_go_patrol(float time)
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member heli_go_patrol!");
		NODEFAULT;
	}
	helicopter->goPatrol(time);
}

void CScriptGameObject::heli_go_to_point(Fvector& pos, Fvector& via, float time)
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member heli_go_to_point!");
		NODEFAULT;
	}
	helicopter->goToPoint(&pos, &via, time);
}

float CScriptGameObject::heli_last_point_time ()
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member heli_last_point_time!");
		NODEFAULT;
		return true;
	}
	return helicopter->getLastPointTime();
}

void CScriptGameObject::heli_go_by_patrol_path	(LPCSTR path_name)
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member heli_go_by_patrol_path!");
		NODEFAULT;
	}
	return helicopter->goPatrolByPatrolPath(path_name);
}
*/
CHelicopter* CScriptGameObject::get_helicopter	()
{
	CHelicopter		*helicopter = smart_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member get_helicopter!");
		NODEFAULT;
	}
	return helicopter;
}

/*
void				heli_use_rocket			(bool b);
bool				heli_is_use_rocket		()const;
void				heli_use_mgun			(bool b);
bool				heli_use_mgun			()const;*/


Fvector	CScriptGameObject::bone_position	(LPCSTR bone_name) const
{
	u16					bone_id;
	if (xr_strlen(bone_name))
		bone_id			= PKinematics(m_tpGameObject->Visual())->LL_BoneID(bone_name);
	else
		bone_id			= PKinematics(m_tpGameObject->Visual())->LL_GetBoneRoot();

	Fmatrix				matrix;
	matrix.mul_43		(m_tpGameObject->XFORM(),PKinematics(m_tpGameObject->Visual())->LL_GetBoneInstance(bone_id).mTransform);
	return				(matrix.c);
}

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


void CScriptGameObject::ChangeTeam(u8 team, u8 squad, u8 group)
{
	CCustomMonster				*custom_monster = smart_cast<CCustomMonster*>(m_tpGameObject);
	if (!custom_monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CCustomMonster: cannot access class member ChangeTeam!");
	else
		custom_monster->ChangeTeam(team,squad,group);
}

CPhysicsShell* CScriptGameObject::get_physics_shell() const
{
	CPhysicsShellHolder* ph_shell_holder =smart_cast<CPhysicsShellHolder*>(m_tpGameObject);
	if(! ph_shell_holder) return NULL;
	return ph_shell_holder->PPhysicsShell();
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
//////////////////////////////////////////////////////////////////////////


void CScriptGameObject::add_restrictions		(LPCSTR in, LPCSTR out)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member add_restrictions!");
		return;
	}
	restricted_object->add_restrictions		(in,out);
}

void CScriptGameObject::remove_restrictions		(LPCSTR in, LPCSTR out)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member remove_restrictions!");
		return;
	}
	restricted_object->remove_restrictions	(in,out);
}

void CScriptGameObject::remove_all_restrictions	()
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member remove_all_restrictions!");
		return;
	}
	restricted_object->remove_all_restrictions	();
}

LPCSTR CScriptGameObject::in_restrictions	()
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member in_restrictions!");
		return								("");
	}
	return									(*restricted_object->in_restrictions());
}

LPCSTR CScriptGameObject::out_restrictions	()
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member out_restrictions!");
		return								("");
	}
	return									(*restricted_object->out_restrictions());
}

bool CScriptGameObject::accessible_position	(const Fvector &position)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(false);
	}
	return									(restricted_object->accessible(position));
}

bool CScriptGameObject::accessible_vertex_id(u32 level_vertex_id)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(false);
	}
	return									(restricted_object->accessible(level_vertex_id));
}

u32	 CScriptGameObject::accessible_nearest	(const Fvector &position, Fvector &result)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(u32(-1));
	}
	return									(restricted_object->accessible_nearest(position,result));
}

bool CScriptGameObject::limping				() const
{
	CEntityCondition						*entity_condition = smart_cast<CEntityCondition*>(m_tpGameObject);
	if (!entity_condition) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member limping!");
		return								(false);
	}
	return									(entity_condition->IsLimping());
}

void CScriptGameObject::enable_vision			(bool value)
{
	CVisualMemoryManager					*visual_memory_manager = smart_cast<CVisualMemoryManager*>(m_tpGameObject);
	if (!visual_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CVisualMemoryManager : cannot access class member enable_vision!");
		return;
	}
	visual_memory_manager->enable			(value);
}

bool CScriptGameObject::vision_enabled			() const
{
	CVisualMemoryManager					*visual_memory_manager = smart_cast<CVisualMemoryManager*>(m_tpGameObject);
	if (!visual_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CVisualMemoryManager : cannot access class member vision_enabled!");
		return								(false);
	}
	return									(visual_memory_manager->enabled());
}

void CScriptGameObject::set_sound_threshold		(float value)
{
	CSoundMemoryManager						*sound_memory_manager = smart_cast<CSoundMemoryManager*>(m_tpGameObject);
	if (!sound_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSoundMemoryManager : cannot access class member set_sound_threshold!");
		return;
	}
	sound_memory_manager->set_threshold		(value);
}

void CScriptGameObject::restore_sound_threshold	()
{
	CSoundMemoryManager						*sound_memory_manager = smart_cast<CSoundMemoryManager*>(m_tpGameObject);
	if (!sound_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSoundMemoryManager : cannot access class member restore_sound_threshold!");
		return;
	}
	sound_memory_manager->restore_threshold	();
}

void CScriptGameObject::enable_attachable_item	(bool value)
{
	CAttachableItem							*attachable_item = smart_cast<CAttachableItem*>(m_tpGameObject);
	if (!attachable_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAttachableItem : cannot access class member enable_attachable_item!");
		return;
	}
	attachable_item->enable					(value);
}

bool CScriptGameObject::attachable_item_enabled	() const
{
	CAttachableItem							*attachable_item = smart_cast<CAttachableItem*>(m_tpGameObject);
	if (!attachable_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAttachableItem : cannot access class member attachable_item_enabled!");
		return								(false);
	}
	return									(attachable_item->enabled());
}
