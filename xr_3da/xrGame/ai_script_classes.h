////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_classes.h
//	Created 	: 25.09.2003
//  Modified 	: 25.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"
#include "ai_script_bind.h"
#include "ai_script_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "script_zone.h"
#include "ai/trader/ai_trader.h"
#include "weapon.h"
#include "WeaponMagazined.h"
#include "enemy_manager.h"
#include "item_manager.h"
#include "hit_memory_manager.h"
#include "sound_memory_manager.h"

class CInventoryItem;
class CLuaHit;

class CLuaGameObject {
	CGameObject			*m_tpGameObject;
public:

							CLuaGameObject		(CGameObject *tpGameObject)
	{
		m_tpGameObject	= tpGameObject;
		R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
	}

							CLuaGameObject		(const CLuaGameObject *tpLuaGameObject)
	{
		m_tpGameObject	= tpLuaGameObject->m_tpGameObject;
		R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
	}
						
							CLuaGameObject		(LPCSTR caObjectName)
	{
		m_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
		R_ASSERT2		(m_tpGameObject,"Null actual object passed!");
	}

	virtual					~CLuaGameObject		()
	{
	}

							operator CObject*	();

	IC		CGameObject		*object				()
	{
		return				(m_tpGameObject);
	}

	IC		CLuaGameObject	*Parent				() const
	{
		CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(m_tpGameObject->H_Parent());
		if (l_tpGameObject)
			return		(l_tpGameObject->lua_game_object());
		else
			return		(0);
	}

			void			Hit					(CLuaHit &tLuaHit);

	// CGameObject
	BIND_MEMBER		(m_tpGameObject,	ClassID,			CGameObject,	SUB_CLS_ID,			CLASS_ID,						CLASS_ID(-1));
	BIND_FUNCTION10	(m_tpGameObject,	Position,			CGameObject,	Position,			Fvector,						Fvector());
	BIND_FUNCTION10	(m_tpGameObject,	Mass,				CGameObject,	GetMass,			float,							float(-1));
	BIND_FUNCTION10	(m_tpGameObject,	ID,					CGameObject,	ID,					u32,							u32(-1));
	BIND_FUNCTION10	(m_tpGameObject,	getVisible,			CGameObject,	getVisible,			BOOL,							FALSE);
	BIND_FUNCTION01	(m_tpGameObject,	setVisible,			CGameObject,	setVisible,			BOOL,							BOOL);
	BIND_FUNCTION10	(m_tpGameObject,	getEnabled,			CGameObject,	getEnabled,			BOOL,							FALSE);
	BIND_FUNCTION01	(m_tpGameObject,	setEnabled,			CGameObject,	setEnabled,			BOOL,							BOOL);
	
	IC		LPCSTR			Name				() const
	{
		VERIFY			(m_tpGameObject);
		return			(*m_tpGameObject->cName());
	}

	IC		LPCSTR			Section				() const
	{
		VERIFY			(m_tpGameObject);
		return			(*m_tpGameObject->cNameSect());
	}

	// CInventoryItem
	u32				Cost			() const;
	float			GetCondition	() const;

	// CEntity
	BIND_MEMBER		(m_tpGameObject,	DeathTime,			CEntity,		m_dwDeathTime,		_TIME_ID,						_TIME_ID(-1));
	BIND_FUNCTION10	(m_tpGameObject,	Armor,				CEntity,		g_Armor,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	MaxHealth,			CEntity,		g_MaxHealth,		float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Accuracy,			CEntity,		g_Accuracy,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Team,				CEntity,		g_Team,				int,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Squad,				CEntity,		g_Squad,			int,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Group,				CEntity,		g_Group,			int,							-1);

	IC		void			Kill		(CLuaGameObject* who)
	{
		CEntity				*l_tpEntity = dynamic_cast<CEntity*>(m_tpGameObject);
		if (!l_tpEntity) {
			LuaOut			(Lua::eLuaMessageTypeError,"%s cannot access class member Kill!",*m_tpGameObject->cName());
			return;
		}
		l_tpEntity->KillEntity	(who ? who->m_tpGameObject : 0);
	}

