////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_space.h
//	Created 	: 08.01.2002
//  Modified 	: 08.01.2003
//	Author		: Dmitriy Iassenev
//	Description : ALife space
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ALIFE_SPACE
#define XRAY_ALIFE_SPACE

// spawn points
#define SPAWN_POINT_VERSION			0x0001
#define SPAWN_POINT_CHUNK_VERSION	0xffff
// ALife objects, events and tasks
#define ALIFE_VERSION				0x0001
#define ALIFE_CHUNK_DATA			0x0000
#define SPAWN_CHUNK_DATA			0x0001
#define OBJECT_CHUNK_DATA			0x0002
#define EVENT_CHUNK_DATA			0x0003
#define TASK_CHUNK_DATA				0x0004
#define GAME_TIME_CHUNK_DATA		0x0005
#define ANOMALY_CHUNK_DATA			0x0006
#define DISCOVERY_CHUNK_DATA		0x0007
#define NEWS_CHUNK_DATA				0x0008
#define REGISTRY_CHUNK_DATA			0x0009
#define LOCATION_TYPE_COUNT			4
#define SECTION_HEADER				"location_"
#define SAVE_EXTENSION				".sav"
#define SPAWN_NAME					"game.spawn"
// inventory rukzak size
#define MAX_ITEM_VOLUME				100
#define INVALID_STORY_ID			ALife::_STORY_ID(-1)

class CALifeEventGroup;
class CSE_ALifeDynamicObject;
class CSE_ALifeMonsterAbstract;
class CSE_ALifeTrader;
class CALifeEvent;
class CALifeTask;
class CALifeEventPersonal;
class CALifeKnownAnomaly;
class CALifeDiscovery;
class CALifeOrganization;
class CALifeArtefactDemand;
class CALifeNews;
class CSE_ALifeArtefactNeed;
class CSE_ALifeInventoryItem;
class CSE_ALifeItemWeapon;
class CSE_ALifeSchedulable;
class CGameGraph;

namespace ALife {
	typedef u64	_CLASS_ID;									// Class ID
	typedef u16	_OBJECT_ID;									// Object ID
	typedef u64	_TIME_ID;									// Time  ID
	typedef u32	_EVENT_ID;									// Event ID
	typedef u32	_TASK_ID;									// Event ID
	typedef u16	_GRAPH_ID;									// Graph ID
	typedef u16	_SPAWN_ID;									// Spawn ID
	typedef u16	_TERRAIN_ID;								// Terrain ID
	typedef u8	_LOCATION_ID;								// Location ID
	typedef u32 _LEVEL_ID;									// Level ID
	typedef u32	_ORGANIZATION_ID;							// Organization ID
	typedef u32	_NEWS_ID;									// News ID
	typedef u32 _STORY_ID;								// Subject ID

	const	u32	LOCATION_COUNT	= (u32(1) << (8*sizeof(_LOCATION_ID)));

	typedef struct tagSTerrainPlace{
		svector<_LOCATION_ID,LOCATION_TYPE_COUNT>	tMask;
		u32											dwMinTime;
		u32											dwMaxTime;
	} STerrainPlace;

	struct SArtefactOrder {
		string64				m_section;
		u32						m_count;
		u32						m_price;
	};

	struct SOrganizationOrder {
		CALifeOrganization		*m_tpALifeOrganization;
		u32						m_dwCount;
	};

	struct STraderSupply {
		shared_str					m_caSections;
		u32						m_dwCount;
		float					m_fMinFactor;
		float					m_fMaxFactor;
		STraderSupply()
		{
			m_dwCount			= 1;
			m_fMinFactor		= 0.f;
			m_fMaxFactor		= 1.f;
		}
	};

	struct SSumStackCell {
		int						i1;
		int						i2;
		int						iCurrentSum;
	};

	enum EInjureType {
		eInjureTypeNone = u32(0),
		eInjureTypeEat,
		eInjureTypeShoot,
		eInjureTypeDestroyed,
		eInjureTypeDummy = u32(-1),
	};

	enum EEventRelationType {
		eEventRelationTypeNone = u32(0),
		eEventRelationTypeMeet,
		eEventRelationTypeWatch,
		eEventRelationTypeHear,
		eEventRelationTypeSeeResult,
		eEventRelationTypeMeetTold,
		eEventRelationTypeWatchTold,
		eEventRelationTypeHearTold,
		eEventRelationTypeSeeResultTold,
		eEventRelationTypeDummy = u32(-1),
	};

