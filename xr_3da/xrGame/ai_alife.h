////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
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
		vector<_OBJECT_ID>			tpItemIDs;
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
		vector<_GRAPH_ID>			wpRouteGraphPoints;
	} SSpawnPoint;

	typedef struct tagSALifeHeader {
		u32							dwVersion;
		_TIME_ID					tTimeID;
	} SALifeHeader;
};

using namespace ALife;

class CALifeObject {
public:
	virtual void					Save(CFS_Memory	&tMemoryStream) = 0;
	virtual void					Load(CStream	&tFileStream)	= 0;
	virtual void					Init(_SPAWN_ID	tSpawnID, vector<SSpawnPoint> &tpSpawnPoints) = 0;
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

	virtual void					Init(_SPAWN_ID	tSpawnID, vector<SSpawnPoint> &tpSpawnPoints)
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

	virtual void					Init(_SPAWN_ID	tSpawnID, vector<SSpawnPoint> &tpSpawnPoints)
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

	virtual void					Init(_SPAWN_ID	tSpawnID, vector<SSpawnPoint> &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tInjureType = eInjureTypeDummy;
	}
};

class CALifeMonster : public CALifeCorp {
public:
	typedef	CALifeCorp inherited;
	
	_GRAPH_ID						m_tCurrentGraphPointID;
	_GRAPH_ID						m_tNextGraphPointID;
	_GRAPH_ID						m_tPrevGraphPointID;
	float							m_fCurSpeed;
	float							m_fMinSpeed;
	float							m_fMaxSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	s32								m_iHealth;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
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
	
	virtual	void					Load(CStream	&tFileStream)
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

	virtual void					Init(_SPAWN_ID	tSpawnID, vector<SSpawnPoint> &tpSpawnPoints)
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
	
	vector<_OBJECT_ID>				m_tpItemIDs;
	vector<SPersonalEvent>			m_tpEvents;
	vector<_TASK_ID>				m_tpTaskIDs;

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

	virtual void					Init(_SPAWN_ID	tSpawnID, vector<SSpawnPoint> &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tCurrentGraphPointID	= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tNextGraphPointID		= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tPrevGraphPointID		= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
	}
};

class CALifeObjectRegistry {
public:
	_OBJECT_ID						m_tObjectID;			// ������������� ����� �������
	map<_OBJECT_ID,CALifeItem *>	m_tppMap;				// ������ ������� ����
	map<_OBJECT_ID,CALifeItem *>::iterator	m_tpIterator;

	virtual							~CALifeObjectRegistry()
	{
		for (m_tpIterator = m_tppMap.begin(); m_tpIterator != m_tppMap.end(); m_tpIterator++)
			_DELETE((*m_tpIterator).second);
		m_tppMap.clear();
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.write(&m_tObjectID,sizeof(m_tObjectID));
		tMemoryStream.Wdword(m_tppMap.size());
		for (m_tpIterator = m_tppMap.begin(); m_tpIterator != m_tppMap.end(); m_tpIterator++) {
			CALifeHuman	*tpALifeHuman = dynamic_cast<CALifeHuman *>((*m_tpIterator).second);
			if (tpALifeHuman)
				tMemoryStream.Wbyte(2);
			else {
				CALifeMonster	*tpALifeMonster = dynamic_cast<CALifeMonster *>((*m_tpIterator).second);
				if (tpALifeMonster)
					tMemoryStream.Wbyte(1);
				else
					tMemoryStream.Wbyte(0);
			}
			(*m_tpIterator).second->Save(tMemoryStream);
		}
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read(&m_tObjectID,sizeof(m_tObjectID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			CALifeItem *tpALifeItem = 0;
			switch (tFileStream.Rbyte()) {
				case 0 : {
					tpALifeItem = new CALifeItem;
					break;
				}
				case 1 : {
					tpALifeItem = new CALifeMonster;
					break;
				}
				case 2 : {
					tpALifeItem = new CALifeHuman;
					break;
				}
				default : NODEFAULT;
			};
			tpALifeItem->Load(tFileStream);
			m_tppMap.insert			(make_pair(tpALifeItem->m_tObjectID,tpALifeItem));
		}
	};

	virtual	void					Add	(CALifeItem *tpALifeItem)
	{
		m_tppMap.insert				(make_pair(tpALifeItem->m_tObjectID = m_tObjectID++,tpALifeItem));
	};

};

class CALifeEventRegistry {
public:
	_EVENT_ID						m_tEventID;				// ������������� ����� �������
	map<_EVENT_ID,SEvent>			m_tpMap;				// ������ ������� ����
	map<_EVENT_ID,SEvent>::iterator	m_tpIterator;