	// CEntityAlive
	BIND_FUNCTION10	(m_tpGameObject,	GetFOV,				CEntityAlive,	ffGetFov,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	GetRange,			CEntityAlive,	ffGetRange,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	GetHealth,			CEntityAlive,	GetHealth,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	GetPower,			CEntityAlive,	GetPower,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	GetSatiety,			CEntityAlive,	GetSatiety,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	GetRadiation,		CEntityAlive,	GetRadiation,		float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	GetCircumspection,	CEntityAlive,	GetCircumspection,	float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	GetMorale,			CEntityAlive,	GetEntityMorale,	float,							-1);
	BIND_FUNCTION01	(m_tpGameObject,	SetHealth,			CEntityAlive,	ChangeHealth,		float,							float);
	BIND_FUNCTION01	(m_tpGameObject,	SetPower,			CEntityAlive,	ChangePower,		float,							float);
	BIND_FUNCTION01	(m_tpGameObject,	SetSatiety,			CEntityAlive,	ChangeSatiety,		float,							float);
	BIND_FUNCTION01	(m_tpGameObject,	SetRadiation,		CEntityAlive,	ChangeRadiation,	float,							float);
	BIND_FUNCTION01	(m_tpGameObject,	SetCircumspection,	CEntityAlive,	ChangeCircumspection,float,							float);
	BIND_FUNCTION01	(m_tpGameObject,	SetMorale,			CEntityAlive,	ChangeEntityMorale,	float,							float);

	IC		bool			Alive	() const
	{
		CEntityAlive		*l_tpEntityAlive = dynamic_cast<CEntityAlive*>(m_tpGameObject);
		if (!l_tpEntityAlive) {
			LuaOut			(Lua::eLuaMessageTypeError,"CSciptMonster : cannot access class member Alive!");
			return			(false);
		}
		return				(!!l_tpEntityAlive->g_Alive());
	}

	IC		void			GetRelationType		(CLuaGameObject* who)
	{
		CEntityAlive		*l_tpEntityAlive1 = dynamic_cast<CEntityAlive*>(m_tpGameObject);
		if (!l_tpEntityAlive1) {
			LuaOut			(Lua::eLuaMessageTypeError,"%s cannot access class member!",*m_tpGameObject->cName());
			return;
		}
		
		CEntityAlive		*l_tpEntityAlive2 = dynamic_cast<CEntityAlive*>(who->m_tpGameObject);
		if (!l_tpEntityAlive2) {
			LuaOut			(Lua::eLuaMessageTypeError,"%s cannot access class member!",*who->m_tpGameObject->cName());
			return;
		}
		
		l_tpEntityAlive1->tfGetRelationType(l_tpEntityAlive2);
	}

	// CScriptMonster
	
	BIND_FUNCTION02	(m_tpGameObject,	SetScriptControl,	CScriptMonster,	SetScriptControl,	bool,								LPCSTR,					bool,					ref_str);
	BIND_FUNCTION10	(m_tpGameObject,	GetScriptControl,	CScriptMonster,	GetScriptControl,	bool,								false);
	BIND_FUNCTION10	(m_tpGameObject,	GetScriptControlName,CScriptMonster,GetScriptControlName,LPCSTR,							"");
//	BIND_FUNCTION02	(m_tpGameObject,	AddAction,			CScriptMonster,	AddAction,			const CEntityAction *,				bool,					const CEntityAction *,	bool);
//	BIND_FUNCTION10	(m_tpGameObject,	GetCurrentAction,	CScriptMonster,	GetCurrentAction,	const CEntityAction *,				0);

	IC		CEntityAction	*GetCurrentAction	() const
	{
		CScriptMonster		*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut		(Lua::eLuaMessageTypeError,"CSciptMonster : cannot access class member GetCurrentAction!");
		else
			if (l_tpScriptMonster->GetCurrentAction())
				return		(xr_new<CEntityAction>(l_tpScriptMonster->GetCurrentAction()));
		return				(0);
	}

	IC		void			AddAction	(const CEntityAction *tpEntityAction, bool bHighPriority = false)
	{
		CScriptMonster		*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut		(Lua::eLuaMessageTypeError,"CSciptMonster : cannot access class member AddAction!");
		else
			l_tpScriptMonster->AddAction(tpEntityAction, bHighPriority);
	}

	// CCustomMonster
	IC		bool			CheckObjectVisibility(const CLuaGameObject *tpLuaGameObject)
	{
		CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
		if (l_tpCustomMonster)
			return			(l_tpCustomMonster->CheckObjectVisibility(tpLuaGameObject->m_tpGameObject));
		else {
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member CheckObjectVisibility!");
			return			(false);
		}
	}

