////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects monsters for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_MonstersH
#define xrServer_Objects_ALife_MonstersH

#include "xrServer_Objects_ALife.h"
#include "xrServer_Objects_ALife_Items.h"
#include "character_info_defs.h"

#pragma warning(push)
#pragma warning(disable:4005)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_ALifeTraderAbstract)
	enum eTraderFlags {
		eTraderFlagInfiniteAmmo		= u32(1) << 0,
		eTraderFlagDummy			= u32(-1),
	};
	float							m_fCumulativeItemMass;
	int								m_iCumulativeItemVolume;
	u32								m_dwMoney;
	ALife::EStalkerRank				m_tRank;
	float							m_fMaxItemMass;
	ALife::PERSONAL_EVENT_P_VECTOR	m_tpEvents;
	Flags32							m_trader_flags;

	////////////////////////////////////////////////////
	//character profile info
	PROFILE_INDEX					character_profile		();
	void							set_character_profile	(PROFILE_INDEX);
	SPECIFIC_CHARACTER_INDEX		specific_character		();
	void							set_specific_character	(SPECIFIC_CHARACTER_INDEX);
private:
	bool							m_character_profile_init;
	PROFILE_INDEX					m_iCharacterProfile;
	SPECIFIC_CHARACTER_INDEX		m_iSpecificCharacter;
	//буферный вектор проверенных персонажей
	xr_vector<SPECIFIC_CHARACTER_INDEX> m_CheckedCharacters;


public:	
									CSE_ALifeTraderAbstract(LPCSTR caSection);
	virtual							~CSE_ALifeTraderAbstract();
	// we need this to prevent virtual inheritance :-(
	virtual CSE_Abstract			*base					() = 0;
	virtual const CSE_Abstract		*base					() const = 0;
	virtual CSE_Abstract			*init					();
	// end of the virtual inheritance dependant code
#ifdef XRGAME_EXPORTS
			void					attach					(CSE_ALifeInventoryItem *tpALifeInventoryItem,	bool		bALifeRequest,	bool bAddChildren = true);
			void					detach					(CSE_ALifeInventoryItem *tpALifeInventoryItem,	ALife::OBJECT_IT	*I = 0,	bool bALifeRequest = true,	bool bRemoveChildren = true);
			void					vfInitInventory			();
	virtual void					spawn_supplies			();
#endif
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeTraderAbstract)
#define script_type_list save_type_list(CSE_ALifeTraderAbstract)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeTrader,CSE_ALifeDynamicObjectVisual,CSE_ALifeTraderAbstract)
	ALife::_ORGANIZATION_ID				m_tOrgID;
	ALife::ARTEFACT_TRADER_ORDER_MAP	m_tpOrderedArtefacts;
	ALife::TRADER_SUPPLY_VECTOR			m_tpSupplies;


									CSE_ALifeTrader			(LPCSTR caSection);
	virtual							~CSE_ALifeTrader		();
	virtual	bool					interactive				() const;
	virtual CSE_Abstract			*init					();
	virtual CSE_Abstract			*base					();
	virtual const CSE_Abstract		*base					() const;
	virtual void					on_surge				();
	int 							supplies_count;
			void 	__stdcall		OnSuppliesCountChange	(PropValue* sender);

#ifdef XRGAME_EXPORTS
			u32						dwfGetItemCost			(CSE_ALifeInventoryItem *tpALifeInventoryItem);
	virtual void					spawn_supplies			();
#endif
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeTrader)
#define script_type_list save_type_list(CSE_ALifeTrader)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeCustomZone,CSE_ALifeSpaceRestrictor)
	f32								m_maxPower;
	f32								m_attn;
	u32								m_period;
	ALife::EAnomalousZoneType		m_tAnomalyType;
	ALife::EHitType					m_tHitType;
	
									CSE_ALifeCustomZone		(LPCSTR caSection);
	virtual							~CSE_ALifeCustomZone	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeCustomZone)
#define script_type_list save_type_list(CSE_ALifeCustomZone)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeAnomalousZone,CSE_ALifeCustomZone,CSE_ALifeSchedulable)
	float							m_fRadius;
	float							m_fBirthProbability;
	u16								m_wItemCount;
	float							*m_faWeights;
	string64						*m_cppArtefactSections;
	u16								m_wArtefactSpawnCount;
	u32								m_dwStartIndex;
	float							m_fStartPower;
	float							m_min_start_power;
	float							m_max_start_power;
	float							m_power_artefact_factor;

									CSE_ALifeAnomalousZone	(LPCSTR caSection);
	virtual							~CSE_ALifeAnomalousZone	();
	virtual CSE_Abstract			*init					();
	virtual CSE_Abstract			*base					();
	virtual const CSE_Abstract		*base					() const;
