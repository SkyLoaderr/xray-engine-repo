////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_classes.h
//	Created 	: 25.09.2003
//  Modified 	: 25.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"
#include "../effectorpp.h"
#include "ai/stalker/ai_stalker.h"
#include "script_zone.h"
#include "ai/trader/ai_trader.h"

class CLuaEffector : public CEffectorPP {
public:
	typedef CEffectorPP inherited;
	EEffectorPPType		m_tEffectorType;
	SPPInfo				m_tInfo;

					CLuaEffector				(int		iType, float time) : CEffectorPP(EEffectorPPType(iType),time)
	{
		m_tEffectorType		= EEffectorPPType(iType);
	}

	virtual			~CLuaEffector				()
	{
	}

	virtual	BOOL	Process						(SPPInfo	&pp)
	{
		return		(inherited::Process(pp));
	}

	virtual	void	Add							()
	{
		Level().Cameras.AddEffector		(this);
	}

	virtual	void	Remove							()
	{
		Level().Cameras.RemoveEffector	(m_tEffectorType);
	}
};

class CLuaEffectorWrapper : public CLuaEffector {
public:
	luabind::object		m_tLuaBindObject;

					CLuaEffectorWrapper	(luabind::object tLuaBindObject, int iType, float fTime) : CLuaEffector(iType, fTime), m_tLuaBindObject(tLuaBindObject)
	{
	}

	virtual BOOL	Process				(SPPInfo &pp)
	{
		BOOL	l_bResult = !!luabind::call_member<bool>(m_tLuaBindObject,"process",pp);
		pp		= m_tInfo;
		return	(l_bResult);
	}

	static	bool	Process_static		(CLuaEffector *tpLuaEffector, SPPInfo &pp)
	{
		return		(!!tpLuaEffector->CLuaEffector::Process(pp));
	}
};

#define DECLARE_FUNCTION10(A,D)\
	IC		D				A					() const\
	{

#define DECLARE_FUNCTION11(A,D,F)\
	IC		D				A					(F f) \
	{

#define DECLARE_FUNCTION12(A,D,F,G)\
	IC		D				A					(F f, G g) \
	{

#define DECLARE_FUNCTION13(A,D,F,G,H)\
	IC		D				A					(F f, G g, H h) \
	{

#define CAST_OBJECT(Z,A,B)\
		B				*l_tpEntity = dynamic_cast<B*>(Z);\
		if (!l_tpEntity) {\
			LuaOut		(Lua::eLuaMessageTypeError,"%s : cannot access class member %s!",#B,#A);

#define CAST_OBJECT0(Z,A,B)\
		CAST_OBJECT(Z,A,B)\
			return;\
		}

#define CAST_OBJECT1(Z,A,B,D,E)\
		CAST_OBJECT(Z,A,B)\
			return		((D)(E));\
		}

#define GET_MEMBER(C,D)\
		return			((D)(l_tpEntity->C));\
	}

#define CALL_FUNCTION10(C,D)\
		return			((D)(l_tpEntity->C()));\
	}

#define CALL_FUNCTION11(C,D,F)\
		return			((D)(l_tpEntity->C((F)(f))));\
	}

#define CALL_FUNCTION01(C,F)\
		l_tpEntity->C((F)(f));\
	}

#define CALL_FUNCTION02(C,F,G)\
		l_tpEntity->C((F)(f),(G)(g));\
	}

#define CALL_FUNCTION03(C,F,G,H)\
		l_tpEntity->C((F)(f),(G)(g),(H)(h));\
	}

#define CALL_FUNCTION00(C)\
		l_tpEntity->C();\
	}

#define BIND_MEMBER(Z,A,B,C,D,E) \
	DECLARE_FUNCTION10	(A,D)\
	CAST_OBJECT1		(Z,A,B,D,E)\
	GET_MEMBER			(C,D)

#define BIND_FUNCTION00(Z,A,B,C) \
	DECLARE_FUNCTION10	(A,void)\
	CAST_OBJECT0		(Z,A,B)\
	CALL_FUNCTION00		(C)

#define BIND_FUNCTION10(Z,A,B,C,D,E) \
	DECLARE_FUNCTION10	(A,D)\
	CAST_OBJECT1		(Z,A,B,D,E)\
	CALL_FUNCTION10		(C,D)

#define BIND_FUNCTION11(Z,A,B,C,D,E,F,I) \
	DECLARE_FUNCTION11	(A,D,F)\
	CAST_OBJECT1		(Z,A,B,D,E)\
	CALL_FUNCTION11		(C,D,I)