	enum ECombatResult {
		eCombatResultRetreat1 = u32(0),
		eCombatResultRetreat2,
		eCombatResultRetreat12,
		eCombatResult1Kill2,
		eCombatResult2Kill1,
		eCombatResultBothKilled,
		eCombatDummy = u32(-1),
	};

	enum ETaskType {
		eTaskTypeNone = u32(0),
		eTaskTypeSearchForItemCG,
		eTaskTypeSearchForItemCL,
		eTaskTypeSearchForItemOG,
		eTaskTypeSearchForItemOL,
		eTaskTypeDummy = u32(-1),
	};

	enum EStalkerRank {
		eStalkerRankNone = u32(0),
		eStalkerRankNovice,
		eStalkerRankExperienced,
		eStalkerRankVeteran,
		eStalkerRankMaster,
		eStalkerRankDummy = u32(-1),
	};

	enum EZoneState {
		eZoneStateNone = u32(0),
		eZoneStateSurge,
		eZoneStateBetweenSurges,
		eZoneStateDummy = u32(-1),
	};
	
	enum EArtefactType {
		eArtefactTypeNone = u32(0),
		eArtefactTypeGravi,
		eArtefactTypeRadio,
		eArtefactTypeMagnet,
		eArtefactTypeMercuryBall,
		eArtefactTypeBlackDroplets,
		eArtefactTypeDummy = u32(-1),
	};
		
	enum EResearchState {
		eResearchStateLeft = u32(0),
		eResearchStateJoin,
		eResearchStateResearch,
		eResearchStateFreeze,
		eResearchStateDummy = u32(-1),
	};
	
	enum ETaskState {
		eTaskStateChooseTask = u32(0),
		eTaskStateGoToCustomer,
		eTaskStateAccomplishTask,
		eTaskStateGoToSOS,
		eTaskStateSendSOS,
		eTaskStateSearchItem,
		eTaskStateGoingToSearchItem,
		eTaskStateGoingToCustomer,
		eTaskStateTradeWithCustomer,
		eTaskStateDummy = u32(-1),
	};
	
	enum EAnomalousZoneType {
		eAnomalousZoneTypeGravi = u32(0),
		eAnomalousZoneTypeFog,
		eAnomalousZoneTypeRadio,
		eAnomalousZoneTypePlant,
		eAnomalousZoneTypeGelatine,
		eAnomalousZoneTypeFluff,
		eAnomalousZoneTypeRustyHair,
		eAnomalousZoneTypeRustyWhistlers,
		eAnomalousZoneTypeDummy,
	};

	enum ECombatAction {
		eCombatActionAttack = u32(0),
		eCombatActionRetreat,
		eCombatActionDummy = u32(-1),
	};

	enum EMeetActionType {
		eMeetActionTypeAttack = u32(0),
		eMeetActionTypeInteract,
		eMeetActionTypeIgnore,
		eMeetActionTypeDummy = u32(-1),
	};

	enum ERelationType {
		eRelationTypeFriend = u32(0),
		eRelationTypeNeutral,
		eRelationTypeEnemy,
		eRelationTypeDummy = u32(-1),
	};

	enum EHitType {
		eHitTypeBurn = u32(0),
		eHitTypeShock,
		eHitTypeStrike,
		eHitTypeWound,
		eHitTypeRadiation,
		eHitTypeTelepatic,
		eHitTypeChemicalBurn,
		eHitTypeExplosion,
		eHitTypeFireWound,
		eHitTypeMax,
	};

	enum ETakeType {
		eTakeTypeAll,
		eTakeTypeMin,
		eTakeTypeRest,
	};

	enum EWeaponPriorityType {
		eWeaponPriorityTypeKnife = u32(0),
		eWeaponPriorityTypeSecondary,
		eWeaponPriorityTypePrimary,
		eWeaponPriorityTypeGrenade,
		eWeaponPriorityTypeDummy = u32(-1),
	};

	enum ECombatType {
		eCombatTypeMonsterMonster = u32(0),
		eCombatTypeMonsterAnomaly,
		eCombatTypeAnomalyMonster,
		eCombatTypeDummy = u32(-1),
	};

	//возможность подключения аддонов
	enum EWeaponAddonStatus{
		eAddonDisabled				= 0,	//нельзя присоеденить
		eAddonPermanent				= 1,	//постоянно подключено по умолчанию
		eAddonAttachable			= 2		//можно присоединять
	};

	enum ENewsType {
		eNewsTypeRetreat = u32(0),
		eNewsTypeRetreatBoth,
		eNewsTypeKill,
		eNewsTypeKillBoth,
		eNewsTypeDummy = u32(-1),
	};

