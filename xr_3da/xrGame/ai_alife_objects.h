////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace AI;
using namespace ALife;

class IPureALifeObject {
public:
	virtual void					Save(CFS_Memory	&tMemoryStream) = 0;
	virtual void					Load(CStream	&tFileStream)	= 0;
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints) = 0;
};

class CALifeMonsterParams : public IPureALifeObject {
public:
	int								m_iHealth;

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.Wdword		(m_iHealth);
	};
	
	virtual void					Load(CStream	&tFileStream)
	{
		m_iHealth					= tFileStream.Rdword();
	};
	
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		m_iHealth					= pSettings->ReadINT	(tpSpawnPoints[tSpawnID].caModel, "health");
	};
};

class CALifeTraderParams : public IPureALifeObject {
public:
	float							m_fCumulativeItemMass;
	u32								m_dwMoney;
	EStalkerRank					m_tRank;
	OBJECT_VECTOR					m_tpItemIDs;

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.Wfloat		(m_fCumulativeItemMass);
		tMemoryStream.Wdword		(m_dwMoney);
		tMemoryStream.Wdword		(m_tRank);
		tMemoryStream.Wdword		(m_tpItemIDs.size());
		OBJECT_IT					I = m_tpItemIDs.begin();
		OBJECT_IT					E = m_tpItemIDs.end();
		for ( ; I !=E; I++)
			tMemoryStream.write		(I,sizeof(*I));
	};
	
	virtual void					Load(CStream	&tFileStream)
	{
		m_fCumulativeItemMass		= tFileStream.Rfloat();
		m_dwMoney					= tFileStream.Rdword();
		m_tRank						= EStalkerRank(tFileStream.Rdword());
		m_tpItemIDs.resize			(tFileStream.Rdword());
		OBJECT_IT					I = m_tpItemIDs.begin();
		OBJECT_IT					E = m_tpItemIDs.end();
		for ( ; I !=E; I++)
			tFileStream.Read		(I,sizeof(*I));
	};
	
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		m_fCumulativeItemMass		= 0.0f;
		m_dwMoney					= 0;
		if (pSettings->LineExists(tpSpawnPoints[tSpawnID].caModel, "money"))
			m_dwMoney = pSettings->ReadINT(tpSpawnPoints[tSpawnID].caModel, "money");
		m_tRank						= EStalkerRank(pSettings->ReadINT	(tpSpawnPoints[tSpawnID].caModel, "rank"));
		m_tpItemIDs.clear			();
	};
};

class CALifeHumanParams : public CALifeMonsterParams, public CALifeTraderParams {
public:
	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		CALifeMonsterParams::Save	(tMemoryStream);
		CALifeTraderParams::Save	(tMemoryStream);
	};
	
	virtual void					Load(CStream	&tFileStream)
	{
		CALifeMonsterParams::Load	(tFileStream);
		CALifeTraderParams::Load	(tFileStream);
	};
	
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		CALifeMonsterParams::Init	(tSpawnID,tpSpawnPoints);
		CALifeTraderParams::Init	(tSpawnID,tpSpawnPoints);
	};
};

class CALifeTraderAbstract : public IPureALifeObject {
public:
	PERSONAL_EVENT_VECTOR			m_tpEvents;
	TASK_VECTOR						m_tpTaskIDs;
	float							m_fMaxItemMass;
	