#define BIND_FUNCTION01(Z,A,B,C,F,I) \
	DECLARE_FUNCTION11	(A,void,F)\
	CAST_OBJECT0		(Z,A,B)\
	CALL_FUNCTION01		(C,I)

#define BIND_FUNCTION02(Z,A,B,C,F,G,I,J) \
	DECLARE_FUNCTION12	(A,void,F,G)\
	CAST_OBJECT0		(Z,A,B)\
	CALL_FUNCTION02		(C,I,J)

#define BIND_FUNCTION03(Z,A,B,C,F,G,H,I,J,K) \
	DECLARE_FUNCTION13	(A,void,F,G,H)\
	CAST_OBJECT0		(Z,A,B)\
	CALL_FUNCTION03		(C,I,J,K)

class CLuaGameObject;

class CLuaSound {
	ref_sound				*m_tpSound;
public:
								CLuaSound		(LPCSTR caSoundName)
	{
		string256			l_caFileName;
		if (FS.exist(l_caFileName,"$game_sounds$",caSoundName,".ogg"))
			::Sound->create	(*(m_tpSound = xr_new<ref_sound>()),TRUE,caSoundName);
		else {
			LuaOut			(Lua::eLuaMessageTypeError,"File not found \"%s\"!",l_caFileName);
			m_tpSound		= 0;
		}
	}

	virtual						~CLuaSound		()
	{
		xr_delete			(m_tpSound);
	}

	BIND_FUNCTION02	(m_tpSound,			Play,				ref_sound,		play,				CLuaGameObject *,		BOOL,				CObject *,				BOOL);
	BIND_FUNCTION02	(m_tpSound,			PlayUnlimited,		ref_sound,		play_unlimited,		CLuaGameObject *,		BOOL,				CObject *,				BOOL);
	BIND_FUNCTION03	(m_tpSound,			PlayAtPos,			ref_sound,		play_at_pos,		CLuaGameObject *,		const Fvector &,	BOOL,					CObject *,		const Fvector &,	BOOL);
	BIND_FUNCTION03	(m_tpSound,			PlayAtPosUnlimited,	ref_sound,		play_at_pos_unlimited,CLuaGameObject *,		const Fvector &,	BOOL,					CObject *,		const Fvector &,	BOOL);
	BIND_FUNCTION00	(m_tpSound,			Stop,				ref_sound,		stop);
	BIND_FUNCTION01	(m_tpSound,			SetPosition,		ref_sound,		set_position,		const Fvector &,		const Fvector &);
	BIND_FUNCTION01	(m_tpSound,			SetFrequency,		ref_sound,		set_frequency,		const float,			const float);
	BIND_FUNCTION01	(m_tpSound,			SetVolume,			ref_sound,		set_volume,			const float,			const float);
	BIND_FUNCTION10	(m_tpSound,			GetParams,			ref_sound,		get_params,			const CSound_params *,	0);
	BIND_FUNCTION01	(m_tpSound,			SetParams,			ref_sound,		set_params,			CSound_params *,		CSound_params *);

			 void				SetMinDistance	(const float fMinDistance)
	{
		VERIFY				(m_tpSound);
		m_tpSound->set_range(fMinDistance,GetMaxDistance());
	}

			 void				SetMaxDistance	(const float fMaxDistance)
	{
		VERIFY				(m_tpSound);
		m_tpSound->set_range(GetMinDistance(),fMaxDistance);
	}

			 const Fvector		&GetPosition	() const
	{
		VERIFY				(m_tpSound);
		const CSound_params	*l_tpSoundParams = m_tpSound->get_params();
		if (l_tpSoundParams)
			return			(l_tpSoundParams->position);
		else {
			LuaOut			(Lua::eLuaMessageTypeError,"Sound was not launched, can't get position!");
			return			(Fvector().set(0,0,0));
		}
	}

			 const float		GetFrequency	() const
	{
		VERIFY				(m_tpSound);
		return				(m_tpSound->get_params()->freq);
	}

			 const float		GetMinDistance	() const
	{
		VERIFY				(m_tpSound);
		return				(m_tpSound->get_params()->min_distance);
	}

			 const float		GetMaxDistance	() const
	{
		VERIFY				(m_tpSound);
		return				(m_tpSound->get_params()->max_distance);
	}

			 const float		GetVolume		() const
	{
		VERIFY				(m_tpSound);
		return				(m_tpSound->get_params()->volume);
	}

			bool				IsPlaying		() const
	{
		VERIFY				(m_tpSound);
		return				(!!m_tpSound->feedback);
	}
};

