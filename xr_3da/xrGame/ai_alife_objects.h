////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
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

class CALifeMonsterGroup;
class CALifeItem;
class CALifeMonster;

namespace ALife {
	typedef u64	_CLASS_ID;									// Class ID
	typedef u32	_OBJECT_ID;									// Object ID
	typedef u32	_TIME_ID;									// Time  ID
	typedef u32	_EVENT_ID;									// Event ID
	typedef u32	_TASK_ID;									// Event ID
	typedef u16	_GRAPH_ID;									// Graph ID
	typedef u16	_SPAWN_ID;									// Spawn ID
	typedef u8	_LOCATION_ID;								// Location ID
	typedef u16	_TERRAIN_ID;								// Terrain ID

	DEFINE_VECTOR	(_OBJECT_ID,	OBJECT_VECTOR,			OBJECT_IT);
	DEFINE_VECTOR	(_GRAPH_ID,		GRAPH_VECTOR,			GRAPH_IT);
	DEFINE_VECTOR	(_TASK_ID,		TASK_VECTOR,			TASK_IT);
	DEFINE_VECTOR	(CALifeMonster*,ALIFE_MONSTER_P_VECTOR, ALIFE_MONSTER_P_IT);
	DEFINE_SVECTOR	(GRAPH_VECTOR,	256,			TERRAIN_SVECTOR, TERRAIN_IT);
	DEFINE_MAP		(_OBJECT_ID,	CALifeItem *,	OBJECT_MAP,	OBJECT_PAIR_IT);

	enum EInjureType {
		eInjureTypeEat = u32(0),
		eInjureTypeShoot,
		eInjureTypeDestroyed,
		eInjureTypeDummy = u32(-1),
	};

	enum EPerception {
		ePerceptSee = u32(0),
		ePerceptHear,
		ePerceptNotice,
		ePerceptMeet,
		ePerceptDummy = u32(-1),
	};

	enum EBattleResult {
		eBattleRun1 = u32(0),
		eBattleRun2,
		eBattleRun12,
		eBattleRun1WF,
		eBattleRun2WF,
		eBattleDummy = u32(-1),
	};

	enum ETaskType {
		eTaskTypeSearchForArtefact = u32(0),
		eTaskTypeSearchForItem,
		eTaskTypeDummy = u32(-1),
	};

	typedef struct tagSTask {
		_TASK_ID					tTaskID;
		_TIME_ID					tTimeID;
		_GRAPH_ID					tGraphID;
		_OBJECT_ID					tCustomerID;
		ETaskType					tTaskType;
	} STask;

	typedef struct tagSEvent {
		_EVENT_ID					tEventID;
		_TIME_ID					tTimeID;
		_GRAPH_ID					tGraphID;
		EBattleResult				tBattleResult;
		u8							ucMonster1CountAfter;
		u8							ucMonster2CountAfter;
		CALifeMonsterGroup			*tpMonsterGroup1;
		CALifeMonsterGroup			*tpMonsterGroup2;
	} SEvent;

	typedef struct tagSPersonalEvent {
		_EVENT_ID					tEventID;
		_TIME_ID					tGameTime;
		_TASK_ID					tTaskID;
		OBJECT_VECTOR				tpItemIDs;
		int							iHealth;
		EPerception					tPerception;
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
		GRAPH_VECTOR				wpRouteGraphPoints;
	} SSpawnPoint;

	typedef struct tagSALifeHeader {
		u32							dwVersion;
		_TIME_ID					tTimeID;
	} SALifeHeader;

	DEFINE_VECTOR	(SSpawnPoint,	SPAWN_VECTOR,			SPAWN_IT);
	DEFINE_VECTOR	(SPersonalEvent,PERSONAL_EVENT_VECTOR,	PERSONAL_EVENT_IT);

	DEFINE_MAP		(_EVENT_ID,		SEvent,			EVENT_MAP,	EVENT_PAIR_IT);
	DEFINE_MAP		(_TASK_ID,		STask,			TASK_MAP,	TASK_PAIR_IT);
};

using namespace ALife;

class CALifeObject {
public:
	virtual void					Save(CFS_Memory	&tMemoryStream) = 0;
	virtual void					Load(CStream	&tFileStream)	= 0;
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints) = 0;
};

