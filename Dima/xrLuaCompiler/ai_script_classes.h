////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_classes.h
//	Created 	: 25.09.2003
//  Modified 	: 25.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"
#include "ai_script_bind.h"
#include "ai_script_actions.h"
#include "script_zone.h"
#include "ai/trader/ai_trader.h"
#include "weapon.h"
#include "WeaponMagazined.h"
#include "enemy_manager.h"
#include "item_manager.h"
#include "hit_memory_manager.h"
#include "sound_memory_manager.h"
#include "missile.h"
#include "script_binder.h"
#include "motivation_action_manager.h"
#ifdef OLD_OBJECT_HANDLER
	#include "object_handler.h"
#else
	#include "object_handler_goap.h"
#endif

#include "ai_script_snd_info.h"

class CInventoryItem;
class CLuaHit;
class CAI_Stalker;

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
	BIND_FUNCTION10	(m_tpGameObject,	Direction,			CGameObject,	Direction,			Fvector,						Fvector());
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

	IC		ref_str			cName				() const
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
	BIND_MEMBER		(m_tpGameObject,	DeathTime,			CEntity,		m_dwDeathTime,		ALife::_TIME_ID,						ALife::_TIME_ID(-1));
	BIND_FUNCTION10	(m_tpGameObject,	Armor,				CEntity,		g_Armor,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	MaxHealth,			CEntity,		g_MaxHealth,		float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Accuracy,			CInventoryOwner,GetWeaponAccuracy,	float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Team,				CEntity,		g_Team,				int,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Squad,				CEntity,		g_Squad,			int,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Group,				CEntity,		g_Group,			int,							-1);

	IC		void			Kill		(CLuaGameObject* who)
	{
		CEntity				*l_tpEntity = dynamic_cast<CEntity*>(m_tpGameObject);
		if (!l_tpEntity) {
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"%s cannot access class member Kill!",*m_tpGameObject->cName());
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
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member Alive!");
			return			(false);
		}
		return				(!!l_tpEntityAlive->g_Alive());
	}

	IC		void			GetRelationType		(CLuaGameObject* who)
	{
		CEntityAlive		*l_tpEntityAlive1 = dynamic_cast<CEntityAlive*>(m_tpGameObject);
		if (!l_tpEntityAlive1) {
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"%s cannot access class member!",*m_tpGameObject->cName());
			return;
		}
		
		CEntityAlive		*l_tpEntityAlive2 = dynamic_cast<CEntityAlive*>(who->m_tpGameObject);
		if (!l_tpEntityAlive2) {
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"%s cannot access class member!",*who->m_tpGameObject->cName());
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
	
	BIND_FUNCTION10	(m_tpGameObject,	GetEnemyStrength,	CScriptMonster,	get_enemy_strength,	int,								0);



	IC		CEntityAction	*GetCurrentAction	() const
	{
		CScriptMonster		*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member GetCurrentAction!");
		else
			if (l_tpScriptMonster->GetCurrentAction())
				return		(xr_new<CEntityAction>(l_tpScriptMonster->GetCurrentAction()));
		return				(0);
	}

	IC		void			AddAction	(const CEntityAction *tpEntityAction, bool bHighPriority = false)
	{
		CScriptMonster		*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member AddAction!");
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
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member CheckObjectVisibility!");
			return			(false);
		}
	}

	IC		bool			CheckTypeVisibility(const char *section_name)
	{
		CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
		if (l_tpCustomMonster)
			return			(l_tpCustomMonster->CheckTypeVisibility(section_name));
		else {
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member CheckTypeVisibility!");
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
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member  WhoHitName()");
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
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member  WhoHitName()");
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
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member UseObject!");
	}

			CLuaGameObject	*GetCurrentWeapon	() const;
			CLuaGameObject	*GetCurrentEquipment() const;
			CLuaGameObject	*GetFood			() const;
			CLuaGameObject	*GetMedikit			() const;

	// CInventoryOwner
	
	//передача порции информации InventoryOwner
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
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
		else if (l_tpScriptZone) l_tpScriptZone->set_callback(tpZoneCallback,bOnEnter);
		else l_tpTrader->set_callback(tpZoneCallback,bOnEnter);
	}

	void SetCallback(const luabind::object &object, LPCSTR method, bool bOnEnter)
	{
		CScriptZone	*l_tpScriptZone = dynamic_cast<CScriptZone*>(m_tpGameObject);
		CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

		if (!l_tpScriptZone && !l_tpTrader)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
		else if (l_tpScriptZone) l_tpScriptZone->set_callback(object,method,bOnEnter);
		else l_tpTrader->set_callback(object,method,bOnEnter);
	}

	void ClearCallback(bool bOnEnter)
	{
		CScriptZone	*l_tpScriptZone = dynamic_cast<CScriptZone*>(m_tpGameObject);
		CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

		if (!l_tpScriptZone && !l_tpTrader)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone : cannot access class member set_callback!");
		else if (l_tpScriptZone)l_tpScriptZone->clear_callback(bOnEnter);
		else l_tpTrader->clear_callback(bOnEnter);
	}

	void SetTradeCallback(const luabind::functor<void> &tpTradeCallback) {
		CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

		if (!l_tpTrader) 
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
		else l_tpTrader->set_trade_callback(tpTradeCallback);
	}
	
	void SetTradeCallback(const luabind::object &object, LPCSTR method) {
		CAI_Trader	*l_tpTrader	= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

		if (!l_tpTrader) 
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
		else l_tpTrader->set_trade_callback(object, method);
	}


	void ClearTradeCallback() {
		CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

		if (!l_tpTrader) 
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member clear_trade_callback!");
		else l_tpTrader->clear_trade_callback();
	}

	LPCSTR	GetPatrolPathName	();

	void SetCallback(const luabind::object &lua_object, LPCSTR method, const CScriptMonster::EActionType tActionType)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_callback!");
		else
			l_tpScriptMonster->set_callback(lua_object,method,tActionType);
	}

	void SetCallback(const luabind::functor<void> &lua_function, const CScriptMonster::EActionType tActionType)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_callback!");
		else
			l_tpScriptMonster->set_callback(lua_function,tActionType);
	}
	
	void ClearCallback(const CScriptMonster::EActionType tActionType)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member clear_callback!");
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
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member GetActionByIndex!");
			return			(0);
		}
		else
			return			(l_tpScriptMonster->GetActionByIndex(action_index));
	}

	void SetSoundCallback(const luabind::object &lua_object, LPCSTR method)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_sound_callback!");
		else
			l_tpScriptMonster->set_sound_callback(lua_object,method);
	}

	void SetSoundCallback(const luabind::functor<void> &lua_function)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_sound_callback!");
		else
			l_tpScriptMonster->set_sound_callback(lua_function);
	}
	
	void ClearSoundCallback(bool member_callback)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member clear_hit_callback!");
		else
			l_tpScriptMonster->clear_sound_callback(member_callback);
	}

	void SetHitCallback(const luabind::object &lua_object, LPCSTR method)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_hit_callback!");
		else
			l_tpScriptMonster->set_hit_callback(lua_object,method);
	}

	void SetHitCallback(const luabind::functor<void> &lua_function)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_hit_callback!");
		else
			l_tpScriptMonster->set_hit_callback(lua_function);
	}
	
	void ClearHitCallback(bool member_callback)
	{
		CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member clear_hit_callback!");
		else
			l_tpScriptMonster->clear_hit_callback(member_callback);
	}