	virtual							~CALifeEventRegistry()
	{
		for (m_tpIterator = m_tpMap.begin(); m_tpIterator != m_tpMap.end(); m_tpIterator++) {
			_DELETE((*m_tpIterator).second.tpMonsterGroup1);
			_DELETE((*m_tpIterator).second.tpMonsterGroup2);
		}
		m_tpMap.clear();
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.write			(&m_tEventID,sizeof(m_tEventID));
		tMemoryStream.Wdword		(m_tpMap.size());
		for (m_tpIterator = m_tpMap.begin(); m_tpIterator != m_tpMap.end(); m_tpIterator++) {
			SEvent					&tEvent = (*m_tpIterator).second;
			tMemoryStream.write		(&tEvent.tEventID,		sizeof(tEvent.tEventID		));
			tMemoryStream.write		(&tEvent.tTimeID,		sizeof(tEvent.tTimeID		));
			tMemoryStream.write		(&tEvent.tGraphID,		sizeof(tEvent.tGraphID		));
			tMemoryStream.write		(&tEvent.tBattleResult,	sizeof(tEvent.tBattleResult	));
			tMemoryStream.Wbyte		(tEvent.ucMonster1CountAfter);
			tMemoryStream.Wbyte		(tEvent.ucMonster2CountAfter);
			tEvent.tpMonsterGroup1->Save(tMemoryStream);
			tEvent.tpMonsterGroup2->Save(tMemoryStream);
		}
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read(&m_tEventID,sizeof(m_tEventID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			SEvent					tEvent;
			tFileStream.Read		(&tEvent.tEventID,		sizeof(tEvent.tEventID		));
			tFileStream.Read		(&tEvent.tTimeID,		sizeof(tEvent.tTimeID		));
			tFileStream.Read		(&tEvent.tGraphID,		sizeof(tEvent.tGraphID		));
			tFileStream.Read		(&tEvent.tBattleResult,	sizeof(tEvent.tBattleResult	));
			tEvent.ucMonster1CountAfter	= tFileStream.Rbyte();
			tEvent.ucMonster2CountAfter	= tFileStream.Rbyte();
			tEvent.tpMonsterGroup1	= new CALifeMonsterGroup;
			tEvent.tpMonsterGroup2	= new CALifeMonsterGroup;
			tEvent.tpMonsterGroup1->Load(tFileStream);
			tEvent.tpMonsterGroup2->Load(tFileStream);
			m_tpMap.insert			(make_pair(tEvent.tEventID,tEvent));
		}
	};
	
	virtual	void					Add	(SEvent	&tEvent)
	{
		m_tpMap.insert				(make_pair(tEvent.tEventID = m_tEventID++,tEvent));
	};
};

class CALifeTaskRegistry {
public:
	_TASK_ID						m_tTaskID;				// ������������� ����� �������
	map<_TASK_ID,STask>				m_tpMap;				// ������ ������� ����
	map<_TASK_ID,STask>::iterator	m_tpIterator;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.write			(&m_tTaskID,sizeof(m_tTaskID));
		tMemoryStream.Wdword		(m_tpMap.size());
		for (m_tpIterator = m_tpMap.begin(); m_tpIterator != m_tpMap.end(); m_tpIterator++) {
			STask					&tTask = (*m_tpIterator).second;
			tMemoryStream.write		(&tTask.tTaskID,		sizeof(tTask.tTaskID	));
			tMemoryStream.write		(&tTask.tTimeID,		sizeof(tTask.tTimeID	));
			tMemoryStream.write		(&tTask.tGraphID,		sizeof(tTask.tGraphID	));
			tMemoryStream.write		(&tTask.tCustomerID,	sizeof(tTask.tCustomerID));
			tMemoryStream.write		(&tTask.tTaskType,		sizeof(tTask.tTaskType	));
		}
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read(&m_tTaskID,sizeof(m_tTaskID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			STask					tTask;
			tFileStream.Read		(&tTask.tTaskID,		sizeof(tTask.tTaskID	));
			tFileStream.Read		(&tTask.tTimeID,		sizeof(tTask.tTimeID	));
			tFileStream.Read		(&tTask.tGraphID,		sizeof(tTask.tGraphID	));
			tFileStream.Read		(&tTask.tCustomerID,	sizeof(tTask.tCustomerID));
			tFileStream.Read		(&tTask.tTaskType,		sizeof(tTask.tTaskType	));
			m_tpMap.insert			(make_pair(tTask.tTaskID,tTask));
		}
	};
	
