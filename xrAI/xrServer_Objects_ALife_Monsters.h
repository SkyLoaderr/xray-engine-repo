////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects monsters for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_MonstersH
#define xrServer_Objects_ALife_MonstersH

#include "xrServer_Objects_ALife.h"

class CSE_ALifeTraderAbstract : virtual public CSE_Abstract {
public:
	float							m_fCumulativeItemMass;
	int								m_iCumulativeItemVolume;
	u32								m_dwMoney;
	EStalkerRank					m_tRank;
	float							m_fMaxItemMass;
	PERSONAL_EVENT_P_VECTOR			m_tpEvents;
	
									CSE_ALifeTraderAbstract(LPCSTR caSection);
	virtual							~CSE_ALifeTraderAbstract();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeTrader,CSE_ALifeDynamicObjectVisual,CSE_ALifeTraderAbstract)
	_ORGANIZATION_ID				m_tOrgID;

									CSE_ALifeTrader	(LPCSTR caSection);
	virtual							~CSE_ALifeTrader();

#ifdef _EDITOR
	int 							supplies_count;
    void __fastcall   				OnSuppliesCountChange	(PropValue* sender);
#endif    
    
	ARTEFACT_TRADER_ORDER_VECTOR	m_tpOrderedArtefacts;
	TRADER_SUPPLY_VECTOR			m_tpSupplies;
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN3(CSE_ALifeAnomalousZone,CSE_ALifeDynamicObject,CSE_ALifeSchedulable,CSE_Shape)
	f32								m_maxPower;
	f32								m_attn;
	u32								m_period;
	float							m_fRadius;
	float							m_fBirthProbability;
	u16								m_wItemCount;
	float							*m_faWeights;
	string64						*m_cppArtefactSections;
	u16								m_wArtefactSpawnCount;
	u32								m_dwStartIndex;
	EAnomalousZoneType				m_tAnomalyType;
	EHitType						m_tHitType;
	float							m_fStartPower;

									CSE_ALifeAnomalousZone	(LPCSTR caSection);
	virtual							~CSE_ALifeAnomalousZone	();
#ifdef _EDITOR
	virtual	void					Update					()	{};
#else
#ifdef AI_COMPILER
	virtual	void					Update					()	{};
#else
	virtual	void					Update					();
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(EHitType				&tHitType,				float &fHitPower);
	virtual	EMeetActionType			tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable,	int iGroupIndex, bool bMutualDetection);
	virtual bool					bfActive				();
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		();
#endif
#endif
SERVER_ENTITY_DECLARE_END

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
	virtual u8						g_team					();
	virtual u8						g_squad					();
	virtual u8						g_group					();
	IC		float					g_Health				()										{ return fHealth;}
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeMonsterAbstract,CSE_ALifeCreatureAbstract,CSE_ALifeSchedulable)
	_GRAPH_ID						m_tNextGraphID;
	_GRAPH_ID						m_tPrevGraphID;
	float							m_fGoingSpeed;
	float							m_fCurSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	TERRAIN_VECTOR					m_tpaTerrain;
	float							m_fMaxHealthValue;
	float							m_fRetreatThreshold;
	float							m_fEyeRange;
	float							m_fHitPower;
	EHitType						m_tHitType;
	svector<float,eHitTypeMax>		m_fpImmunityFactors;
	
									CSE_ALifeMonsterAbstract(LPCSTR					caSection);
	IC		float					g_MaxHealth				()											{ return m_fMaxHealthValue;	}
#ifdef _EDITOR
	virtual	void					Update					()	{};
#else
#ifdef AI_COMPILER
	virtual	void					Update					()	{};
#else
	virtual	void					Update					();
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(EHitType				&tHitType,			 float &fHitPower);
	virtual	EMeetActionType			tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable, int iGroupIndex, bool bMutualDetection);
	virtual bool					bfActive				();
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		();
	virtual	void					vfDetachAll				()	{};
			void					vfCheckForPopulationChanges();
#endif
#endif
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeCreatureActor,CSE_ALifeCreatureAbstract,CSE_ALifeTraderAbstract)
	u16								mstate;
	Fvector							accel;
	Fvector							velocity;
	float							fArmor;
	u8								weapon;
									CSE_ALifeCreatureActor		(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeCreatureCrow,CSE_ALifeCreatureAbstract)
									CSE_ALifeCreatureCrow		(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

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
	virtual bool					bfUseful();
SERVER_ENTITY_DECLARE_END

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
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterBiting,CSE_ALifeMonsterAbstract)
									CSE_ALifeMonsterBiting	(LPCSTR caSection);				// constructor for variable initialization
SERVER_ENTITY_DECLARE_END

//-------------------------------

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeHumanAbstract,CSE_ALifeTraderAbstract,CSE_ALifeMonsterAbstract)
	DWORD_VECTOR					m_tpPath;
	u32								m_dwCurNode;
	_GRAPH_ID						m_tDestGraphPointIndex;								
	xr_vector<bool>					m_baVisitedVertices;
	ETaskState						m_tTaskState;
	u32								m_dwCurTaskLocation;
	u32								m_dwCurTaskID;
	float							m_fSearchSpeed;
	string128						m_caKnownCustomers;
	OBJECT_VECTOR					m_tpKnownCustomers;
	svector<char,5>					m_cpEquipmentPreferences;
	svector<char,4>					m_cpMainWeaponPreferences;
	ITEM_P_VECTOR					m_tpTempItemBuffer;


									CSE_ALifeHumanAbstract	(LPCSTR					caSection);
	virtual							~CSE_ALifeHumanAbstract	();
#ifndef _EDITOR
#ifndef AI_COMPILER
	virtual	void					Update					();
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
			bool					bfCheckIfTaskCompleted	(OBJECT_IT				&I);
			bool					bfCheckIfTaskCompleted	();
			void					vfCheckForDeletedEvents	();
			bool					bfChooseNextRoutePoint	();
			void					vfSetCurrentTask		(_TASK_ID				&tTaskID);
			u16						get_available_ammo_count(CSE_ALifeItemWeapon	*tpALifeItemWeapon, OBJECT_VECTOR &tpObjectVector);
			u16						get_available_ammo_count(CSE_ALifeItemWeapon	*tpALifeItemWeapon,	ITEM_P_VECTOR &tpItemVector);
			void					attach_available_ammo	(CSE_ALifeItemWeapon	*tpALifeItemWeapon, ITEM_P_VECTOR &tpItemVector);
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(EHitType				&tHitType,			float &fHitPower);
	virtual bool					bfPerformAttack			();
	virtual	void					vfUpdateWeaponAmmo		();
	virtual	void					vfProcessItems			();
	virtual	void					vfAttachItems			(ETakeType tTakeType = eTakeTypeAll);
			bool					bfCanGetItem			(CSE_ALifeInventoryItem	*tpALifeInventoryItem);
	virtual	EMeetActionType			tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable,int iGroupIndex, bool bMutualDetection);
			void					vfCollectAmmoBoxes		();
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		();
	virtual	void					vfDetachAll				();
#endif
#endif
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeHumanStalker,CSE_ALifeHumanAbstract)
									CSE_ALifeHumanStalker	(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectIdol,CSE_ALifeHumanAbstract)
	string256						m_caAnimations;
	u32								m_dwAniPlayType;
									CSE_ALifeObjectIdol		(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

#endif