//////////////////////////////////////////////////////////////////////////
// Inventory Owner
//////////////////////////////////////////////////////////////////////////
	void SetPdaCallback(const luabind::functor<void> &lua_function)
	{
		CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
		if (!pInvOwner)
			ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_pda_callback!");
		else
			pInvOwner->set_pda_callback(lua_function);
	}
	void ClearPdaCallback()
	{
		CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
		if (!pInvOwner)
			ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member clear_pda_callback!");
		else
			pInvOwner->clear_pda_callback();
	}

	void SetInfoCallback(const luabind::functor<void> &lua_function)
	{
		CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
		if (!pInvOwner)
			ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member set_info_callback!");
		else
			pInvOwner->set_info_callback(lua_function);
	}
	void ClearInfoCallback()
	{
		CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
		if (!pInvOwner)
			ai().script_engine().script_log 	(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member clear_info_callback!");
		else
			pInvOwner->clear_info_callback();
	}

	

	MemorySpace::CMemoryInfo *memory(const CLuaGameObject &lua_game_object)
	{
		CMemoryManager	*memory_manager = dynamic_cast<CMemoryManager*>(m_tpGameObject);
		if (!memory_manager) {
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member memory!");
			return			(0);
		}
		else
			return			(xr_new<MemorySpace::CMemoryInfo>(memory_manager->memory(lua_game_object.m_tpGameObject)));
	}

	CLuaGameObject *best_weapon()
	{
#ifdef OLD_OBJECT_HANDLER
		CObjectHandler		*object_handler = dynamic_cast<CObjectHandler*>(m_tpGameObject);
#else
		CObjectHandlerGOAP	*object_handler = dynamic_cast<CObjectHandlerGOAP*>(m_tpGameObject);
#endif
		if (!object_handler) {
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member best_weapon!");
			return			(0);
		}
		else {
			CGameObject		*game_object = object_handler->best_weapon();
			return			(game_object ? game_object->lua_game_object() : 0);
		}
	}

			void explode	(u32 level_time)
	{
		CMissile			*missile = dynamic_cast<CMissile*>(m_tpGameObject);
		if (!missile)
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member explode!");
		else {
			NET_Packet			P;
			missile->u_EventGen	(P,GE_GRENADE_EXPLODE,missile->ID());	
			missile->u_EventSend(P);
		}
	}

			CLuaGameObject		*GetEnemy			() const;
			CLuaGameObject		*GetCorpse			() const;
			CLuaSoundInfo		GetSoundInfo		();

	IC		void				bind_object			(CScriptBinderObject *object)
	{
		CScriptBinder			*binder = dynamic_cast<CScriptBinder*>(m_tpGameObject);
		if (!binder)
			ai().script_engine().script_log				(ScriptStorage::eLuaMessageTypeError,"CScriptBinder : cannot access class member bind_object!");
		else
			binder->set_object	(object);
	}

	IC		CMotivationActionManager<CLuaGameObject>	*motivation_action_manager()
	{
		CMotivationActionManager<CLuaGameObject>	*manager = dynamic_cast<CMotivationActionManager<CLuaGameObject>*>(m_tpGameObject);
		if (!manager)
			ai().script_engine().script_log				(ScriptStorage::eLuaMessageTypeError,"CMotivationActionManager : cannot access class member motivation_action_manager!");
		return					(manager);
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
			void				set_body_state			(EBodyState body_state);
			void				set_movement_type		(EMovementType movement_type);
			void				set_mental_state		(EMentalState mental_state);
			void				set_path_type			(CMovementManager::EPathType path_type);
			void				set_detail_path_type	(CMovementManager::EDetailPathType detail_path_type);
			void				add_sound				(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name = "bip01_head");
			void				remove_sound			(u32 internal_type);
			void				set_sound_mask			(u32 sound_mask);
			void				set_sight				(SightManager::ESightType tLookType, const Fvector	*tPointToLook = 0, u32 dwLookOverDelay = u32(-1));
			u32					GetRank					();
//			void				play_sound				(u32 internal_type);
//			void				play_sound				(u32 internal_type, u32 max_start_time);
//			void				play_sound				(u32 internal_type, u32 max_start_time, u32 min_start_time);
//			void				play_sound				(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time);
//			void				play_sound				(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time);
			void				play_sound				(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id);
			void				set_item				(MonsterSpace::EObjectAction object_action);
			void				set_item				(MonsterSpace::EObjectAction object_action, CLuaGameObject *game_object);
			void				set_desired_position	();
			void				set_desired_position	(const Fvector *desired_position);
			void				set_desired_direction	();
			void				set_desired_direction	(const Fvector *desired_direction);
			void				set_node_evaluator		();
			void				set_node_evaluator		(CAbstractVertexEvaluator *node_evaluator);
			void				set_path_evaluator		();
			void				set_path_evaluator		(CAbstractVertexEvaluator *path_evaluator);
			void				set_patrol_path			(LPCSTR path_name, const CMovementManager::EPatrolStartType patrol_start_type, const CMovementManager::EPatrolRouteType patrol_route_type, bool random);
			void				set_dest_level_vertex_id(u32 level_vertex_id);
			u32					level_vertex_id			() const;
};