	IC EHitType	g_tfString2HitType(LPCSTR caHitType)
	{
		if (!stricmp(caHitType,"burn"))
			return(eHitTypeBurn);
		else if (!stricmp(caHitType,"shock"))
				return(eHitTypeShock);
		else if (!stricmp(caHitType,"strike"))
            	return(eHitTypeStrike);
		else if (!stricmp(caHitType,"wound"))
				return(eHitTypeWound);
		else if (!stricmp(caHitType,"radiation"))
				return(eHitTypeRadiation);
		else if (!stricmp(caHitType,"telepatic"))
				return(eHitTypeTelepatic);
		else if (!stricmp(caHitType,"fire_wound"))
				return(eHitTypeFireWound);
		else if (!stricmp(caHitType,"chemical_burn"))
				return(eHitTypeChemicalBurn);
		else if (!stricmp(caHitType,"explosion"))
				return(eHitTypeExplosion);
		else
				Debug.fatal("Unsupported hit type!");
		NODEFAULT;
#ifdef DEBUG
		return(eHitTypeMax);
#endif
	}

	IC LPCSTR g_cafHitType2String(EHitType tHitType)
	{
		switch (tHitType) {
			case eHitTypeBurn			: return("burn");
			case eHitTypeShock			: return("shock");
			case eHitTypeStrike			: return("strike");
			case eHitTypeWound			: return("wound");
			case eHitTypeRadiation		: return("radiation");
			case eHitTypeTelepatic		: return("telepatic");
			case eHitTypeFireWound		: return("fire_wound");
			case eHitTypeChemicalBurn	: return("chemical_burn");
			case eHitTypeExplosion		: return("explosion");
			default						: NODEFAULT;
		}
#ifdef DEBUG
		return(0);
#endif
	}

	DEFINE_MAP		(_NEWS_ID,					CALifeNews*,					NEWS_REGISTRY,				NEWS_REGISTRY_IT);

	DEFINE_MAP		(_OBJECT_ID,				CSE_ALifeDynamicObject*,		D_OBJECT_MAP,				D_OBJECT_PAIR_IT);
	DEFINE_MAP		(_EVENT_ID,					CALifeEvent*,					EVENT_MAP,					EVENT_PAIR_IT);

	typedef struct tagSGraphPoint {
		D_OBJECT_MAP			tpObjects;
		EVENT_MAP				tpEvents;
	} SGraphPoint;

	DEFINE_VECTOR	(SArtefactOrder,			ARTEFACT_ORDER_VECTOR,			ARTEFACT_ORDER_IT);

	struct SArtefactTraderOrder {
		string64				m_caSection;
		u32						m_dwTotalCount;
		ARTEFACT_ORDER_VECTOR	m_tpOrders;
	};

	DEFINE_VECTOR	(int,						INT_VECTOR,						INT_IT);
	DEFINE_VECTOR	(CSE_ALifeDynamicObject*,	D_OBJECT_P_VECTOR,				D_OBJECT_P_IT);
	DEFINE_VECTOR	(CALifeEvent*,				EVENT_P_VECTOR,					EVENT_P_IT);
	DEFINE_VECTOR	(_OBJECT_ID,				OBJECT_VECTOR,					OBJECT_IT);
	DEFINE_VECTOR	(_OBJECT_ID,				OBJECT_VECTOR,					OBJECT_IT);
	DEFINE_VECTOR	(_EVENT_ID,					EVENT_VECTOR,					EVENT_IT);
	DEFINE_VECTOR	(_TASK_ID,					TASK_VECTOR,					TASK_IT);
	DEFINE_VECTOR	(_GRAPH_ID,					GRAPH_VECTOR,					GRAPH_IT);
	DEFINE_VECTOR	(OBJECT_VECTOR,				OBJECT_VECTOR_VECTOR,			OBJECT_VECTOR_IT);
	DEFINE_VECTOR	(CSE_ALifeMonsterAbstract*,	MONSTER_P_VECTOR,				MONSTER_P_IT);
	DEFINE_VECTOR	(CSE_ALifeTrader*,			TRADER_P_VECTOR,				TRADER_P_IT);
	DEFINE_VECTOR	(CALifeEventPersonal*,	PERSONAL_EVENT_P_VECTOR,		PERSONAL_EVENT_P_IT);
	DEFINE_VECTOR	(MONSTER_P_VECTOR,			MONSTER_P_VECTOR_VECTOR,		MONSTER_P_VECTOR_IT);
	DEFINE_VECTOR	(CALifeKnownAnomaly*,		ANOMALY_P_VECTOR,				ANOMALY_P_IT);
	DEFINE_VECTOR	(ANOMALY_P_VECTOR,			ANOMALY_P_VECTOR_VECTOR,		ANOMALY_P_VECTOR_IT);
	DEFINE_VECTOR	(CALifeArtefactDemand*,		DEMAND_P_VECTOR,				DEMAND_P_IT);
	DEFINE_VECTOR	(STerrainPlace,				TERRAIN_VECTOR,					TERRAIN_IT);
	DEFINE_VECTOR	(STraderSupply,				TRADER_SUPPLY_VECTOR,			TRADER_SUPPLY_IT);
	DEFINE_VECTOR	(SGraphPoint,				GRAPH_POINT_VECTOR,				GRAPH_POINT_IT);
	DEFINE_VECTOR	(SArtefactTraderOrder,		ARTEFACT_TRADER_ORDER_VECTOR,	ARTEFACT_TRADER_ORDER_IT);
	DEFINE_VECTOR	(CSE_ALifeInventoryItem*,	ITEM_P_VECTOR,					ITEM_P_IT);
	DEFINE_VECTOR	(CSE_ALifeItemWeapon*,		WEAPON_P_VECTOR,				WEAPON_P_IT);
	DEFINE_VECTOR	(CSE_ALifeSchedulable*,		SCHEDULE_P_VECTOR,				SCHEDULE_P_IT);