	virtual	void					Add	(STask	&tTask)
	{
		m_tpMap.insert				(make_pair(tTask.tTaskID = m_tTaskID++,tTask));
	};
};

class CAI_ALife : public CSheduled {
public:
	typedef CSheduled inherited;

	u32								m_dwObjectsBeingProcessed;
	u64								m_qwMaxProcessTime;
	bool							m_bLoaded;
	
	SSpawnHeader					m_tSpawnHeader;
	SALifeHeader					m_tALifeHeader;

	// auto
	svector<vector<_GRAPH_ID>,256>	m_tpTerrain;			// ������ �������: �� �������������� 
															//	��������� �������� ������ ����� 
															//  �����
	vector<vector<_OBJECT_ID> >		m_tpLocationOwners;		// ������ ������� : �� ����� ����� 
															//  �������� ������ � ����������
	vector<vector<_OBJECT_ID> >		m_tpGraphObjects;		// �� ����� ����� �������� ��� 
															//  ������������ �������
	vector<_OBJECT_ID>				m_tpScheduledObjects;	// ������ ����������� ��������
	
	// static
	vector<SSpawnPoint>				m_tpSpawnPoints;		// ������ spawn-point-��

	// dynamic
	CALifeObjectRegistry			m_tObjectRegistry;		// ����� ��������
	CALifeEventRegistry				m_tEventRegistry;		// ����� �������
	CALifeTaskRegistry				m_tTaskRegistry;		// ����� �������

	IC void vfRemoveFromGraphPoint(_OBJECT_ID tObjectID, _GRAPH_ID tGraphID)
	{
		for (int i=0; i<(int)m_tpGraphObjects[tGraphID].size(); i++)
			if (m_tpGraphObjects[tGraphID][i] == tObjectID) {
				m_tpGraphObjects[tGraphID].erase(m_tpGraphObjects[m_tObjectRegistry.m_tppMap[tObjectID]->m_tGraphID].begin() + i);
				break;
			}
	}
	
	IC void vfAddToGraphPoint(_OBJECT_ID tObjectID, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].push_back(tObjectID);
	}

	IC void vfChangeGraphPoint(_OBJECT_ID tObjectID, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveFromGraphPoint	(tObjectID,tGraphPointID);
		vfAddToGraphPoint		(tObjectID,tNextGraphPointID);
	}

	void							vfCheckForDeletedEvents	(_OBJECT_ID tObjectID);
	void							vfCheckForTheBattle		(_OBJECT_ID tObjectID);
	void							vfChooseNextRoutePoint	(_OBJECT_ID tObjectID);
	void							vfProcessNPC			(_OBJECT_ID tObjectID);
	void							vfInitTerrain			();
	void							vfInitLocationOwners	();
	void							vfInitGraph				();
	void							vfInitScheduledObjects	();
	// temporary
	void							vfGenerateSpawnPoints	(u32 dwSpawnCount);
	void							vfSaveSpawnPoints		();
	//
public:
									CAI_ALife				();
	virtual							~CAI_ALife				();
	virtual float					shedule_Scale			()					{return .5f;};
	virtual BOOL					Ready					()					{return TRUE;};
	virtual LPCSTR					cName					()					{return "ALife simulator";}; 
	virtual void					Load					();
	virtual void					Update					(u32 dt);	
			void					Save					();
			void					Generate				();
};