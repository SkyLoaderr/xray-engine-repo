////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

// spawn points
#define SPAWN_POINT_VERSION				0x0001

#define SPAWN_POINT_CHUNK_VERSION		0x0001
#define SPAWN_POINT_CHUNK_DATA			0x0002
	
// ALife objects, events and tasks
#define ALIFE_VERSION					0x0001

#define ALIFE_CHUNK_VERSION				0x0001
#define OBJECT_CHUNK_DATA				0x0002
#define EVENT_CHUNK_DATA				0x0003
#define TASK_CHUNK_DATA					0x0004

class CALifeMonsterGroup;

namespace ALife {
	typedef u64	CLASSID;										// Class ID
	typedef u32	OBJECTID;										// Object ID
	typedef u32	TIMEID;											// Time  ID
	typedef u32	EVENTID;										// Event ID
	typedef u32	TASKID;											// Event ID
	typedef u16	GRAPHID;										// Graph  ID
	typedef u16	SPAWNID;										// Spawn ID
	typedef u16	TERRAINID;										// Spawn ID

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
		TASKID							tTaskID;
		GRAPHID							tGraphPointID;
		OBJECTID						tCustomerGOID;
		ETaskType						tTaskType;
		TERRAINID						tTerrain;
	} STask;

	typedef struct tagSEvent {
		TIMEID							tGameTimeID;
		GRAPHID							tGraphPointID;
		CALifeMonsterGroup				*tpMonsterGroup1;
		CALifeMonsterGroup				*tpMonsterGroup2;
		u8								ucMonster1CountAfter;
		u8								ucMonster2CountAfter;
		EBattleResult					tBattleResult;
	} SEvent;

	typedef struct tagSPersonalEvent {
		EVENTID							tEventID;
		TIMEID							tGameTime;
		TASKID							tTaskID;
		vector<OBJECTID>				tpItemIDs;
		int								iHealth;
		EPerception						tPerception;
	} SPersonalEvent;

	typedef struct tagSSpawnHeader {
		u32								dwVersion;
		u32								dwCount;
	} SSpawnHeader;

	typedef struct tagSSpawnPoint {
		GRAPHID							tNearestGraphPointID;
		string64						caModel;
		u8								ucTeam;
		u8								ucSquad;
		u8								ucGroup;
		u16								wGroupID;
		u16								wCount;
		float							fBirthRadius;
		float							fBirthProbability;
		float							fIncreaseCoefficient;
		float							fAnomalyDeathProbability;
		u8								ucRoutePointCount;
		vector<GRAPHID>					wpRouteGraphPoints;
	} SSpawnPoint;

	typedef struct tagSALifeHeader {
		u32								dwVersion;
		TIMEID							tTimeID;
	} SALifeHeader;
};

using namespace ALife;

class CALifeObject {
public:
	virtual void						Save(CFS_Memory		&tMemoryStream) = 0;
	virtual void						Load(CStream		&tFileStream)	= 0;
	virtual void						Init(ALife::SPAWNID	tSpawnID, vector<ALife::SSpawnPoint> &tpSpawnPoints) = 0;
};

class CALifeMonsterGroup : public CALifeObject {
public:
	ALife::CLASSID						m_tClassID;
	ALife::OBJECTID						m_tObjectID;
	ALife::SPAWNID						m_tSpawnID;
	ALife::GRAPHID						m_tGraphID;
	u16									m_wCount;
	
	virtual	void						Save(CFS_Memory		&tMemoryStream)
	{
		tMemoryStream.write	(&m_tClassID,	sizeof(m_tClassID));
		tMemoryStream.write	(&m_tObjectID,	sizeof(m_tObjectID));
		tMemoryStream.write	(&m_tSpawnID,	sizeof(m_tSpawnID));
		tMemoryStream.write	(&m_tGraphID,	sizeof(m_tGraphID));
		tMemoryStream.write	(&m_wCount,		sizeof(m_wCount));
	};

	virtual	void						Load(CStream		&tFileStream)
	{
		tFileStream.Read	(&m_tClassID,	sizeof(m_tClassID));
		tFileStream.Read	(&m_tObjectID,	sizeof(m_tObjectID));
		tFileStream.Read	(&m_tSpawnID,	sizeof(m_tSpawnID));
		tFileStream.Read	(&m_tGraphID,	sizeof(m_tGraphID));
		tFileStream.Read	(&m_wCount,		sizeof(m_wCount));
	};

	virtual void						Init(ALife::SPAWNID	tSpawnID, vector<ALife::SSpawnPoint> &tpSpawnPoints)
	{
		m_tClassID	= ALife::CLASSID(pSettings->ReadSTRING(tpSpawnPoints[tSpawnID].caModel, "class"));
		m_tGraphID	= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tSpawnID	= tSpawnID;
		m_wCount	= tpSpawnPoints[tSpawnID].wCount;
	}
};