	IC		bool			CheckTypeVisibility(const char *section_name)
	{
		CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
		if (l_tpCustomMonster)
			return			(l_tpCustomMonster->CheckTypeVisibility(section_name));
		else {
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member CheckTypeVisibility!");
			return			(false);
		}
	}

	IC	const char* WhoHitName()
	{
		CEntityCondition *pEntityCondition = 
						  dynamic_cast<CEntityCondition*>(m_tpGameObject);
		
		if (pEntityCondition)
			return			(*pEntityCondition->GetWhoHitLastTime()->cName());
		else 
		{
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member  WhoHitName()");
			return			NULL;
		}
	}

	IC	const char* WhoHitSectionName()
	{
		CEntityCondition *pEntityCondition = 
						  dynamic_cast<CEntityCondition*>(m_tpGameObject);
		
		if (pEntityCondition)
			return			(*pEntityCondition->GetWhoHitLastTime()->cNameSect());
		else 
		{
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member  WhoHitName()");
			return			NULL;
		}
	}

	// CAI_Stalker
	IC		void			UseObject(const CLuaGameObject *tpLuaGameObject)
	{
		CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
		if (l_tpCustomMonster)
			l_tpCustomMonster->UseObject(tpLuaGameObject->m_tpGameObject);
		else
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member UseObject!");
	}

	BIND_FUNCTION10	(m_tpGameObject,	GetRank,			CAI_Stalker,	GetRank,			u32,								eStalkerRankDummy);
	
			CLuaGameObject	*GetCurrentWeapon	() const;
			CLuaGameObject	*GetCurrentEquipment() const;
			CLuaGameObject	*GetFood			() const;
			CLuaGameObject	*GetMedikit			() const;

	// CInventoryOwner
	
	//�������� ������ ���������� InventoryOwner
	bool GiveInfoPortion(int info_index);
	bool GiveInfoPortionViaPda(int info_index, CLuaGameObject* pFromWho);
	
	u32				GetInventoryObjectCount() const;

	CLuaGameObject	*GetActiveItem();

	CLuaGameObject	*GetObjectByName	(LPCSTR caObjectName) const;
	CLuaGameObject	*GetObjectByIndex	(int iIndex) const;

	void SetCallback(const luabind::functor<void> &tpZoneCallback, bool bOnEnter)
	{
		CScriptZone	*l_tpScriptZone = dynamic_cast<CScriptZone*>(m_tpGameObject);
		CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

		if (!l_tpScriptZone && !l_tpTrader)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptZone : cannot access class member set_callback!");
		else if (l_tpScriptZone) l_tpScriptZone->set_callback(tpZoneCallback,bOnEnter);
		else l_tpTrader->set_callback(tpZoneCallback,bOnEnter);
	}

	void ClearCallback(bool bOnEnter)
	{
		CScriptZone	*l_tpScriptZone = dynamic_cast<CScriptZone*>(m_tpGameObject);
		CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

		if (!l_tpScriptZone && !l_tpTrader)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptZone : cannot access class member set_callback!");
		else if (l_tpScriptZone)l_tpScriptZone->clear_callback(bOnEnter);
		else l_tpTrader->clear_callback(bOnEnter);
	}

	void SetTradeCallback(const luabind::functor<void> &tpTradeCallback) {
		CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

		if (!l_tpTrader) 
			LuaOut			(Lua::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
		else l_tpTrader->set_trade_callback(tpTradeCallback);
	}
	
	void ClearTradeCallback() {
		CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

		if (!l_tpTrader) 
			LuaOut			(Lua::eLuaMessageTypeError,"CAI_Trader : cannot access class member clear_trade_callback!");
		else l_tpTrader->clear_trade_callback();
	}

	BIND_FUNCTION10	(m_tpGameObject,	GetPatrolPathName,		CScriptMonster,	GetPatrolPathName,		LPCSTR,								"");

	void SetCallback(const luabind::object &lua_object, LPCSTR method, const CScriptMonster::EActionType tActionType)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_callback!");
		else
			l_tpScriptMonster->set_callback(lua_object,method,tActionType);
	}