#ifndef XRGAME_EXPORTS
	virtual	void					update					()	{};
#else
	virtual	void					update					();
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(ALife::EHitType		&tHitType,				float &fHitPower);
	virtual	ALife::EMeetActionType	tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable,	int iGroupIndex, bool bMutualDetection);
	virtual bool					bfActive				();
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		();
#endif
	virtual bool					need_update				(CSE_ALifeDynamicObject *object);
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeAnomalousZone)
#define script_type_list save_type_list(CSE_ALifeAnomalousZone)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeTorridZone,CSE_ALifeCustomZone,CSE_Motion)
									CSE_ALifeTorridZone		(LPCSTR caSection);
	virtual							~CSE_ALifeTorridZone	();
	virtual CSE_Motion*	__stdcall	motion					();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeTorridZone)
#define script_type_list save_type_list(CSE_ALifeTorridZone)

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeAnomalousZoneMoving, CSE_ALifeAnomalousZone)
//	
//	u32								timestamp;				
//
//									CSE_ALifeAnomalousZoneMoving	(LPCSTR caSection);
//	virtual							~CSE_ALifeAnomalousZoneMoving	();
//
//SERVER_ENTITY_DECLARE_END
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeCreatureAbstract,CSE_ALifeDynamicObjectVisual)
	u8								s_team;
	u8								s_squad;
	u8								s_group;
	float							fHealth;
	float							m_fMorale;
	float							m_fAccuracy;
	float							m_fIntelligence;

	u32								timestamp;				// server(game) timestamp
	u8								flags;
	float							o_model;				// model yaw
	SRotation						o_torso;				// torso in world coords
	bool							m_bDeathIsProcessed;
									
									CSE_ALifeCreatureAbstract(LPCSTR caSection);
	virtual							~CSE_ALifeCreatureAbstract();
	virtual u8						g_team					();
	virtual u8						g_squad					();
	virtual u8						g_group					();
	IC		float					g_Health				() const								{ return fHealth;}
	virtual bool					used_ai_locations		() const;
	virtual bool					can_switch_online		() const;
	virtual bool					can_switch_offline		() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeCreatureAbstract)
#define script_type_list save_type_list(CSE_ALifeCreatureAbstract)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeMonsterAbstract,CSE_ALifeCreatureAbstract,CSE_ALifeSchedulable)
	ALife::_GRAPH_ID				m_tNextGraphID;
	ALife::_GRAPH_ID				m_tPrevGraphID;
	float							m_fGoingSpeed;
	float							m_fCurSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	ALife::TERRAIN_VECTOR			m_tpaTerrain;
	float							m_fMaxHealthValue;
	float							m_fRetreatThreshold;
	float							m_fEyeRange;
	float							m_fHitPower;
	ALife::EHitType					m_tHitType;
	shared_str							m_out_space_restrictors;
	shared_str							m_in_space_restrictors;
	svector<float,ALife::eHitTypeMax>	m_fpImmunityFactors;
	
									CSE_ALifeMonsterAbstract(LPCSTR					caSection);
	virtual							~CSE_ALifeMonsterAbstract();
	IC		float					g_MaxHealth				()	const									{ return m_fMaxHealthValue;	}
	virtual CSE_Abstract			*init					();
	virtual CSE_Abstract			*base					();
	virtual const CSE_Abstract		*base					() const;
#ifndef XRGAME_EXPORTS
	virtual	void					update					()	{};
#else
	virtual	void					update					();
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(ALife::EHitType		&tHitType,				float	&fHitPower);
	virtual	ALife::EMeetActionType	tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable,	int		iGroupIndex,	bool bMutualDetection);
	virtual bool					bfActive				();
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		();
	virtual	void					vfDetachAll				(bool					bFictitious = false) {};
			void					vfCheckForPopulationChanges();
#endif
	virtual bool					need_update				(CSE_ALifeDynamicObject *object);
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeMonsterAbstract)
#define script_type_list save_type_list(CSE_ALifeMonsterAbstract)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeCreatureActor,CSE_ALifeCreatureAbstract,CSE_ALifeTraderAbstract)
	u16								mstate;
	Fvector							accel;
	Fvector							velocity;
	float							fArmor;
	u8								weapon;
	///////////////////////////////////////////
	u16								m_u16NumItems;

//	DEF_DEQUE		(PH_STATES, SPHNetState); 
	SPHNetState						m_AliveState;