class CALifeItem : public CALifeMonsterGroup {
public:
	typedef	CALifeMonsterGroup inherited;
	
	ALife::TIMEID						m_tTimeID;
	
	virtual	void						Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save		(tMemoryStream);
		tMemoryStream.write	(&m_tTimeID,	sizeof(m_tTimeID));
	};
	
	virtual	void						Load(CStream	&tFileStream)
	{
		inherited::Load		(tFileStream);
		tFileStream.Read	(&m_tTimeID,	sizeof(m_tTimeID));
	};

	virtual void						Init(ALife::SPAWNID	tSpawnID, vector<ALife::SSpawnPoint> &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tTimeID = 0;
	}
};

class CALifeCorp : public CALifeItem {
public:
	typedef	CALifeItem inherited;
	
	ALife::EInjureType					m_tInjureType;
	
	virtual	void						Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save		(tMemoryStream);
		tMemoryStream.write	(&m_tInjureType,	sizeof(m_tInjureType));
	};
	
	virtual	void						Load(CStream	&tFileStream)
	{
		inherited::Load		(tFileStream);
		tFileStream.Read	(&m_tInjureType,	sizeof(m_tInjureType));
	};

	virtual void						Init(ALife::SPAWNID	tSpawnID, vector<ALife::SSpawnPoint> &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tInjureType = eInjureTypeDummy;
	}
};

class CALifeMonster : public CALifeCorp {
public:
	typedef	CALifeCorp inherited;
	
	ALife::GRAPHID						m_tCurrentGraphPointID;
	ALife::GRAPHID						m_tNextGraphPointID;
	ALife::GRAPHID						m_tPrevGraphPointID;
	float								m_fCurSpeed;
	float								m_fMinSpeed;
	float								m_fMaxSpeed;
	float								m_fDistanceFromPoint;
	float								m_fDistanceToPoint;
	s32									m_iHealth;
	
