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

class CALifeMonsterParams {
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

class CALifeHumanParams : public CALifeMonsterParams{
public:
	typedef CALifeMonsterParams inherited;

	float							m_fCumulativeItemMass;
	u32								m_dwMoney;
	EStalkerRank					m_tRank;
	OBJECT_VECTOR					m_tpItemIDs;

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
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
		inherited::Load				(tFileStream);
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
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_fCumulativeItemMass		= 0.0f;
		m_dwMoney					= 0;
		if (pSettings->LineExists(tpSpawnPoints[tSpawnID].caModel, "money"))
			m_dwMoney = pSettings->ReadINT(tpSpawnPoints[tSpawnID].caModel, "money");
		m_tRank						= EStalkerRank(pSettings->ReadINT	(tpSpawnPoints[tSpawnID].caModel, "rank"));
		m_tpItemIDs.clear			();
	};
};

class CALifeObject : public IPureALifeObject {
public:
	_CLASS_ID						m_tClassID;
	_OBJECT_ID						m_tObjectID;
	_SPAWN_ID						m_tSpawnID;
	_GRAPH_ID						m_tGraphID;
	u16								m_wCount;
	
	virtual	void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tClassID,	sizeof(m_tClassID));
		tMemoryStream.write			(&m_tObjectID,	sizeof(m_tObjectID));
		tMemoryStream.write			(&m_tSpawnID,	sizeof(m_tSpawnID));
		tMemoryStream.write			(&m_tGraphID,	sizeof(m_tGraphID));
		tMemoryStream.write			(&m_wCount,		sizeof(m_wCount));
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tClassID,	sizeof(m_tClassID));
		tFileStream.Read			(&m_tObjectID,	sizeof(m_tObjectID));
		tFileStream.Read			(&m_tSpawnID,	sizeof(m_tSpawnID));
		tFileStream.Read			(&m_tGraphID,	sizeof(m_tGraphID));
		tFileStream.Read			(&m_wCount,		sizeof(m_wCount));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		LPCSTR						S = pSettings->ReadSTRING(tpSpawnPoints[tSpawnID].caModel, "class");
		memcpy						(&m_tClassID,S,sizeof(m_tClassID));
		m_tGraphID					= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tSpawnID					= tSpawnID;
		m_wCount					= tpSpawnPoints[tSpawnID].wCount;
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

class CALifeMonsterAbstract : public CALifeDynamicObject {
public:
	typedef	CALifeDynamicObject inherited;
	
	_GRAPH_ID						m_tNextGraphID;
	_GRAPH_ID						m_tPrevGraphID;
	float							m_fCurSpeed;
	float							m_fMinSpeed;
	float							m_fMaxSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		tMemoryStream.write			(&m_tNextGraphID,			sizeof(m_tNextGraphID));
		tMemoryStream.write			(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
		tMemoryStream.write			(&m_fMinSpeed,				sizeof(m_fMinSpeed));
		tMemoryStream.write			(&m_fMaxSpeed,				sizeof(m_fMaxSpeed));
		tMemoryStream.write			(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tMemoryStream.write			(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tMemoryStream.write			(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		tFileStream.Read			(&m_tNextGraphID,			sizeof(m_tNextGraphID));
		tFileStream.Read			(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
		tFileStream.Read			(&m_fMinSpeed,				sizeof(m_fMinSpeed));
		tFileStream.Read			(&m_fMaxSpeed,				sizeof(m_fMaxSpeed));
		tFileStream.Read			(&m_fCurSpeed,				sizeof(m_fCurSpeed));
		tFileStream.Read			(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
		tFileStream.Read			(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tNextGraphID				= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_tPrevGraphID				= tpSpawnPoints[tSpawnID].tNearestGraphPointID;
		m_fMinSpeed					= pSettings->ReadFLOAT	(tpSpawnPoints[tSpawnID].caModel, "minSpeed");
		m_fMaxSpeed					= pSettings->ReadFLOAT	(tpSpawnPoints[tSpawnID].caModel, "maxSpeed");
		m_fCurSpeed					= 0.0f;
		m_fDistanceFromPoint		= 0.0f;
		m_fDistanceToPoint			= 0.0f;
	};
};

class CALifeMonster : public CALifeMonsterAbstract {
public:
	typedef	CALifeMonsterAbstract inherited;
	
	CALifeMonsterParams				m_tMonsterParams;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		m_tMonsterParams.Save		(tMemoryStream);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_tMonsterParams.Load		(tFileStream);
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tMonsterParams.Init		(tSpawnID,tpSpawnPoints);
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

class CALifeHumanAbstract : public CALifeMonsterAbstract {
public:
	typedef	CALifeMonsterAbstract inherited;
	
	PERSONAL_EVENT_VECTOR			m_tpEvents;
	TASK_VECTOR						m_tpTaskIDs;
	float							m_fMaxItemMass;
	ETaskState						m_tTaskState;
	STask							m_tCurTask;
	DWORD_VECTOR					m_tpaVertices;
	u32								m_dwCurNode;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		// calling inherited
		inherited::Save				(tMemoryStream);
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
		{
			tMemoryStream.Wdword	(m_tpaVertices.size());
			DWORD_IT 				I = m_tpaVertices.begin();
			DWORD_IT 				E = m_tpaVertices.end();
			for ( ; I != E; I++)
				tMemoryStream.write	(I,sizeof(*I));
		}
		tMemoryStream.Wfloat		(m_fMaxItemMass);
		tMemoryStream.Wdword		(m_tTaskState);
		tMemoryStream.write			(&m_tCurTask,	sizeof(m_tCurTask));
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		// calling inherited
		inherited::Load				(tFileStream);
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
		{
			m_tpaVertices.resize	(tFileStream.Rdword());
			DWORD_IT 				I = m_tpaVertices.begin();
			DWORD_IT 				E = m_tpaVertices.end();
			for ( ; I != E; I++)
				tFileStream.Read	(I,sizeof(*I));
		}
		m_fMaxItemMass				= tFileStream.Rfloat();
		m_tTaskState				= ETaskState(tFileStream.Rdword());
		tFileStream.Read			(&m_tCurTask,	sizeof(m_tCurTask));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tpEvents.	clear			();
		m_tpTaskIDs.clear			();
		m_fMaxItemMass				= pSettings->ReadFLOAT				(tpSpawnPoints[tSpawnID].caModel, "max_item_mass");
		m_tTaskState				= eTaskStateNone;
		m_tCurTask.tTaskID			= u32(-1);
		m_dwCurNode					= u32(-1);
		m_tpaVertices.clear			();
	};
};

class CALifeHuman : public CALifeHumanAbstract {
public:
	typedef	CALifeHumanAbstract inherited;

	CALifeHumanParams				m_tHumanParams;
	bool							m_bIsTrader;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		m_tHumanParams.Save			(tMemoryStream);
		tMemoryStream.Wdword		(m_bIsTrader);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_tHumanParams.Load			(tFileStream);
		m_bIsTrader					= !!(tFileStream.Rdword());
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tHumanParams.Init			(tSpawnID,tpSpawnPoints);
		m_bIsTrader					= pSettings->LineExists(tpSpawnPoints[tSpawnID].caModel, "trader") && pSettings->ReadBOOL(tpSpawnPoints[tSpawnID].caModel, "trader");
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