	virtual	void					Save(CFS_Memory	&tMemoryStream)
	{
		// saving events
		{
			tMemoryStream.Wdword	(m_tpEvents.size());
			PERSONAL_EVENT_IT 		I = m_tpEvents.begin();
			PERSONAL_EVENT_IT 		E = m_tpEvents.end();
			for ( ; I != E; I++) {
				SPersonalEvent		&tPersonalEvent = *I;
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
			tMemoryStream.Wdword	(m_tpTaskIDs.size());
			TASK_IT 				I = m_tpTaskIDs.begin();
			TASK_IT 				E = m_tpTaskIDs.end();
			for ( ; I != E; I++)
				tMemoryStream.write	(I,sizeof(*I));
		}
		tMemoryStream.Wfloat		(m_fMaxItemMass);
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		// loading events
		{
			m_tpEvents.resize		(tFileStream.Rdword());
			PERSONAL_EVENT_IT 		I = m_tpEvents.begin();
			PERSONAL_EVENT_IT 		E = m_tpEvents.end();
			for ( ; I != E; I++) {
				SPersonalEvent		&tPersonalEvent = *I;
				tFileStream.Read	(&tPersonalEvent.tEventID,			sizeof(tPersonalEvent.tEventID));
				tFileStream.Read	(&tPersonalEvent.tGameTime,			sizeof(tPersonalEvent.tGameTime));
				tFileStream.Read	(&tPersonalEvent.tTaskID,			sizeof(tPersonalEvent.tTaskID));
				tPersonalEvent.tpItemIDs.resize(tFileStream.Rdword());
				for (int j=0; j<(int)tPersonalEvent.tpItemIDs.size(); j++)
					tFileStream.Read(&(tPersonalEvent.tpItemIDs[j]),	sizeof(tPersonalEvent.tpItemIDs[j]));
				tFileStream.Read	(&tPersonalEvent.iHealth,			sizeof(tPersonalEvent.iHealth));
				tFileStream.Read	(&tPersonalEvent.tRelation,			sizeof(tPersonalEvent.tRelation));
			}
		}
		// loading tasks
		{
			m_tpTaskIDs.resize		(tFileStream.Rdword());
			TASK_IT I				= m_tpTaskIDs.begin();
			TASK_IT E				= m_tpTaskIDs.end();
			for ( ; I != E; I++)
				tFileStream.Read	(I,sizeof(*I));
		}
		m_fMaxItemMass				= tFileStream.Rfloat();
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		m_tpEvents.clear			();
		m_tpTaskIDs.clear			();
		m_fMaxItemMass				= pSettings->ReadFLOAT(tpSpawnPoints[tSpawnID].caModel, "max_item_mass");
	};
};

class CALifeObject : public IPureALifeObject {
public:
	_CLASS_ID						m_tClassID;
	_OBJECT_ID						m_tObjectID;
	_SPAWN_ID						m_tSpawnID;
	_GRAPH_ID						m_tGraphID;
	u16								m_wCount;
	bool							m_bOnline;
	
	virtual	void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tClassID,	sizeof(m_tClassID));
		tMemoryStream.write			(&m_tObjectID,	sizeof(m_tObjectID));
		tMemoryStream.write			(&m_tSpawnID,	sizeof(m_tSpawnID));
		tMemoryStream.write			(&m_tGraphID,	sizeof(m_tGraphID));
		tMemoryStream.write			(&m_wCount,		sizeof(m_wCount));
		tMemoryStream.write			(&m_bOnline,	sizeof(m_bOnline));
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tClassID,	sizeof(m_tClassID));
		tFileStream.Read			(&m_tObjectID,	sizeof(m_tObjectID));
		tFileStream.Read			(&m_tSpawnID,	sizeof(m_tSpawnID));
		tFileStream.Read			(&m_tGraphID,	sizeof(m_tGraphID));
		tFileStream.Read			(&m_wCount,		sizeof(m_wCount));
		tFileStream.Read			(&m_bOnline,	sizeof(m_bOnline));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		LPCSTR						S = pSettings->ReadSTRING(tpSpawnPoints[tSpawnID].caModel, "class");
		PSGP.memCopy				(&m_tClassID,S,sizeof(m_tClassID));
		m_tGraphID					= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tSpawnID					= tSpawnID;
		m_wCount					= tpSpawnPoints[tSpawnID].wCount;
		m_bOnline					= false;
	};
};

class CALifeEventGroup : public CALifeObject {
public:
	typedef	CALifeObject inherited;
	u16								m_wCountAfter;
	
	virtual	void					Save(CFS_Memory	&tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		tMemoryStream.write			(&m_wCountAfter,sizeof(m_wCountAfter));
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		tFileStream.Read			(&m_wCountAfter,sizeof(m_wCountAfter));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_wCountAfter				= m_wCount;
	};
};

class CALifeDynamicObject : public CALifeObject {
public:
	typedef	CALifeObject inherited;
	
	_TIME_ID						m_tTimeID;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		tMemoryStream.write			(&m_tTimeID,	sizeof(m_tTimeID));
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		tFileStream.Read			(&m_tTimeID,	sizeof(m_tTimeID));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tTimeID					= 0;
	};
};

class CALifeItem : public CALifeDynamicObject {
public:
	typedef	CALifeDynamicObject inherited;
	