	virtual	void						Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save		(tMemoryStream);
		tMemoryStream.write	(&m_tCurrentGraphPointID,	sizeof(m_tCurrentGraphPointID));
		tMemoryStream.write	(&m_tNextGraphPointID,		sizeof(m_tNextGraphPointID));
		tMemoryStream.write	(&m_tPrevGraphPointID,		sizeof(m_tPrevGraphPointID));
		tMemoryStream.write	(&m_fMinSpeed,				sizeof(m_fMinSpeed));
		tMemoryStream.write	(&m_fMaxSpeed,				sizeof(m_fMaxSpeed));
		tMemoryStream.write	(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tMemoryStream.write	(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tMemoryStream.write	(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
		tMemoryStream.write	(&m_iHealth,				sizeof(m_iHealth));
	};
	
	virtual	void						Load(CStream	&tFileStream)
	{
		inherited::Load		(tFileStream);
		tFileStream.Read	(&m_tCurrentGraphPointID,	sizeof(m_tCurrentGraphPointID));
		tFileStream.Read	(&m_tNextGraphPointID,		sizeof(m_tNextGraphPointID));
		tFileStream.Read	(&m_tPrevGraphPointID,		sizeof(m_tPrevGraphPointID));
		tFileStream.Read	(&m_fMinSpeed,				sizeof(m_fMinSpeed));
		tFileStream.Read	(&m_fMaxSpeed,				sizeof(m_fMaxSpeed));
		tFileStream.Read	(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tFileStream.Read	(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tFileStream.Read	(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
		tFileStream.Read	(&m_iHealth,				sizeof(m_iHealth));
	};

	virtual void						Init(ALife::SPAWNID	tSpawnID, vector<ALife::SSpawnPoint> &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_iHealth				= pSettings->ReadINT	(tpSpawnPoints[tSpawnID].caModel, "Health");
		m_fMinSpeed				= pSettings->ReadFLOAT	(tpSpawnPoints[tSpawnID].caModel, "MinSpeed");
		m_fMaxSpeed				= pSettings->ReadFLOAT	(tpSpawnPoints[tSpawnID].caModel, "MaxSpeed");
		m_fCurSpeed				= 0.0f;
		m_fDistanceFromPoint	= 0.0f;
		m_fDistanceToPoint		= 0.0f;
		m_tCurrentGraphPointID	= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tNextGraphPointID		= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tPrevGraphPointID		= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
	}
};

class CALifeHuman : public CALifeMonster {
public:
	typedef	CALifeMonster inherited;
	
	vector<ALife::OBJECTID>				m_tpItemIDs;
	vector<ALife::SPersonalEvent>		m_tpEvents;
	vector<ALife::TASKID>				m_tpTaskIDs;

	virtual	void						Save(CFS_Memory &tMemoryStream)
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

	virtual	void						Load(CStream	&tFileStream)
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

	virtual void						Init(ALife::SPAWNID	tSpawnID, vector<ALife::SSpawnPoint> &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tCurrentGraphPointID	= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tNextGraphPointID		= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tPrevGraphPointID		= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
	}
};

class CALifeObjectRegistry {
public:
	ALife::OBJECTID						m_dwEventID;			// идентификатор карты событий
	map<ALife::OBJECTID,CALifeItem *>	m_tppMap;				// список событий игры
	map<ALife::OBJECTID,CALifeItem *>::iterator	m_tpIterator;

	virtual	void						Save(CFS_Memory &tMemoryStream)
	{
	};

	virtual	void						Load(CStream	&tFileStream)
	{
	};

	virtual	void						Add	(CALifeItem *tpALifeItem)
	{
	};

};

class CALifeEventRegistry {
public:
	ALife::EVENTID						m_tEventID;				// идентификатор карты событий
	map<ALife::EVENTID,ALife::SEvent>	m_tpMap;				// список событий игры
	map<ALife::EVENTID,ALife::SEvent>::iterator	m_tpIterator;

	virtual	void						Save(CFS_Memory &tMemoryStream)
	{
	};

	virtual	void						Load(CStream	&tFileStream)
	{
	};
	
	virtual	void						Add	(ALife::SEvent	&tEvent)
	{
	};
};

class CALifeTaskRegistry {
public:
	ALife::TASKID						m_dwTaskID;				// идентификатор карты событий
	map<ALife::TASKID,ALife::STask>		m_tpMap;				// список событий игры
	map<ALife::TASKID,ALife::STask>::iterator	m_tpIterator;

	virtual	void						Save(CFS_Memory &tMemoryStream)
	{
	};
	
	virtual	void						Load(CStream	&tFileStream)
	{
	};
	
	virtual	void						Add	(ALife::STask	&tTask)
	{
	};
};

class CAI_ALife : public CSheduled {
public:
	typedef CSheduled inherited;

	u32									m_dwObjectsBeingProcessed;
	u64									m_qwMaxProcessTime;
	bool								m_bLoaded;
	
	ALife::SSpawnHeader					m_tSpawnHeader;
	ALife::SALifeHeader					m_tALifeHeader;

	// auto
	svector<vector<ALife::GRAPHID>,256>	m_tpTerrain;			// массив списков: по идетнификатору 
																//	местности получить список точек 
																//  графа
	vector<vector<ALife::OBJECTID> >	m_tpLocationOwners;		// массив списков : по точке графа 
																//  получить список её владельцев
	vector<vector<ALife::OBJECTID> >	m_tpGraphObjects;		// по точке графа получить все 
																//  динамические объекты
	vector<ALife::OBJECTID>				m_tpScheduledObjects;	// массив обновляемых объектов
	
	// static
	vector<ALife::SSpawnPoint>			m_tpSpawnPoints;		// массив spawn-point-ов

	// dynamic
	CALifeObjectRegistry				m_tObjectRegistry;		// карта объектов
	CALifeEventRegistry					m_tEventRegistry;		// карта событий
	CALifeTaskRegistry					m_tTaskRegistry;		// карта заданий

	IC void vfRemoveFromGraphPoint(ALife::OBJECTID tObjectID, ALife::GRAPHID tGraphID)
	{
		for (int i=0; i<(int)m_tpGraphObjects[tGraphID].size(); i++)
			if (m_tpGraphObjects[tGraphID][i] == tObjectID) {
				m_tpGraphObjects[tGraphID].erase(m_tpGraphObjects[m_tObjectRegistry.m_tppMap[tObjectID]->m_tGraphID].begin() + i);
				break;
			}
	}
	
	IC void vfAddToGraphPoint(ALife::OBJECTID tObjectID, ALife::GRAPHID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].push_back(tObjectID);
	}

	IC void vfChangeGraphPoint(ALife::OBJECTID tObjectID, ALife::GRAPHID tGraphPointID, ALife::GRAPHID tNextGraphPointID)
	{
		vfRemoveFromGraphPoint	(tObjectID,tGraphPointID);
		vfAddToGraphPoint		(tObjectID,tNextGraphPointID);
	}

	void								vfCheckForDeletedEvents	(ALife::OBJECTID tObjectID);
	void								vfCheckForTheBattle		(ALife::OBJECTID tObjectID);
	void								vfChooseNextRoutePoint	(ALife::OBJECTID tObjectID);
	void								vfProcessNPC			(ALife::OBJECTID tObjectID);
	void								vfInitTerrain			();
	void								vfInitLocationOwners	();
	void								vfInitGraph				();
	void								vfInitScheduledObjects	();
	// temporary
	void								vfGenerateSpawnPoints	(u32 dwSpawnCount);
	void								vfSaveSpawnPoints		();
	//
public:
										CAI_ALife				();
	virtual								~CAI_ALife				();
	virtual float						shedule_Scale			()					{return .5f;};
	virtual BOOL						Ready					()					{return TRUE;};
	virtual LPCSTR						cName					()					{return "ALife simulator";}; 
	virtual void						Load					();
	virtual void						Update					(u32 dt);	
			void						Save					();
			void						Generate				();
};