class CLuaHit {
public:
	float				m_fPower; 
	Fvector				m_tDirection;
	ref_str				m_caBoneName;
	CLuaGameObject		*m_tpDraftsman;
	float				m_fImpulse;
	int					m_tHitType;

							CLuaHit				()
	{
		m_fPower		= 100;
		m_tDirection.set(1,0,0);
		m_caBoneName	= "";
		m_tpDraftsman	= 0;
		m_fImpulse		= 100;
		m_tHitType		= ALife::eHitTypeWound;
	}

							CLuaHit				(const CLuaHit *tpLuaHit)
	{
		*this			= *tpLuaHit;
	}

	virtual					~CLuaHit			()
	{
	}

			void			set_bone_name		(LPCSTR bone_name)
	{
		m_caBoneName	= bone_name;
	}
};

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
//
//							operator CWeapon*	() const
//	{
//		return			(dynamic_cast<CWeapon*>(m_tpGameObject));
//	}
//
	IC		CLuaGameObject	*Parent				() const
	{
		CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(m_tpGameObject->H_Parent());
		if (l_tpGameObject)
			return		(xr_new<CLuaGameObject>(l_tpGameObject));
		else
			return		(0);
	}
			
			void			Hit					(CLuaHit &tLuaHit);

	// CGameObject
	BIND_MEMBER		(m_tpGameObject,	ClassID,			CGameObject,	SUB_CLS_ID,			CLASS_ID,						CLASS_ID(-1));
	BIND_FUNCTION10	(m_tpGameObject,	Position,			CGameObject,	Position,			Fvector,						Fvector());
	BIND_FUNCTION10	(m_tpGameObject,	Name,				CGameObject,	cName,				LPCSTR,							"");
	BIND_FUNCTION10	(m_tpGameObject,	Section,			CGameObject,	cNameSect,			LPCSTR,							"");
	BIND_FUNCTION10	(m_tpGameObject,	Mass,				CGameObject,	GetMass,			float,							float(-1));
	BIND_FUNCTION10	(m_tpGameObject,	ID,					CGameObject,	ID,					u32,							u32(-1));
	BIND_FUNCTION10	(m_tpGameObject,	getVisible,			CGameObject,	getVisible,			BOOL,							FALSE);
	BIND_FUNCTION01	(m_tpGameObject,	setVisible,			CGameObject,	setVisible,			BOOL,							BOOL);
	BIND_FUNCTION10	(m_tpGameObject,	getEnabled,			CGameObject,	getEnabled,			BOOL,							FALSE);
	BIND_FUNCTION01	(m_tpGameObject,	setEnabled,			CGameObject,	setEnabled,			BOOL,							BOOL);

	// CInventoryItem
	BIND_FUNCTION10	(m_tpGameObject,	Cost,				CInventoryItem,	Cost,				u32,							u32(-1));
	BIND_FUNCTION10	(m_tpGameObject,	GetCondition,		CInventoryItem,	GetCondition,		float,							0);

	// CEntity
	BIND_MEMBER		(m_tpGameObject,	DeathTime,			CEntity,		m_dwDeathTime,		_TIME_ID,						_TIME_ID(-1));
	BIND_FUNCTION10	(m_tpGameObject,	Armor,				CEntity,		g_Armor,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	MaxHealth,			CEntity,		g_MaxHealth,		float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Accuracy,			CEntity,		g_Accuracy,			float,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Alive,				CEntity,		g_Alive,			BOOL,							FALSE);
	BIND_FUNCTION10	(m_tpGameObject,	Team,				CEntity,		g_Team,				int,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Squad,				CEntity,		g_Squad,			int,							-1);
	BIND_FUNCTION10	(m_tpGameObject,	Group,				CEntity,		g_Group,			int,							-1);
	BIND_FUNCTION01	(m_tpGameObject,	Kill,				CEntity,		KillEntity,			CLuaGameObject *,				CObject *);

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

	IC		void			GetRelationType		(CLuaGameObject* who)
	{
		CEntityAlive		*l_tpEntityAlive1 = dynamic_cast<CEntityAlive*>(m_tpGameObject);
		if (!l_tpEntityAlive1) {
			LuaOut			(Lua::eLuaMessageTypeError,"%s cannot access class member!",m_tpGameObject->cName());
			return;
		}
		
		CEntityAlive		*l_tpEntityAlive2 = dynamic_cast<CEntityAlive*>(who->m_tpGameObject);
		if (!l_tpEntityAlive2) {
			LuaOut			(Lua::eLuaMessageTypeError,"%s cannot access class member!",who->m_tpGameObject->cName());
			return;
		}
		
		l_tpEntityAlive1->tfGetRelationType(l_tpEntityAlive2);
	}

	// CScriptMonster
	
	BIND_FUNCTION02	(m_tpGameObject,	SetScriptControl,	CScriptMonster,	SetScriptControl,	bool,								LPCSTR,					bool,					ref_str);
	BIND_FUNCTION10	(m_tpGameObject,	GetScriptControl,	CScriptMonster,	GetScriptControl,	bool,								false);
	BIND_FUNCTION10	(m_tpGameObject,	GetScriptControlName,CScriptMonster,GetScriptControlName,LPCSTR,							"");
