////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_space.h
//	Created 	: 08.01.2002
//  Modified 	: 08.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life space
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_ALIFE_SPACE
#define XRAY_AI_ALIFE_SPACE

// spawn points
#define SPAWN_POINT_VERSION			0x0001
#define SPAWN_POINT_CHUNK_VERSION	0xffff
// ALife objects, events and tasks
#define ALIFE_VERSION				0x0001
#define ALIFE_CHUNK_DATA			0x0000
#define OBJECT_CHUNK_DATA			0x0001
#define EVENT_CHUNK_DATA			0x0002
#define TASK_CHUNK_DATA				0x0003
#define GAME_TIME_CHUNK_DATA		0x0004
#define ANOMALY_CHUNK_DATA			0x0005
#define DISCOVERY_CHUNK_DATA		0x0006
#define LOCATION_TYPE_COUNT			4
#define SECTION_HEADER				"location_"
#define SAVE_EXTENSION				".sav"
#define SPAWN_NAME					"game.spawn"
//#define DEBUG_LOG

class CSE_ALifeEventGroup;
class CSE_ALifeDynamicObject;
class CSE_ALifeMonsterAbstract;
class CSE_ALifeTrader;
class CSE_ALifeEvent;
class CSE_ALifeTask;
class CSE_ALifePersonalEvent;
class CSE_ALifePersonalTask;
class CSE_ALifeKnownAnomaly;
class CSE_ALifeDiscovery;
class CSE_ALifeOrganization;
class CSE_ALifeArtefactDemand;
class CSE_ALifeArtefactNeed;

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

	const	u32	LOCATION_COUNT	= (u32(1) << (8*sizeof(_LOCATION_ID)));

	DEFINE_VECTOR	(float,						FLOAT_VECTOR,					FLOAT_IT);
	DEFINE_VECTOR	(LPSTR,						LPSTR_VECTOR,					LPSTR_IT);
	DEFINE_VECTOR	(Fvector,					FVECTOR_VECTOR,					FVECTOR_IT);
	DEFINE_VECTOR	(_OBJECT_ID,				OBJECT_VECTOR,					OBJECT_IT);
	DEFINE_VECTOR	(_OBJECT_ID,				OBJECT_VECTOR,					OBJECT_IT);
	DEFINE_VECTOR	(_EVENT_ID,					EVENT_VECTOR,					EVENT_IT);
	DEFINE_VECTOR	(_TASK_ID,					TASK_VECTOR,					TASK_IT);
	DEFINE_VECTOR	(_GRAPH_ID,					GRAPH_VECTOR,					GRAPH_IT);
	DEFINE_VECTOR	(OBJECT_VECTOR,				OBJECT_VECTOR_VECTOR,			OBJECT_VECTOR_IT);
	DEFINE_VECTOR	(CSE_ALifeDynamicObject*,	DYNAMIC_OBJECT_P_VECTOR,		DYNAMIC_OBJECT_P_IT);
	DEFINE_VECTOR	(CSE_ALifeMonsterAbstract*,	ALIFE_MONSTER_P_VECTOR,			ALIFE_MONSTER_P_IT);
	DEFINE_VECTOR	(CSE_ALifeTrader*,			TRADER_P_VECTOR,				TRADER_P_IT);
	DEFINE_VECTOR	(CSE_ALifeEvent*,			EVENT_P_VECTOR,					EVENT_P_IT);
	DEFINE_VECTOR	(CSE_ALifePersonalEvent*,	PERSONAL_EVENT_P_VECTOR,		PERSONAL_EVENT_P_IT);
	DEFINE_VECTOR	(CSE_ALifePersonalTask*,	PERSONAL_TASK_P_VECTOR,			PERSONAL_TASK_P_IT);
	DEFINE_VECTOR	(ALIFE_MONSTER_P_VECTOR,	ALIFE_MONSTER_P_VECTOR_VECTOR,	ALIFE_MONSTER_P_VECTOR_IT);
	DEFINE_VECTOR	(CSE_ALifeDynamicObject*,	ALIFE_ENTITY_P_VECTOR,			ALIFE_ENTITY_P_IT);
	DEFINE_VECTOR	(CSE_ALifeKnownAnomaly*,	ANOMALY_P_VECTOR,				ANOMALY_P_IT);
	DEFINE_VECTOR	(ANOMALY_P_VECTOR,			ANOMALY_P_VECTOR_VECTOR,		ANOMALY_P_VECTOR_IT);
	DEFINE_VECTOR	(CSE_ALifeArtefactDemand*,	DEMAND_P_VECTOR,				DEMAND_P_IT);

	DEFINE_SVECTOR	(GRAPH_VECTOR,				LOCATION_COUNT,					GRAPH_VECTOR_SVECTOR,		GRAPH_VECTOR_IT);
	
	DEFINE_MAP		(_OBJECT_ID,				CSE_ALifeDynamicObject*,		OBJECT_MAP,					OBJECT_PAIR_IT);
	DEFINE_MAP		(_EVENT_ID,					CSE_ALifeEvent*,				EVENT_MAP,					EVENT_PAIR_IT);
	DEFINE_MAP		(_TASK_ID,					CSE_ALifeTask*,					TASK_MAP,					TASK_PAIR_IT);
	DEFINE_MAP		(u8,						ALIFE_ENTITY_P_VECTOR*,			ALIFE_ENTITY_P_VECTOR_MAP,	ALIFE_ENTITY_P_VECTOR_PAIR_IT);
	DEFINE_MAP_PRED	(LPCSTR,					CSE_ALifeDiscovery*,			DISCOVERY_P_MAP,			DISCOVERY_P_PAIR_IT,	pred_str);
	DEFINE_MAP_PRED	(LPCSTR,					CSE_ALifeOrganization*,			ORGANIZATION_P_MAP,			ORGANIZATION_P_PAIR_IT, pred_str);
	DEFINE_MAP_PRED	(LPSTR,						u32,							ITEM_COUNT_MAP,				ITEM_COUNT_PAIR_IT,		pred_str);

	typedef struct tagSGraphPoint {
		ALIFE_ENTITY_P_VECTOR		tpObjects;
		EVENT_P_VECTOR				tpEvents;
	} SGraphPoint;
	
	DEFINE_VECTOR	(SGraphPoint,				GRAPH_POINT_VECTOR,				GRAPH_POINT_IT);

	typedef struct tagSLevelPoint {
		Fvector		tPoint;
		u32			tNodeID;
		float		fDistance;	
	} SLevelPoint;

	DEFINE_VECTOR	(SLevelPoint,				LEVEL_POINT_VECTOR,				LEVEL_POINT_IT);

	typedef struct tagSTerrainPlace{
		svector<_LOCATION_ID,LOCATION_TYPE_COUNT>	tMask;
		u32											dwMinTime;
		u32											dwMaxTime;
	} STerrainPlace;

	DEFINE_VECTOR(STerrainPlace, TERRAIN_VECTOR, TERRAIN_IT);

	struct SArtefactOrder {
		string64				m_caSection;
		u32						m_dwCount;
		u32						m_dwPrice;
	};

	DEFINE_VECTOR		(SArtefactOrder,				ARTEFACT_ORDER_VECTOR,			ARTEFACT_ORDER_IT);

	struct SOrganizationOrder {
		CSE_ALifeOrganization	*m_tpALifeOrganization;
		u32						m_dwCount;
	};

	DEFINE_MAP_PRED		(u32,							SOrganizationOrder,				ORGANIZATION_ORDER_MAP,		ORGANIZATION_ORDER_PAIR_IT, std::greater<u32>);

	struct STraderSupply {
		string256				m_caSections;
		u32						m_dwCount;
		float					m_fMinFactor;
		float					m_fMaxFactor;
	};

	DEFINE_VECTOR		(STraderSupply,					TRADER_SUPPLY_VECTOR,			TRADER_SUPPLY_IT);

	enum EInjureType {
		eInjureTypeNone = u32(0),
		eInjureTypeEat,
		eInjureTypeShoot,
		eInjureTypeDestroyed,
		eInjureTypeDummy = u32(-1),
	};

	enum ERelation {
		eRelationNone = u32(0),
		eRelationMeet,
		eRelationWatch,
		eRelationHear,
		eRelationSeeResult,
		eRelationMeetTold,
		eRelationWatchTold,
		eRelationHearTold,
		eRelationSeeResultTold,
		eRelationDummy = u32(-1),
	};

	enum EBattleResult {
		eBattleNone = u32(0),
		eBattleRun1,
		eBattleRun2,
		eBattleRun12,
		eBattleRun1WF,
		eBattleRun2WF,
		eBattleDummy = u32(-1),
	};

	enum ETaskType {
		eTaskTypeNone = u32(0),
		eTaskTypeSearchForItemCG,
		eTaskTypeSearchForItemCL,
		eTaskTypeSearchForItemOG,
		eTaskTypeSearchForItemOL,
		eTaskTypeDummy = u32(-1),
	};

	enum ETaskState {
		eTaskStateNoTask = u32(0),
		eTaskStateGoToTrader,
		eTaskStateChooseTask,
		eTaskStateGoing,
		eTaskStateSearching,
		eTaskStateDummy = u32(-1),
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
		eZoneStateAfterSurge,
		eZoneStateDummy = u32(-1),
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
};

#endif