//	PH_STATES						m_DeadStates;

	// статический массив - 6 float(вектора пределов квантизации) + m_u16NumItems*(7 u8) (позиция и поворот кости)
	u8								m_BoneDataSize;
	char							m_DeadBodyData[1024];
	///////////////////////////////////////////
									CSE_ALifeCreatureActor	(LPCSTR caSection);
	virtual							~CSE_ALifeCreatureActor	();
	virtual CSE_Abstract			*base					();
	virtual const CSE_Abstract		*base					() const;
	virtual CSE_Abstract			*init					();

#ifdef XRGAME_EXPORTS
	virtual void					spawn_supplies			();
#endif
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeCreatureActor)
#define script_type_list save_type_list(CSE_ALifeCreatureActor)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeCreatureCrow,CSE_ALifeCreatureAbstract)
									CSE_ALifeCreatureCrow	(LPCSTR caSection);
	virtual							~CSE_ALifeCreatureCrow	();
	virtual bool					used_ai_locations		() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeCreatureCrow)
#define script_type_list save_type_list(CSE_ALifeCreatureCrow)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeMonsterRat,CSE_ALifeMonsterAbstract,CSE_ALifeInventoryItem)
	// Personal characteristics:
	float							fEyeFov;
	float							fEyeRange;
	float							fMinSpeed;
	float							fMaxSpeed;
	float							fAttackSpeed;
	float							fMaxPursuitRadius;
	float							fMaxHomeRadius;
	// morale
	float							fMoraleSuccessAttackQuant;
	float							fMoraleDeathQuant;
	float							fMoraleFearQuant;
	float							fMoraleRestoreQuant;
	u16								u16MoraleRestoreTimeInterval;
	float							fMoraleMinValue;
	float							fMoraleMaxValue;
	float							fMoraleNormalValue;
	// attack
	float							fHitPower;
	u16								u16HitInterval;
	float							fAttackDistance;
	float							fAttackAngle;
	float							fAttackSuccessProbability;

									CSE_ALifeMonsterRat	(LPCSTR caSection);				// constructor for variable initialization
	virtual							~CSE_ALifeMonsterRat();
	virtual bool					bfUseful();
	virtual CSE_Abstract			*init				();
	virtual CSE_Abstract			*base				();
	virtual const CSE_Abstract		*base				() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeMonsterRat)
#define script_type_list save_type_list(CSE_ALifeMonsterRat)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterZombie,CSE_ALifeMonsterAbstract)
	// Personal characteristics:
	float							fEyeFov;
	float							fEyeRange;
	float							fMinSpeed;
	float							fMaxSpeed;
	float							fAttackSpeed;
	float							fMaxPursuitRadius;
	float							fMaxHomeRadius;
	// attack
	float							fHitPower;
	u16								u16HitInterval;
	float							fAttackDistance;
	float							fAttackAngle;

									CSE_ALifeMonsterZombie	(LPCSTR caSection);				// constructor for variable initialization
	virtual							~CSE_ALifeMonsterZombie	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeMonsterZombie)
#define script_type_list save_type_list(CSE_ALifeMonsterZombie)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeMonsterBase,CSE_ALifeMonsterAbstract,CSE_PHSkeleton)
									CSE_ALifeMonsterBase	(LPCSTR caSection);				// constructor for variable initialization
	virtual							~CSE_ALifeMonsterBase	();
	virtual	void					load					(NET_Packet &tNetPacket);
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeMonsterBase)
#define script_type_list save_type_list(CSE_ALifeMonsterBase)

//-------------------------------
SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeHumanAbstract,CSE_ALifeTraderAbstract,CSE_ALifeMonsterAbstract)
	DWORD_VECTOR					m_tpPath;
	u32								m_dwCurNode;
	ALife::_GRAPH_ID				m_tDestGraphPointIndex;								
	xr_vector<bool>					m_baVisitedVertices;
	ALife::ETaskState				m_tTaskState;
	u32								m_dwCurTaskLocation;
	u32								m_dwCurTaskID;
	float							m_fSearchSpeed;
	shared_str						m_caKnownCustomers;
	ALife::OBJECT_VECTOR			m_tpKnownCustomers;
	svector<char,5>					m_cpEquipmentPreferences;
	svector<char,4>					m_cpMainWeaponPreferences;
	u32								m_dwTotalMoney;
	float							m_fGoingSuccessProbability;
	float							m_fSearchSuccessProbability;

									CSE_ALifeHumanAbstract	(LPCSTR					caSection);
	virtual							~CSE_ALifeHumanAbstract	();
	virtual CSE_Abstract			*init					();
	virtual CSE_Abstract			*base					();
	virtual const CSE_Abstract		*base					() const;
	virtual void					on_surge				();