class CALifeMonsterGroup : public CALifeObject {
public:
	_CLASS_ID						m_tClassID;
	_OBJECT_ID						m_tObjectID;
	_SPAWN_ID						m_tSpawnID;
	_GRAPH_ID						m_tGraphID;
	u16								m_wCount;
	
	virtual	void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write	(&m_tClassID,	sizeof(m_tClassID));
		tMemoryStream.write	(&m_tObjectID,	sizeof(m_tObjectID));
		tMemoryStream.write	(&m_tSpawnID,	sizeof(m_tSpawnID));
		tMemoryStream.write	(&m_tGraphID,	sizeof(m_tGraphID));
		tMemoryStream.write	(&m_wCount,		sizeof(m_wCount));
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read	(&m_tClassID,	sizeof(m_tClassID));
		tFileStream.Read	(&m_tObjectID,	sizeof(m_tObjectID));
		tFileStream.Read	(&m_tSpawnID,	sizeof(m_tSpawnID));
		tFileStream.Read	(&m_tGraphID,	sizeof(m_tGraphID));
		tFileStream.Read	(&m_wCount,		sizeof(m_wCount));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		m_tClassID	= _CLASS_ID(pSettings->ReadSTRING(tpSpawnPoints[tSpawnID].caModel, "class"));
		m_tGraphID	= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tSpawnID	= tSpawnID;
		m_wCount	= tpSpawnPoints[tSpawnID].wCount;
	}
};

class CALifeItem : public CALifeMonsterGroup {
public:
	typedef	CALifeMonsterGroup inherited;
	
	_TIME_ID						m_tTimeID;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save		(tMemoryStream);
		tMemoryStream.write	(&m_tTimeID,	sizeof(m_tTimeID));
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load		(tFileStream);
		tFileStream.Read	(&m_tTimeID,	sizeof(m_tTimeID));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tTimeID = 0;
	}
};

class CALifeCorp : public CALifeItem {
public:
	typedef	CALifeItem inherited;
	
	EInjureType					m_tInjureType;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save		(tMemoryStream);
		tMemoryStream.write	(&m_tInjureType,	sizeof(m_tInjureType));
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load		(tFileStream);
		tFileStream.Read	(&m_tInjureType,	sizeof(m_tInjureType));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tInjureType = eInjureTypeDummy;
	}
};

class CALifeMonster : public CALifeCorp {
public:
	typedef	CALifeCorp inherited;
	
