////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_space.h
//	Created 	: 08.01.2002
//  Modified 	: 08.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life space
////////////////////////////////////////////////////////////////////////////

#pragma once

// spawn points
#define SPAWN_POINT_VERSION			0x0001

#define SPAWN_POINT_CHUNK_VERSION	0x0001
#define SPAWN_POINT_CHUNK_DATA		0x0002
	
// ALife objects, events and tasks
#define ALIFE_VERSION				0x0001

#define ALIFE_CHUNK_VERSION			0x0001
#define OBJECT_CHUNK_DATA			0x0002
#define EVENT_CHUNK_DATA			0x0003
#define TASK_CHUNK_DATA				0x0004
#define ALIFE_CHUNK_DATA			0x0005

#define ALIFE_ITEM_ID				0x00
#define ALIFE_MONSTER_ID			0x01
#define ALIFE_MONSTER_GROUP_ID		0x02
#define ALIFE_HUMAN_ID				0x03
#define ALIFE_HUMAN_GROUP_ID		0x04
#define ALIFE_ANOMALOUS_ZONE_ID		0x05

class CALifeMonsterParams;
class CALifeHumanParams;
class CALifeEventGroup;
class CALifeDynamicObject;
class CALifeDynamicObject;
class CALifeMonsterAbstract;
class CALifeHuman;

namespace ALife {
	typedef u64	_CLASS_ID;									// Class ID
	typedef u32	_OBJECT_ID;									// Object ID
	typedef u64	_TIME_ID;									// Time  ID
	typedef u32	_EVENT_ID;									// Event ID
	typedef u32	_TASK_ID;									// Event ID
	typedef u16	_GRAPH_ID;									// Graph ID
	typedef u16	_SPAWN_ID;									// Spawn ID
	typedef u16	_TERRAIN_ID;								// Terrain ID
	typedef u8	_LOCATION_ID;								// Location ID

	const	u32	LOCATION_COUNT	= (u32(1) << (8*sizeof(_LOCATION_ID)));

	DEFINE_VECTOR	(_OBJECT_ID,				OBJECT_VECTOR,					OBJECT_IT);
	DEFINE_VECTOR	(_OBJECT_ID,				OBJECT_VECTOR,					OBJECT_IT);
	DEFINE_VECTOR	(_EVENT_ID,					EVENT_VECTOR,					EVENT_IT);
	DEFINE_VECTOR	(_TASK_ID,					TASK_VECTOR,					TASK_IT);
	DEFINE_VECTOR	(_GRAPH_ID,					GRAPH_VECTOR,					GRAPH_IT);
	DEFINE_VECTOR	(OBJECT_VECTOR,				OBJECT_VECTOR_VECTOR,			OBJECT_VECTOR_IT);
	DEFINE_VECTOR	(CALifeMonsterParams,		MONSTER_PARAMS_VECTOR,			MONSTER_PARAMS_IT);
	DEFINE_VECTOR	(CALifeHumanParams,			HUMAN_PARAMS_VECTOR,			HUMAN_PARAMS_IT);
	DEFINE_VECTOR	(CALifeDynamicObject *,		DYNAMIC_OBJECT_P_VECTOR,		DYNAMIC_OBJECT_P_IT);
	DEFINE_VECTOR	(CALifeMonsterAbstract *,	ALIFE_MONSTER_P_VECTOR,			ALIFE_MONSTER_P_IT);
	DEFINE_VECTOR	(ALIFE_MONSTER_P_VECTOR,	ALIFE_MONSTER_P_VECTOR_VECTOR,	ALIFE_MONSTER_P_VECTOR_IT);
	DEFINE_VECTOR	(CALifeHuman *,				HUMAN_P_VECTOR,					HUMAN_P_IT);
	
	DEFINE_SVECTOR	(GRAPH_VECTOR,				LOCATION_COUNT,					GRAPH_VECTOR_SVECTOR,	GRAPH_VECTOR_IT);
	
	DEFINE_MAP		(_OBJECT_ID,				CALifeDynamicObject *,			OBJECT_MAP,				OBJECT_PAIR_IT);

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

	enum EAnomalousZoneType {
		eAnomalousZoneTypeNone = u32(0),
		eAnomalousZoneTypeGravi,
		eAnomalousZoneTypeFog,
		eAnomalousZoneTypeRadio,
		eAnomalousZoneTypePlant,
		eAnomalousZoneTypeGelatine,
		eAnomalousZoneTypeFluff,
		eAnomalousZoneTypeRustyHair,
		eAnomalousZoneTypeRustyWhistlers,
		eAnomalousZoneTypeDummy = u32(-1),
	};

	typedef struct tagSAnomalousZone {
		EAnomalousZoneType			tAnomalousZone;

	} SAnomalousZone;

	typedef struct tagSTask {
		_TASK_ID					tTaskID;
		_TIME_ID					tTimeID;
		_OBJECT_ID					tCustomerID;
		union {
			_CLASS_ID				tClassID;
			_OBJECT_ID				tObjectID;
		};
		union {
			_LOCATION_ID			tLocationID;
			_GRAPH_ID				tGraphID;
		};
		ETaskType					tTaskType;
	} STask;

	typedef struct tagSEvent {
		_EVENT_ID					tEventID;
		_TIME_ID					tTimeID;
		_GRAPH_ID					tGraphID;
		EBattleResult				tBattleResult;
		CALifeEventGroup			*tpMonsterGroup1;
		CALifeEventGroup			*tpMonsterGroup2;
	} SEvent;

	typedef struct tagSPersonalEvent {
		_EVENT_ID					tEventID;
		_TIME_ID					tGameTime;
		_TASK_ID					tTaskID;
		OBJECT_VECTOR				tpItemIDs;
		int							iHealth;
		ERelation					tRelation;
	} SPersonalEvent;
	
	typedef struct tagSSpawnHeader {
		u32							dwVersion;
		u32							dwCount;
	} SSpawnHeader;

	typedef struct tagSSpawnPoint {
		_GRAPH_ID					tNearestGraphPointID;
		string64					caModel;
		u8							ucTeam;
		u8							ucSquad;
		u8							ucGroup;
		u16							wGroupID;
		u16							wCount;
		float						fBirthRadius;
		float						fBirthProbability;
		float						fIncreaseCoefficient;
		float						fAnomalyDeathProbability;
		u8							ucRoutePointCount;
		GRAPH_VECTOR				tpRouteGraphPoints;
	} SSpawnPoint;

	typedef struct tagSALifeHeader {
		u32							dwVersion;
	} SALifeHeader;

	DEFINE_VECTOR	(SEvent *,					EVENT_P_VECTOR,			EVENT_P_IT);

	typedef struct tagSGraphPoint {
		DYNAMIC_OBJECT_P_VECTOR		tpObjects;
		EVENT_P_VECTOR				tpEvents;
	} SGraphPoint;
	
	DEFINE_VECTOR	(SSpawnPoint,				SPAWN_VECTOR,			SPAWN_IT);
	DEFINE_VECTOR	(SPersonalEvent,			PERSONAL_EVENT_VECTOR,	PERSONAL_EVENT_IT);
	DEFINE_VECTOR	(SGraphPoint,				GRAPH_POINT_VECTOR,		GRAPH_POINT_IT);

	DEFINE_MAP		(_EVENT_ID,					SEvent,					EVENT_MAP,				EVENT_PAIR_IT);
	DEFINE_MAP		(_TASK_ID,					STask,					TASK_MAP,				TASK_PAIR_IT);
};