#ifdef XRGAME_EXPORTS
	virtual	void					update					();
			// FSM
			void					vfChooseTask			();
			void					vfHealthCare			();
			void					vfBuySupplies			();
			void					vfGoToCustomer			();
			void					vfBringToCustomer		();
			void					vfGoToSOS				();
			void					vfSendSOS				();
			void					vfAccomplishTask		();
			void					vfSearchObject			();
			// FSM miscellanious
			void					vfChooseHumanTask		();
			bool					bfHealthIsGood			();
			bool					bfItemCanTreat			(CSE_ALifeInventoryItem	*tpALifeInventoryItem);
			void					vfUseItem				(CSE_ALifeInventoryItem	*tpALifeInventoryItem);
			bool					bfCanTreat				();
			bool					bfEnoughMoneyToTreat	();
			bool					bfEnoughTimeToTreat		();
			bool					bfEnoughEquipmentToGo	();
			bool					bfDistanceToTraderIsDanger();
			bool					bfEnoughMoneyToEquip	();
			// miscellanious
			bool					bfCheckIfTaskCompleted	(ALife::OBJECT_IT		&I);
			bool					bfCheckIfTaskCompleted	();
			void					vfCheckForDeletedEvents	();
			bool					bfChooseNextRoutePoint	();
			void					vfSetCurrentTask		(ALife::_TASK_ID		&tTaskID);
			u16						get_available_ammo_count(const CSE_ALifeItemWeapon	*tpALifeItemWeapon,		ALife::OBJECT_VECTOR	&tpObjectVector);
			u16						get_available_ammo_count(const CSE_ALifeItemWeapon	*tpALifeItemWeapon,		ALife::ITEM_P_VECTOR	&tpItemVector,		ALife::OBJECT_VECTOR	*tpObjectVector = 0);
			void					attach_available_ammo	(CSE_ALifeItemWeapon	*tpALifeItemWeapon,			ALife::ITEM_P_VECTOR	&tpItemVector,		ALife::OBJECT_VECTOR	*tpObjectVector = 0);
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(ALife::EHitType		&tHitType,					float					&fHitPower);
	virtual bool					bfPerformAttack			();
	virtual	void					vfUpdateWeaponAmmo		();
	virtual	void					vfProcessItems			();
	virtual	void					vfAttachItems			(ALife::ETakeType		tTakeType = ALife::eTakeTypeAll);
			bool					bfCanGetItem			(CSE_ALifeInventoryItem	*tpALifeInventoryItem);
	virtual	ALife::EMeetActionType	tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable,		int						iGroupIndex,		bool			bMutualDetection);
			void					vfCollectAmmoBoxes		();
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		();
	virtual	void					vfDetachAll				(bool					bFictitious = false);
			int						ifChooseEquipment		(ALife::OBJECT_VECTOR	*tpObjectVector = 0);
			int						ifChooseWeapon			(ALife::EWeaponPriorityType	tWeaponPriorityType,	ALife::OBJECT_VECTOR	*tpObjectVector = 0);
			int						ifChooseFood			(ALife::OBJECT_VECTOR	*tpObjectVector = 0);
			int						ifChooseMedikit			(ALife::OBJECT_VECTOR	*tpObjectVector = 0);
			int						ifChooseDetector		(ALife::OBJECT_VECTOR	*tpObjectVector = 0);
			int						ifChooseValuables		();
			bool					bfChooseFast			();
			void					vfChooseGroup			(CSE_ALifeGroupAbstract *tpALifeGroupAbstract);
	virtual void					spawn_supplies			();
#endif
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeHumanAbstract)
#define script_type_list save_type_list(CSE_ALifeHumanAbstract)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeHumanStalker,CSE_ALifeHumanAbstract,CSE_PHSkeleton)
									CSE_ALifeHumanStalker	(LPCSTR caSection);
	virtual							~CSE_ALifeHumanStalker	();
	virtual	void					load					(NET_Packet &tNetPacket);
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeHumanStalker)
#define script_type_list save_type_list(CSE_ALifeHumanStalker)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectIdol,CSE_ALifeHumanAbstract)
	shared_str							m_caAnimations;
	u32								m_dwAniPlayType;
									CSE_ALifeObjectIdol		(LPCSTR caSection);
	virtual							~CSE_ALifeObjectIdol	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectIdol)
#define script_type_list save_type_list(CSE_ALifeObjectIdol)

#pragma warning(pop)

#endif