	_GRAPH_ID						m_tNextGraphID;
	_GRAPH_ID						m_tPrevGraphID;
	float							m_fCurSpeed;
	float							m_fMinSpeed;
	float							m_fMaxSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	s32								m_iHealth;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save		(tMemoryStream);
		tMemoryStream.write	(&m_tNextGraphID,			sizeof(m_tNextGraphID));
		tMemoryStream.write	(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
		tMemoryStream.write	(&m_fMinSpeed,				sizeof(m_fMinSpeed));
		tMemoryStream.write	(&m_fMaxSpeed,				sizeof(m_fMaxSpeed));
		tMemoryStream.write	(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tMemoryStream.write	(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tMemoryStream.write	(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
		tMemoryStream.write	(&m_iHealth,				sizeof(m_iHealth));
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load		(tFileStream);
		tFileStream.Read	(&m_tNextGraphID,			sizeof(m_tNextGraphID));
		tFileStream.Read	(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
		tFileStream.Read	(&m_fMinSpeed,				sizeof(m_fMinSpeed));
		tFileStream.Read	(&m_fMaxSpeed,				sizeof(m_fMaxSpeed));
		tFileStream.Read	(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tFileStream.Read	(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tFileStream.Read	(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
		tFileStream.Read	(&m_iHealth,				sizeof(m_iHealth));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init		(tSpawnID,tpSpawnPoints);
		m_tNextGraphID		= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tPrevGraphID		= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_fMinSpeed			= pSettings->ReadFLOAT	(tpSpawnPoints[tSpawnID].caModel, "MinSpeed");
		m_fMaxSpeed			= pSettings->ReadFLOAT	(tpSpawnPoints[tSpawnID].caModel, "MaxSpeed");
		m_fCurSpeed			= 0.0f;
		m_fDistanceFromPoint= 0.0f;
		m_fDistanceToPoint	= 0.0f;
		m_iHealth			= pSettings->ReadINT	(tpSpawnPoints[tSpawnID].caModel, "Health");
	}
};

class CALifeHuman : public CALifeMonster {
public:
	typedef	CALifeMonster inherited;
	
	PERSONAL_EVENT_VECTOR			m_tpEvents;
	OBJECT_VECTOR					m_tpItemIDs;
	TASK_VECTOR						m_tpTaskIDs;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		// calling inherited
		inherited::Save(tMemoryStream);
		// saving items
		tMemoryStream.Wdword	(m_tpItemIDs.size());
		for (int i=0; i<(int)m_tpItemIDs.size(); i++)
			tMemoryStream.write	(&(m_tpItemIDs[i]),sizeof(m_tpItemIDs[i]));
		// saving events
		tMemoryStream.Wdword(m_tpEvents.size());
		for (int i=0; i<(int)m_tpEvents.size(); i++) {
			SPersonalEvent &tPersonalEvent = m_tpEvents[i];
			tMemoryStream.write	(&tPersonalEvent.tEventID,			sizeof(tPersonalEvent.tEventID));
			tMemoryStream.write	(&tPersonalEvent.tGameTime,			sizeof(tPersonalEvent.tGameTime));
			tMemoryStream.write	(&tPersonalEvent.tTaskID,			sizeof(tPersonalEvent.tTaskID));
			tMemoryStream.Wdword(tPersonalEvent.tpItemIDs.size());
			for (int j=0; j<(int)tPersonalEvent.tpItemIDs.size(); j++)
				tMemoryStream.write	(&(tPersonalEvent.tpItemIDs[j]),sizeof(tPersonalEvent.tpItemIDs[j]));
			tMemoryStream.write	(&tPersonalEvent.iHealth,			sizeof(tPersonalEvent.iHealth));
			tMemoryStream.write	(&tPersonalEvent.tPerception,		sizeof(tPersonalEvent.tPerception));
		}
		// saving tasks
		tMemoryStream.Wdword(m_tpTaskIDs.size());
		for (int i=0; i<(int)m_tpTaskIDs.size(); i++)
			tMemoryStream.write	(&(m_tpTaskIDs[i]),sizeof(m_tpTaskIDs[i]));
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		// calling inherited
		inherited::Load(tFileStream);
		// loading items
		m_tpItemIDs.resize(tFileStream.Rdword());
		for (int i=0; i<(int)m_tpItemIDs.size(); i++)
			tFileStream.Read	(&(m_tpItemIDs[i]),sizeof(m_tpItemIDs[i]));
		// loading events
		m_tpEvents.resize(tFileStream.Rdword());
		for (int i=0; i<(int)m_tpEvents.size(); i++) {
			SPersonalEvent &tPersonalEvent = m_tpEvents[i];
			tFileStream.Read	(&tPersonalEvent.tEventID,			sizeof(tPersonalEvent.tEventID));
			tFileStream.Read	(&tPersonalEvent.tGameTime,			sizeof(tPersonalEvent.tGameTime));
			tFileStream.Read	(&tPersonalEvent.tTaskID,			sizeof(tPersonalEvent.tTaskID));
			tPersonalEvent.tpItemIDs.resize(tFileStream.Rdword());
			for (int j=0; j<(int)tPersonalEvent.tpItemIDs.size(); j++)
				tFileStream.Read(&(tPersonalEvent.tpItemIDs[j]),	sizeof(tPersonalEvent.tpItemIDs[j]));
			tFileStream.Read	(&tPersonalEvent.iHealth,			sizeof(tPersonalEvent.iHealth));
			tFileStream.Read	(&tPersonalEvent.tPerception,		sizeof(tPersonalEvent.tPerception));
		}
		// loading tasks
		m_tpTaskIDs.resize(tFileStream.Rdword());
		for (int i=0; i<(int)m_tpTaskIDs.size(); i++)
			tFileStream.Read	(&(m_tpTaskIDs[i]),sizeof(m_tpTaskIDs[i]));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tpEvents.	clear();
		m_tpItemIDs.clear();
		m_tpTaskIDs.clear();
	}
};

class CRemovePredicate {
	EVENT_MAP	*m_tpMap;
public:
	CRemovePredicate(EVENT_MAP &tpMap)
	{
		m_tpMap = &tpMap;
	};

	IC bool operator()(const SPersonalEvent &tPersonalEvent)  const
	{
		return(m_tpMap->find(tPersonalEvent.tEventID) == m_tpMap->end());
	};
};