	DEFINE_SVECTOR	(ANOMALY_P_VECTOR,			eAnomalousZoneTypeDummy,		ANOMALY_P_VECTOR_SVECTOR,	ANOMALY_P_VECTOR_SIT)

	DEFINE_SET		(_TASK_ID,					TASK_SET,						TASK_SET_IT);
	DEFINE_SET		(CSE_ALifeTrader*,			TRADER_SET,						TRADER_SET_IT);
	DEFINE_SET		(EAnomalousZoneType,		U32_SET,						U32_SET_IT);
	DEFINE_SET		(CSE_ALifeDynamicObject*,	D_OBJECT_SET,					D_OBJECT_SET_IT);

	DEFINE_MAP		(_OBJECT_ID,				TASK_SET,						OBJECT_TASK_MAP,			OBJECT_TASK_PAIR_IT);
	DEFINE_MAP		(_TASK_ID,					CALifeTask*,					TASK_MAP,					TASK_PAIR_IT);
	DEFINE_MAP		(_OBJECT_ID,				CSE_ALifeDynamicObject*,		D_OBJECT_P_MAP,				D_OBJECT_P_PAIR_IT);
	DEFINE_MAP		(_OBJECT_ID,				CSE_ALifeSchedulable*,			SCHEDULE_P_MAP,				SCHEDULE_P_PAIR_IT);
	DEFINE_MAP		(_STORY_ID,					CSE_ALifeDynamicObject*,		STORY_P_MAP,				STORY_P_PAIR_IT);

	DEFINE_MAP_PRED	(LPCSTR,					CALifeDiscovery*,				DISCOVERY_P_MAP,			DISCOVERY_P_PAIR_IT,			pred_str);
	DEFINE_MAP_PRED	(LPCSTR,					CALifeOrganization*,			ORGANIZATION_P_MAP,			ORGANIZATION_P_PAIR_IT,			pred_str);
	DEFINE_MAP_PRED	(LPCSTR,					u32,							ITEM_COUNT_MAP,				ITEM_COUNT_PAIR_IT,				pred_str);
	DEFINE_MAP_PRED	(u32,						SOrganizationOrder,				ORGANIZATION_ORDER_MAP,		ORGANIZATION_ORDER_PAIR_IT,		std::greater<u32>);
	DEFINE_MAP_PRED	(LPSTR,						bool,							LPSTR_BOOL_MAP,				LPSTR_BOOL_PAIR_IT,				pred_str);
	DEFINE_MAP_PRED	(LPSTR,						U32_SET,						ITEM_SET_MAP,				ITEM_SET_PAIR_IT,				pred_str);
	DEFINE_MAP_PRED	(LPCSTR,					TRADER_SET,						TRADER_SET_MAP,				TRADER_SET_PAIR_IT,				pred_str);
	DEFINE_MAP_PRED	(LPCSTR,					SArtefactTraderOrder*,			ARTEFACT_TRADER_ORDER_MAP,	ARTEFACT_TRADER_ORDER_PAIR_IT,	pred_str);
};

#ifdef XRGAME_EXPORTS
#	include "ai_debug.h"
#endif

#endif //XRAY_ALIFE_SPACE