	void SetCallback(const luabind::functor<void> &lua_function, const CScriptMonster::EActionType tActionType)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_callback!");
		else
			l_tpScriptMonster->set_callback(lua_function,tActionType);
	}
	
	void ClearCallback(const CScriptMonster::EActionType tActionType)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member clear_callback!");
		else
			l_tpScriptMonster->clear_callback(tActionType);
	}

	u32 GetAmmoElapsed()
	{
		const CWeapon	*weapon = dynamic_cast<const CWeapon*>(m_tpGameObject);
		if (!weapon)
			return		(0);
		return			(weapon->GetAmmoElapsed());
	}

	u32 GetAmmoCurrent() const
	{
		const CWeapon	*weapon = dynamic_cast<const CWeapon*>(m_tpGameObject);
		if (!weapon)
			return		(0);
		return			(weapon->GetAmmoCurrent());
	}

	void SetQueueSize(u32 queue_size)
	{
		CWeaponMagazined	*weapon = dynamic_cast<CWeaponMagazined*>(m_tpGameObject);
		if (!weapon)
			return;
		weapon->SetQueueSize(queue_size);
	}

	const CHitObject	*GetBestHit	() const
	{
		const CHitMemoryManager	*hit_memory_manager = dynamic_cast<const CHitMemoryManager*>(m_tpGameObject);
		if (!hit_memory_manager)
			return				(0);
		return					(hit_memory_manager->hit());
	}

	const CSoundObject	*GetBestSound	() const
	{
		const CSoundMemoryManager	*sound_memory_manager = dynamic_cast<const CSoundMemoryManager*>(m_tpGameObject);
		if (!sound_memory_manager)
			return				(0);
		return					(sound_memory_manager->sound());
	}

	CLuaGameObject	*GetBestEnemy()
	{
		const CEnemyManager	*enemy_manager = dynamic_cast<const CEnemyManager*>(m_tpGameObject);
		if (!enemy_manager)
			return				(0);
		return					(const_cast<CGameObject*>(dynamic_cast<const CGameObject*>(enemy_manager->selected()))->lua_game_object());
	}

	CLuaGameObject	*GetBestItem()
	{
		const CItemManager	*item_manager = dynamic_cast<const CItemManager*>(m_tpGameObject);
		if (!item_manager)
			return				(0);
		return					(const_cast<CGameObject*>(dynamic_cast<const CGameObject*>(item_manager->selected()))->lua_game_object());
	}

	BIND_FUNCTION10			(m_tpGameObject,	GetActionCount,		CScriptMonster,	GetActionCount,		u32,					0);
	
	const CEntityAction		*GetActionByIndex(u32 action_index = 0)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster) {
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member GetActionByIndex!");
			return			(0);
		}
		else
			return			(l_tpScriptMonster->GetActionByIndex(action_index));
	}

	void SetSoundCallback(const luabind::object &lua_object, LPCSTR method)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_sound_callback!");
		else
			l_tpScriptMonster->set_sound_callback(lua_object,method);
	}

	void SetSoundCallback(const luabind::functor<void> &lua_function)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_sound_callback!");
		else
			l_tpScriptMonster->set_sound_callback(lua_function);
	}
	
	void ClearSoundCallback(bool member_callback)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member clear_hit_callback!");
		else
			l_tpScriptMonster->clear_sound_callback(member_callback);
	}

	void SetHitCallback(const luabind::object &lua_object, LPCSTR method)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_hit_callback!");
		else
			l_tpScriptMonster->set_hit_callback(lua_object,method);
	}

	void SetHitCallback(const luabind::functor<void> &lua_function)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_hit_callback!");
		else
			l_tpScriptMonster->set_hit_callback(lua_function);
	}
	
	void ClearHitCallback(bool member_callback)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member clear_hit_callback!");
		else
			l_tpScriptMonster->clear_hit_callback(member_callback);
	}

	MemorySpace::CMemoryInfo *memory(const CLuaGameObject &lua_game_object)
	{
		CMemoryManager	*memory_manager = dynamic_cast<CMemoryManager*>(m_tpGameObject);
		if (!memory_manager) {
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member memory!");
			return			(0);
		}
		else
			return			(xr_new<MemorySpace::CMemoryInfo>(memory_manager->memory(lua_game_object.m_tpGameObject)));
	}

	CLuaGameObject *best_weapon()
	{
		CObjectHandler	*object_handler = dynamic_cast<CObjectHandler*>(m_tpGameObject);
		if (!object_handler) {
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member best_weapon!");
			return			(0);
		}
		else {
			CGameObject		*game_object = object_handler->best_weapon();
			return			(game_object ? game_object->lua_game_object() : 0);
		}
	}
};