	float							m_fMass;
	u32								m_dwCost;
	s32								m_iHealthValue;
	bool							m_bAttached;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		tMemoryStream.Wfloat		(m_fMass);
		tMemoryStream.Wdword		(m_dwCost);
		tMemoryStream.Wdword		(m_iHealthValue);
		tMemoryStream.Wdword		(m_bAttached);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_fMass						= tFileStream.Rfloat();
		m_dwCost					= tFileStream.Rdword();
		m_iHealthValue				= tFileStream.Rint();
		m_bAttached					= !!(tFileStream.Rdword());
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_fMass						= pSettings->ReadFLOAT(tpSpawnPoints[tSpawnID].caModel, "ph_mass");
		m_dwCost					= pSettings->ReadINT(tpSpawnPoints[tSpawnID].caModel, "cost");
		if (pSettings->LineExists	(tpSpawnPoints[tSpawnID].caModel, "health_value"))
			m_iHealthValue			= pSettings->ReadINT(tpSpawnPoints[tSpawnID].caModel, "health_value");
		else
			m_iHealthValue			= 0;
		m_bAttached					= false;
	};
};

class CALifeTrader : public CALifeDynamicObject, public CALifeTraderParams, public CALifeTraderAbstract {
public:
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		CALifeDynamicObject::Save	(tMemoryStream);
		CALifeTraderParams::Save	(tMemoryStream);
		CALifeTraderAbstract::Save	(tMemoryStream);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		CALifeDynamicObject::Load	(tFileStream);
		CALifeTraderParams::Load	(tFileStream);
		CALifeTraderAbstract::Load	(tFileStream);
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		CALifeDynamicObject::Init	(tSpawnID,tpSpawnPoints);
		CALifeTraderParams::Init	(tSpawnID,tpSpawnPoints);
		CALifeTraderAbstract::Init	(tSpawnID,tpSpawnPoints);
	};
};

class CALifeMonsterAbstract : public CALifeDynamicObject {
public:
	typedef	CALifeDynamicObject inherited;
	
