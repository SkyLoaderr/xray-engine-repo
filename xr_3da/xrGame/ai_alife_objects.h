////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#pragma once

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
	float							m_fMass;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save		(tMemoryStream);
		tMemoryStream.write	(&m_tTimeID,	sizeof(m_tTimeID));
		tMemoryStream.Wfloat(m_fMass);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load		(tFileStream);
		tFileStream.Read	(&m_tTimeID,	sizeof(m_tTimeID));
		m_fMass				= tFileStream.Rfloat();
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init		(tSpawnID,tpSpawnPoints);
		m_tTimeID			= 0;
		m_fMass				= pSettings->ReadFLOAT(tpSpawnPoints[tSpawnID].caModel, "ph_mass");
	}
};

class CALifeCorp : public CALifeItem {
public:
	typedef	CALifeItem inherited;

	EInjureType						m_tInjureType;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save		(tMemoryStream);
		tMemoryStream.write	(&m_tInjureType,sizeof(m_tInjureType));
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load		(tFileStream);
		tFileStream.Read	(&m_tInjureType,sizeof(m_tInjureType));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tInjureType		= eInjureTypeDummy;
	}
};

class CALifeMonster : public CALifeItem {
public:
	typedef	CALifeItem inherited;
	
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
	float							m_fItemMass;
	float							m_fMaxItemMass;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		// calling inherited
		inherited::Save(tMemoryStream);
		// saving items
		{
			tMemoryStream.Wdword	(m_tpItemIDs.size());
			OBJECT_IT it			= m_tpItemIDs.begin();
			OBJECT_IT E				= m_tpItemIDs.end();
			for ( ; it != E; it++)
				tMemoryStream.write	(it,sizeof(*it));
		}
		// saving events
		{
			tMemoryStream.Wdword(m_tpEvents.size());
			PERSONAL_EVENT_IT it	= m_tpEvents.begin();
			PERSONAL_EVENT_IT E		= m_tpEvents.end();
			for ( ; it != E; it++) {
				SPersonalEvent &tPersonalEvent = *it;
				tMemoryStream.write	(&tPersonalEvent.tEventID,			sizeof(tPersonalEvent.tEventID));
				tMemoryStream.write	(&tPersonalEvent.tGameTime,			sizeof(tPersonalEvent.tGameTime));
				tMemoryStream.write	(&tPersonalEvent.tTaskID,			sizeof(tPersonalEvent.tTaskID));
				tMemoryStream.Wdword(tPersonalEvent.tpItemIDs.size());
				for (int j=0; j<(int)tPersonalEvent.tpItemIDs.size(); j++)
					tMemoryStream.write	(&(tPersonalEvent.tpItemIDs[j]),sizeof(tPersonalEvent.tpItemIDs[j]));
				tMemoryStream.write	(&tPersonalEvent.iHealth,			sizeof(tPersonalEvent.iHealth));
				tMemoryStream.write	(&tPersonalEvent.tRelation,		sizeof(tPersonalEvent.tRelation));
			}
		}
		// saving tasks
		{
			tMemoryStream.Wdword(m_tpTaskIDs.size());
			TASK_IT it				= m_tpTaskIDs.begin();
			TASK_IT E				= m_tpTaskIDs.end();
			for ( ; it != E; it++)
				tMemoryStream.write	(it,sizeof(*it));
		}
		tMemoryStream.Wfloat		(m_fItemMass);
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		// calling inherited
		inherited::Load(tFileStream);
		// loading items
		{
			m_tpItemIDs.resize(tFileStream.Rdword());
			OBJECT_IT it			= m_tpItemIDs.begin();
			OBJECT_IT E				= m_tpItemIDs.end();
			for ( ; it != E; it++)
				tFileStream.Read	(it,sizeof(*it));
		}
		// loading events
		{
			m_tpEvents.resize(tFileStream.Rdword());
			PERSONAL_EVENT_IT it	= m_tpEvents.begin();
			PERSONAL_EVENT_IT E		= m_tpEvents.end();
			for ( ; it != E; it++) {
				SPersonalEvent &tPersonalEvent = *it;
				tFileStream.Read	(&tPersonalEvent.tEventID,			sizeof(tPersonalEvent.tEventID));
				tFileStream.Read	(&tPersonalEvent.tGameTime,			sizeof(tPersonalEvent.tGameTime));
				tFileStream.Read	(&tPersonalEvent.tTaskID,			sizeof(tPersonalEvent.tTaskID));
				tPersonalEvent.tpItemIDs.resize(tFileStream.Rdword());
				for (int j=0; j<(int)tPersonalEvent.tpItemIDs.size(); j++)
					tFileStream.Read(&(tPersonalEvent.tpItemIDs[j]),	sizeof(tPersonalEvent.tpItemIDs[j]));
				tFileStream.Read	(&tPersonalEvent.iHealth,			sizeof(tPersonalEvent.iHealth));
				tFileStream.Read	(&tPersonalEvent.tRelation,		sizeof(tPersonalEvent.tRelation));
			}
		}
		// loading tasks
		{
			m_tpTaskIDs.resize(tFileStream.Rdword());
			TASK_IT it				= m_tpTaskIDs.begin();
			TASK_IT E				= m_tpTaskIDs.end();
			for ( ; it != E; it++)
				tFileStream.Read	(it,sizeof(*it));
		}
		m_fItemMass					= tFileStream.Rfloat();
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init(tSpawnID,tpSpawnPoints);
		m_tpEvents.	clear			();
		m_tpItemIDs.clear			();
		m_tpTaskIDs.clear			();
		m_fItemMass					= 0.0f;
		m_fMaxItemMass				= pSettings->ReadFLOAT(tpSpawnPoints[tSpawnID].caModel, "MaxItemMass");
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