//	BIND_FUNCTION02	(m_tpGameObject,	AddAction,			CScriptMonster,	AddAction,			const CEntityAction *,				bool,					const CEntityAction *,	bool);
	BIND_FUNCTION10	(m_tpGameObject,	GetCurrentAction,	CScriptMonster,	GetCurrentAction,	const CEntityAction *,				0);

	IC		void			AddAction	(const CEntityAction *tpEntityAction, bool bHighPriority = false)
	{
		CScriptMonster		*l_tpScriptMonster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!l_tpScriptMonster)
			LuaOut		(Lua::eLuaMessageTypeError,"CSciptMonster : cannot access class member AddAction!");
		else
			l_tpScriptMonster->AddAction(tpEntityAction, bHighPriority);
	}

	// CCustomMonster
//	BIND_FUNCTION11	(m_tpGameObject,	CheckObjectVisibility,CCustomMonster,CheckObjectVisibility,	bool,							false,					const CLuaGameObject*,	const CObject*);

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
			return			(pEntityCondition->GetWhoHitLastTime()->cName());
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
			return			(pEntityCondition->GetWhoHitLastTime()->cNameSect());
		else 
		{
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member  WhoHitName()");
			return			NULL;
		}
	}

	// CAI_Stalker
//	BIND_FUNCTION01	(m_tpGameObject,	UseObject,			CAI_Stalker,	UseObject,			const CLuaGameObject*,				CObject*);
	IC		void			UseObject(const CLuaGameObject *tpLuaGameObject)
	{
		CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
		if (l_tpCustomMonster)
			l_tpCustomMonster->UseObject(tpLuaGameObject->m_tpGameObject);
		else
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member UseObject!");
	}

	BIND_FUNCTION10	(m_tpGameObject,	GetRank,			CAI_Stalker,	GetRank,			u32,								eStalkerRankDummy);
	BIND_FUNCTION10	(m_tpGameObject,	GetWeaponAmmo,		CAI_Stalker,	GetWeaponAmmo,		u32,								0);
	
			CLuaGameObject	*GetCurrentWeapon	() const;
			CLuaGameObject	*GetCurrentEquipment() const;
			CLuaGameObject	*GetFood			() const;
			CLuaGameObject	*GetMedikit			() const;

	// CInventoryOwner
	
	//�������� ������ ���������� InventoryOwner
	bool GiveInfoPortion(int info_index);
	bool GiveInfoPortionViaPda(int info_index, CLuaGameObject* pFromWho);
	
	IC		u32				GetInventoryObjectCount() const
	{
		CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
		if (l_tpInventoryOwner)
			return			(l_tpInventoryOwner->m_inventory.dwfGetObjectCount());
		else {
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member obj_count!");
			return			(0);
		}
	}

	IC		CLuaGameObject	*GetObjectByName	(LPCSTR caObjectName) const
	{
		CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
		if (l_tpInventoryOwner) {
			CInventoryItem	*l_tpInventoryItem = l_tpInventoryOwner->m_inventory.GetItemFromInventory(caObjectName);
			CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tpInventoryItem);
			if (!l_tpGameObject)
				return		(0);
			else
				return		(xr_new<CLuaGameObject>(l_tpGameObject));
		}
		else {
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member object!");
			return			(0);
		}
	}
	
	IC		CLuaGameObject	*GetObjectByIndex	(int iIndex) const
	{
		CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
		if (l_tpInventoryOwner) {
			CInventoryItem	*l_tpInventoryItem = l_tpInventoryOwner->m_inventory.tpfGetObjectByIndex(iIndex);
			CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tpInventoryItem);
			if (!l_tpGameObject)
				return		(0);
			else
				return		(xr_new<CLuaGameObject>(l_tpGameObject));
		}
		else {
			LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member object!");
			return			(0);	
		}
	}

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
			LuaOut			(Lua::eLuaMessageTypeError,"CScriptMonster : cannot access class member set_callback!");
		else
			l_tpScriptMonster->clear_callback(tActionType);
	}
};