	_GRAPH_ID						m_tNextGraphID;
	_GRAPH_ID						m_tPrevGraphID;
	float							m_fGoingSpeed;
	float							m_fCurSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		tMemoryStream.write			(&m_tNextGraphID,			sizeof(m_tNextGraphID));
		tMemoryStream.write			(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
		tMemoryStream.write			(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
		tMemoryStream.write			(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tMemoryStream.write			(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tMemoryStream.write			(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		tFileStream.Read			(&m_tNextGraphID,			sizeof(m_tNextGraphID));
		tFileStream.Read			(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
		tFileStream.Read			(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
		tFileStream.Read			(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tFileStream.Read			(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tFileStream.Read			(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tNextGraphID				= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tPrevGraphID				= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_fGoingSpeed				= pSettings->ReadFLOAT	(tpSpawnPoints[tSpawnID].caModel, "going_speed");
		m_fCurSpeed					= 0.0f;
		m_fDistanceFromPoint		= 0.0f;
		m_fDistanceToPoint			= 0.0f;
	};
};

class CALifeAnomalousZone : public CALifeMonsterAbstract {
public:
	typedef	CALifeMonsterAbstract inherited;
	
	SAnomalousZone					m_tAnomalousZone;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		tMemoryStream.write			(&m_tAnomalousZone, sizeof(m_tAnomalousZone));
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		tFileStream.Read			(&m_tAnomalousZone, sizeof(m_tAnomalousZone));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tAnomalousZone.tAnomalousZone	= eAnomalousZoneTypeDummy;
	};
};

class CALifeMonster : public CALifeMonsterAbstract, public CALifeMonsterParams {
public:
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		CALifeMonsterAbstract::Save	(tMemoryStream);
		CALifeMonsterParams::Save	(tMemoryStream);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		CALifeMonsterAbstract::Load	(tFileStream);
		CALifeMonsterParams::Load	(tFileStream);
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		CALifeMonsterAbstract::Init	(tSpawnID,tpSpawnPoints);
		CALifeMonsterParams::Init	(tSpawnID,tpSpawnPoints);
	};
};

class CALifeMonsterGroup : public CALifeMonsterAbstract {
public:
	typedef	CALifeMonsterAbstract inherited;
	
	MONSTER_PARAMS_VECTOR			m_tpMembers;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		tMemoryStream.Wdword		(m_tpMembers.size());
		MONSTER_PARAMS_IT 			I = m_tpMembers.begin();
		MONSTER_PARAMS_IT 			E = m_tpMembers.end();
		for ( ; I != E; I++)
			(*I).Save				(tMemoryStream);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_tpMembers.resize			(tFileStream.Rdword());
		MONSTER_PARAMS_IT 			I = m_tpMembers.begin();
		MONSTER_PARAMS_IT 			E = m_tpMembers.end();
		for ( ; I != E; I++)
			(*I).Load				(tFileStream);
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tpMembers.resize			(m_wCount);
		MONSTER_PARAMS_IT 			I = m_tpMembers.begin();
		MONSTER_PARAMS_IT 			E = m_tpMembers.end();
		for ( ; I != E; I++)
			(*I).Init				(tSpawnID,tpSpawnPoints);
	};
};

class CALifeHumanAbstract : public CALifeMonsterAbstract, public CALifeTraderAbstract {
public:
	DWORD_VECTOR					m_tpaVertices;
	BOOL_VECTOR						m_baVisitedVertices;
	PERSONAL_TASK_VECTOR			m_tpTasks;
	ETaskState						m_tTaskState;
	u32								m_dwCurNode;
	u32								m_dwCurTaskLocation;
	STask							m_tCurTask;
	float							m_fSearchSpeed;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		// calling inherited
		CALifeMonsterAbstract::Save	(tMemoryStream);
		CALifeTraderAbstract::Save	(tMemoryStream);
		{
			tMemoryStream.Wdword	(m_tpaVertices.size());
			DWORD_IT 				I = m_tpaVertices.begin();
			DWORD_IT 				E = m_tpaVertices.end();
			for ( ; I != E; I++)
				tMemoryStream.write	(I,sizeof(*I));
		}
		{
			tMemoryStream.Wdword	(m_baVisitedVertices.size());
			BOOL_IT 				I = m_baVisitedVertices.begin();
			BOOL_IT 				E = m_baVisitedVertices.end();
			u32						dwMask = 0;
			if (I != E) {
				for (int j=0; I != E; I++, j++) {
					if (j >= 32) {
						tMemoryStream.Wdword	(dwMask);
						dwMask = 0;
						j = 0;
					}
					if (*I)
						dwMask |= u32(1) << j;
				}
				tMemoryStream.Wdword	(dwMask);
			}
		}
		{
			tMemoryStream.Wdword	(m_tpTasks.size());
			PERSONAL_TASK_IT 		I = m_tpTasks.begin();
			PERSONAL_TASK_IT 		E = m_tpTasks.end();
			for ( ; I != E; I++)
				tMemoryStream.write	(I,sizeof(*I));
		}
		tMemoryStream.Wdword		(m_tTaskState);
		tMemoryStream.Wdword		(m_dwCurNode);
		tMemoryStream.Wdword		(m_dwCurTaskLocation);
		tMemoryStream.write			(&m_tCurTask,	sizeof(m_tCurTask));
		tMemoryStream.Wfloat		(m_fSearchSpeed);
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		// calling inherited
		CALifeMonsterAbstract::Load	(tFileStream);
		CALifeTraderAbstract::Load	(tFileStream);
		{
			m_tpaVertices.resize	(tFileStream.Rdword());
			DWORD_IT 				I = m_tpaVertices.begin();
			DWORD_IT 				E = m_tpaVertices.end();
			for ( ; I != E; I++)
				tFileStream.Read	(I,sizeof(*I));
		}
		{
			m_baVisitedVertices.resize	(tFileStream.Rdword());
			BOOL_IT 				I = m_baVisitedVertices.begin();
			BOOL_IT 				E = m_baVisitedVertices.end();
			u32						dwMask = 0;
			for (int j=32; I != E; I++, j++) {
				if (j >= 32) {
					dwMask = tFileStream.Rdword();
					j = 0;
				}
				*I = !!(dwMask & (u32(1) << j));
			}
		}
		{
			m_tpTasks.resize		(tFileStream.Rdword());
			PERSONAL_TASK_IT 		I = m_tpTasks.begin();
			PERSONAL_TASK_IT 		E = m_tpTasks.end();
			for ( ; I != E; I++)
				tFileStream.Read	(I,sizeof(*I));
		}
		m_tTaskState				= ETaskState(tFileStream.Rdword());
		m_dwCurNode					= tFileStream.Rdword();
		m_dwCurTaskLocation			= tFileStream.Rdword();
		tFileStream.Read			(&m_tCurTask,	sizeof(m_tCurTask));
		m_fSearchSpeed				= tFileStream.Rfloat();
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		CALifeMonsterAbstract::Init	(tSpawnID,tpSpawnPoints);
		CALifeTraderAbstract::Init	(tSpawnID,tpSpawnPoints);
		m_tpaVertices.clear			();
		m_baVisitedVertices.clear	();
		m_tpTasks.clear				();
		m_tTaskState				= eTaskStateNoTask;
		memset						(&m_tCurTask,0,sizeof(m_tCurTask));
		m_tCurTask.tTaskID			= u32(-1);
		m_dwCurNode					= u32(-1);
		m_dwCurTaskLocation			= u32(-1);
		m_fSearchSpeed				= pSettings->ReadFLOAT				(tpSpawnPoints[tSpawnID].caModel, "search_speed");
	};
};

class CALifeHuman : public CALifeHumanAbstract, public CALifeHumanParams {
public:
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		CALifeHumanAbstract::Save	(tMemoryStream);
		CALifeHumanParams::Save		(tMemoryStream);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		CALifeHumanAbstract::Load	(tFileStream);
		CALifeHumanParams::Load		(tFileStream);
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		CALifeHumanAbstract::Init	(tSpawnID,tpSpawnPoints);
		CALifeHumanParams::Init		(tSpawnID,tpSpawnPoints);
	};
};

class CALifeHumanGroup : public CALifeHumanAbstract {
public:
	typedef	CALifeHumanAbstract inherited;

	HUMAN_PARAMS_VECTOR				m_tpMembers;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		tMemoryStream.Wdword		(m_tpMembers.size());
		HUMAN_PARAMS_IT				I = m_tpMembers.begin();
		HUMAN_PARAMS_IT				E = m_tpMembers.end();
		for ( ; I != E; I++)
			(*I).Save				(tMemoryStream);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_tpMembers.resize			(tFileStream.Rdword());
		HUMAN_PARAMS_IT				I = m_tpMembers.begin();
		HUMAN_PARAMS_IT				E = m_tpMembers.end();
		for ( ; I != E; I++)
			(*I).Load				(tFileStream);
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tpMembers.resize			(m_wCount);
		HUMAN_PARAMS_IT				I = m_tpMembers.begin();
		HUMAN_PARAMS_IT				E = m_tpMembers.end();
		for ( ; I != E; I++)
			(*I).Init				(tSpawnID,tpSpawnPoints);
	};
};

class CRemovePersonalEventPredicate {
private:
	EVENT_MAP						*m_tpMap;
public:
	CRemovePersonalEventPredicate	(EVENT_MAP &tpMap)
	{
		m_tpMap = &tpMap;
	};

	IC bool operator()(const SPersonalEvent &tPersonalEvent)  const
	{
		return(m_tpMap->find(tPersonalEvent.tEventID) == m_tpMap->end());
	};
};

class CSortItemPredicate {
private:
	OBJECT_MAP						*m_tpMap;
public:
	CSortItemPredicate				(OBJECT_MAP &tpMap)
	{
		m_tpMap = &tpMap;
	};

	IC bool operator()(const _OBJECT_ID &tObjectID1, const _OBJECT_ID &tObjectID2)  const
	{
		OBJECT_PAIR_IT it1 = m_tpMap->find(tObjectID1);
		VERIFY(it1 != m_tpMap->end());
		OBJECT_PAIR_IT it2 = m_tpMap->find(tObjectID2);
		VERIFY(it2 != m_tpMap->end());
		CALifeItem *tpItem1 = dynamic_cast<CALifeItem *>((*it1).second);
		VERIFY(tpItem1);
		CALifeItem *tpItem2 = dynamic_cast<CALifeItem *>((*it2).second);
		VERIFY(tpItem2);
		return(float(tpItem1->m_dwCost)/tpItem1->m_fMass > float(tpItem2->m_dwCost)/tpItem2->m_fMass);
	};
};
