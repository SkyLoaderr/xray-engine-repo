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

class IPureALifeLObject {
public:
	virtual void					Load(CStream	&tFileStream)	= 0;
};

class IPureALifeSObject {
public:
	virtual void					Save(CFS_Memory	&tMemoryStream) = 0;
};

class IPureALifeLSObject : public IPureALifeLObject, public IPureALifeSObject {
};

class IPureALifeLSIObject : public IPureALifeLSObject {
public:
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints) = 0;
};

template <class T>
void save_vector(vector<T *> &tpVector, CFS_Memory &tMemoryStream)
{
	tMemoryStream.Wdword			(tpVector.size());
	vector<T *>::iterator			I = tpVector.begin();
	vector<T *>::iterator			E = tpVector.end();
	for ( ; I != E; I++)
		(*I)->Save(tMemoryStream);
};

template <class T>
void load_vector(vector<T *> &tpVector, CStream &tFileStream)
{
	tpVector.resize					(tFileStream.Rdword());
	vector<T *>::iterator			I = tpVector.begin();
	vector<T *>::iterator			E = tpVector.end();
	for ( ; I != E; I++)
		(*I)->Load(tFileStream);
};

template <class T>
void save_base_vector(vector<T> &tpVector, CFS_Memory &tMemoryStream)
{
	tMemoryStream.Wdword	(tpVector.size());
	vector<T>::iterator		I = tpVector.begin();
	vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; I++)
		tMemoryStream.write(I,sizeof(*I));
};

template <class T>
void load_base_vector(vector<T> &tpVector, CStream &tFileStream)
{
	tpVector.resize			(tFileStream.Rdword());
	vector<T>::iterator		I = tpVector.begin();
	vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; I++)
		tFileStream.Read(I,sizeof(*I));
};

class CALifeSpawnHeader : public IPureALifeLObject {
public:
	u32								m_tSpawnVersion;
	
	virtual void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(SPAWN_POINT_CHUNK_VERSION));
		m_tSpawnVersion				= tFileStream.Rdword();
		if (m_tSpawnVersion != SPAWN_POINT_VERSION)
			THROW;
	};
};

class CALifeHeader : public IPureALifeLSObject {
public:
	u32								m_tALifeVersion;
	_TIME_ID						m_tGameTime;
	
	virtual void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(ALIFE_CHUNK_VERSION));
		m_tALifeVersion				= tFileStream.Rdword();
		if (m_tALifeVersion != ALIFE_VERSION)
			THROW;
		tFileStream.Read			(&m_tGameTime,sizeof(m_tGameTime));
	};
	
	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.open_chunk	(ALIFE_CHUNK_VERSION);
		tMemoryStream.write			(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		tMemoryStream.write			(&m_tGameTime,		sizeof(m_tGameTime));
		tMemoryStream.close_chunk	();
	}
};

class CALifeSpawnPoint : public IPureALifeLObject {
public:
	_GRAPH_ID						m_tNearestGraphPointID;
	string64						m_caModel;
	u8								m_ucTeam;
	u8								m_ucSquad;
	u8								m_ucGroup;
	u16								m_wGroupID;
	u16								m_wCount;
	float							m_fBirthRadius;
	float							m_fBirthProbability;
	float							m_fIncreaseCoefficient;
	float							m_fAnomalyDeathProbability;
	u8								m_ucRoutePointCount;
	GRAPH_VECTOR					m_tpRouteGraphPoints;
	
	virtual void					Load(CStream	&tFileStream)
	{
		m_tNearestGraphPointID		= tFileStream.Rword();
		tFileStream.Rstring				(m_caModel);
		m_ucTeam						= tFileStream.Rbyte();
		m_ucSquad					= tFileStream.Rbyte();
		m_ucGroup					= tFileStream.Rbyte();
		m_wGroupID					= tFileStream.Rword();
		m_wCount						= tFileStream.Rword();
		m_fBirthRadius				= tFileStream.Rfloat();
		m_fBirthProbability			= tFileStream.Rfloat();
		m_fIncreaseCoefficient		= tFileStream.Rfloat();
		m_fAnomalyDeathProbability	= tFileStream.Rfloat();
		m_ucRoutePointCount			= tFileStream.Rbyte();
		m_tpRouteGraphPoints.resize	(m_ucRoutePointCount);
		GRAPH_IT					i = m_tpRouteGraphPoints.begin();
		GRAPH_IT					e = m_tpRouteGraphPoints.end();
		for ( ; i != e; i++)
			*i	= tFileStream.Rword	();
	};
};

class CALifeMonsterParams : public IPureALifeLSIObject {
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
	
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		m_iHealth					= pSettings->ReadINT	(tpSpawnPoints[tSpawnID]->m_caModel, "health");
	};
};

class CALifeTraderParams : public IPureALifeLSIObject {
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
	
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		m_fCumulativeItemMass		= 0.0f;
		m_dwMoney					= 0;
		if (pSettings->LineExists(tpSpawnPoints[tSpawnID]->m_caModel, "money"))
			m_dwMoney = pSettings->ReadINT(tpSpawnPoints[tSpawnID]->m_caModel, "money");
		m_tRank						= EStalkerRank(pSettings->ReadINT	(tpSpawnPoints[tSpawnID]->m_caModel, "rank"));
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
	
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		CALifeMonsterParams::Init	(tSpawnID,tpSpawnPoints);
		CALifeTraderParams::Init	(tSpawnID,tpSpawnPoints);
	};
};

class CALifeTraderAbstract : public IPureALifeLSIObject {
public:
	PERSONAL_EVENT_P_VECTOR			m_tpEvents;
	TASK_VECTOR						m_tpTaskIDs;
	float							m_fMaxItemMass;
	
	virtual	void					Save(CFS_Memory	&tMemoryStream)
	{
		// saving events
		save_vector					(m_tpEvents,tMemoryStream);
		// saving tasks
		save_base_vector			(m_tpTaskIDs,tMemoryStream);
		tMemoryStream.Wfloat		(m_fMaxItemMass);
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		// loading events
		load_vector					(m_tpEvents,tFileStream);
		// loading tasks
		load_base_vector			(m_tpTaskIDs,tFileStream);
		m_fMaxItemMass				= tFileStream.Rfloat();
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		m_tpEvents.clear			();
		m_tpTaskIDs.clear			();
		m_fMaxItemMass				= pSettings->ReadFLOAT(tpSpawnPoints[tSpawnID]->m_caModel, "max_item_mass");
	};
};

class CALifeObject : public IPureALifeLSIObject {
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

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		LPCSTR						S = pSettings->ReadSTRING(tpSpawnPoints[tSpawnID]->m_caModel, "class");
		PSGP.memCopy				(&m_tClassID,S,sizeof(m_tClassID));
		m_tGraphID					= tpSpawnPoints[tSpawnID]->m_tNearestGraphPointID;
		m_tSpawnID					= tSpawnID;
		m_wCount					= tpSpawnPoints[tSpawnID]->m_wCount;
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

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_wCountAfter				= m_wCount;
	};
};

class CALifeEvent : public IPureALifeLSObject {
public:
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_GRAPH_ID						m_tGraphID;
	EBattleResult					m_tBattleResult;
	CALifeEventGroup				*m_tpMonsterGroup1;
	CALifeEventGroup				*m_tpMonsterGroup2;
	
	virtual void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tEventID,		sizeof(m_tEventID));
		tFileStream.Read			(&m_tTimeID,		sizeof(m_tTimeID));
		tFileStream.Read			(&m_tGraphID,		sizeof(m_tGraphID));
		tFileStream.Read			(&m_tBattleResult,	sizeof(m_tBattleResult));
		m_tpMonsterGroup1->Load		(tFileStream);
		m_tpMonsterGroup2->Load		(tFileStream);
	};

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tEventID,		sizeof(m_tEventID));
		tMemoryStream.write			(&m_tTimeID,		sizeof(m_tTimeID));
		tMemoryStream.write			(&m_tGraphID,		sizeof(m_tGraphID));
		tMemoryStream.write			(&m_tBattleResult,	sizeof(m_tBattleResult));
		m_tpMonsterGroup1->Save		(tMemoryStream);
		m_tpMonsterGroup2->Save		(tMemoryStream);
	};
};

class CALifePersonalEvent : public IPureALifeLSObject {
public:
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_TASK_ID						m_tTaskID;
	int								m_iHealth;
	ERelation						m_tRelation;
	OBJECT_VECTOR					m_tpItemIDs;
	
	virtual void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tEventID,		sizeof(m_tEventID));
		tFileStream.Read			(&m_tTimeID,		sizeof(m_tTimeID));
		tFileStream.Read			(&m_tTaskID,		sizeof(m_tTaskID));
		m_iHealth					= tFileStream.Rdword();
		tFileStream.Read			(&m_tRelation,		sizeof(m_tRelation));
		load_base_vector			(m_tpItemIDs, tFileStream);
	};

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tEventID,			sizeof(m_tEventID));
		tMemoryStream.write			(&m_tTimeID,			sizeof(m_tTimeID));
		tMemoryStream.write			(&m_tTaskID,			sizeof(m_tTaskID));
		tMemoryStream.Wdword		(m_iHealth);
		tMemoryStream.write			(&m_tRelation,			sizeof(m_tRelation));
		save_base_vector			(m_tpItemIDs, tMemoryStream);
	};
};

class CALifeTask : public IPureALifeLSObject {
public:
	_TASK_ID						m_tTaskID;
	_TIME_ID						m_tTimeID;
	_OBJECT_ID						m_tCustomerID;
	float							m_fCost;
	ETaskType						m_tTaskType;
	union {
		_CLASS_ID					m_tClassID;
		_OBJECT_ID					m_tObjectID;
	};
	union {
		_LOCATION_ID				m_tLocationID;
		_GRAPH_ID					m_tGraphID;
	};

	virtual void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tTaskID,	sizeof(m_tTaskID));
		tFileStream.Read			(&m_tTimeID,	sizeof(m_tTimeID));
		tFileStream.Read			(&m_tCustomerID,sizeof(m_tCustomerID));
		m_fCost						= tFileStream.Rfloat();
		tFileStream.Read			(&m_tTaskType,	sizeof(m_tTaskType));
		switch (m_tTaskType) {
			case eTaskTypeSearchForItemCL : {
				tFileStream.Read	(&m_tClassID,	sizeof(m_tClassID));
				tFileStream.Read	(&m_tLocationID,sizeof(m_tLocationID));
				break;
			}
			case eTaskTypeSearchForItemCG : {
				tFileStream.Read	(&m_tClassID,	sizeof(m_tClassID));
				tFileStream.Read	(&m_tGraphID,	sizeof(m_tGraphID));
				break;
			}
			case eTaskTypeSearchForItemOL : {
				tFileStream.Read	(&m_tObjectID,	sizeof(m_tObjectID));
				tFileStream.Read	(&m_tLocationID,sizeof(m_tLocationID));
				break;
			}
			case eTaskTypeSearchForItemOG : {
				tFileStream.Read	(&m_tObjectID,	sizeof(m_tObjectID));
				tFileStream.Read	(&m_tGraphID,	sizeof(m_tGraphID));
				break;
			}
			default : NODEFAULT;
		};
	}

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tTaskID,	sizeof(m_tTaskID));
		tMemoryStream.write			(&m_tTimeID,	sizeof(m_tTimeID));
		tMemoryStream.write			(&m_tCustomerID,sizeof(m_tCustomerID));
		tMemoryStream.Wfloat		(m_fCost);
		tMemoryStream.write			(&m_tTaskType,	sizeof(m_tTaskType));
		switch (m_tTaskType) {
			case eTaskTypeSearchForItemCL : {
				tMemoryStream.write	(&m_tClassID,	sizeof(m_tClassID));
				tMemoryStream.write	(&m_tLocationID,sizeof(m_tLocationID));
				break;
			}
			case eTaskTypeSearchForItemCG : {
				tMemoryStream.write	(&m_tClassID,	sizeof(m_tClassID));
				tMemoryStream.write	(&m_tGraphID,	sizeof(m_tGraphID));
				break;
			}
			case eTaskTypeSearchForItemOL : {
				tMemoryStream.write	(&m_tObjectID,	sizeof(m_tObjectID));
				tMemoryStream.write	(&m_tLocationID,sizeof(m_tLocationID));
				break;
			}
			case eTaskTypeSearchForItemOG : {
				tMemoryStream.write	(&m_tObjectID,	sizeof(m_tObjectID));
				tMemoryStream.write	(&m_tGraphID,	sizeof(m_tGraphID));
				break;
			}
			default : NODEFAULT;
		};
	};
};

class CALifePersonalTask : public IPureALifeLSObject {
public:
	_TASK_ID						m_tTaskID;
	_TIME_ID						m_tTimeID;
	float							m_fCost;
	u32								m_dwTryCount;

	virtual void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tTaskID,	sizeof(m_tTaskID));
		tFileStream.Read			(&m_tTimeID,	sizeof(m_tTimeID));
		m_fCost						= tFileStream.Rfloat();
		m_dwTryCount				= tFileStream.Rdword();
	};

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tTaskID,	sizeof(m_tTaskID));
		tMemoryStream.write			(&m_tTimeID,	sizeof(m_tTimeID));
		tMemoryStream.Wfloat		(m_fCost);
		tMemoryStream.Wdword		(m_dwTryCount);
	};
};

class CALifeAnomalousZone : public IPureALifeLSIObject {
public:
	EAnomalousZoneType				m_tAnomalousZone;
	
	virtual void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tAnomalousZone,sizeof(m_tAnomalousZone));
	};

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tAnomalousZone,sizeof(m_tAnomalousZone));
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		m_tAnomalousZone			= EAnomalousZoneType(pSettings->ReadINT(tpSpawnPoints[tSpawnID]->m_caModel, "anomaly_type"));
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

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
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

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_fMass						= pSettings->ReadFLOAT(tpSpawnPoints[tSpawnID]->m_caModel, "ph_mass");
		m_dwCost					= pSettings->ReadINT(tpSpawnPoints[tSpawnID]->m_caModel, "cost");
		if (pSettings->LineExists	(tpSpawnPoints[tSpawnID]->m_caModel, "health_value"))
			m_iHealthValue			= pSettings->ReadINT(tpSpawnPoints[tSpawnID]->m_caModel, "health_value");
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

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
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

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tNextGraphID				= tpSpawnPoints[tSpawnID]->m_tNearestGraphPointID;
		m_tPrevGraphID				= tpSpawnPoints[tSpawnID]->m_tNearestGraphPointID;
		m_fGoingSpeed				= pSettings->ReadFLOAT	(tpSpawnPoints[tSpawnID]->m_caModel, "going_speed");
		m_fCurSpeed					= 0.0f;
		m_fDistanceFromPoint		= 0.0f;
		m_fDistanceToPoint			= 0.0f;
	};
};

class CALifeDynamicAnomalousZone : public CALifeMonsterAbstract, public CALifeAnomalousZone {
public:
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		CALifeMonsterAbstract::Save	(tMemoryStream);
		CALifeAnomalousZone::Save	(tMemoryStream);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		CALifeMonsterAbstract::Load	(tFileStream);
		CALifeAnomalousZone::Load	(tFileStream);
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		CALifeMonsterAbstract::Init	(tSpawnID, tpSpawnPoints);
		CALifeAnomalousZone::Init	(tSpawnID, tpSpawnPoints);
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

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		CALifeMonsterAbstract::Init	(tSpawnID,tpSpawnPoints);
		CALifeMonsterParams::Init	(tSpawnID,tpSpawnPoints);
	};
};

class CALifeMonsterGroup : public CALifeMonsterAbstract {
public:
	typedef	CALifeMonsterAbstract inherited;
	
	MONSTER_PARAMS_P_VECTOR			m_tpMembers;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		save_vector					(m_tpMembers,tMemoryStream);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		load_vector					(m_tpMembers,tFileStream);
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tpMembers.resize			(m_wCount);
		MONSTER_PARAMS_P_IT 		I = m_tpMembers.begin();
		MONSTER_PARAMS_P_IT 		E = m_tpMembers.end();
		for ( ; I != E; I++)
			(*I)->Init				(tSpawnID,tpSpawnPoints);
	};
};

class CALifeHumanAbstract : public CALifeMonsterAbstract, public CALifeTraderAbstract {
public:
	DWORD_VECTOR					m_tpaVertices;
	BOOL_VECTOR						m_baVisitedVertices;
	PERSONAL_TASK_P_VECTOR			m_tpTasks;
	ETaskState						m_tTaskState;
	u32								m_dwCurNode;
	u32								m_dwCurTaskLocation;
	CALifeTask						m_tCurTask;
	float							m_fSearchSpeed;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		// calling inherited
		CALifeMonsterAbstract::Save	(tMemoryStream);
		CALifeTraderAbstract::Save	(tMemoryStream);
		save_base_vector			(m_tpaVertices,tMemoryStream);
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
		save_vector					(m_tpTasks,tMemoryStream);
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
		load_base_vector			(m_tpaVertices,tFileStream);
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
		load_vector					(m_tpTasks,tFileStream);
		m_tTaskState				= ETaskState(tFileStream.Rdword());
		m_dwCurNode					= tFileStream.Rdword();
		m_dwCurTaskLocation			= tFileStream.Rdword();
		tFileStream.Read			(&m_tCurTask,	sizeof(m_tCurTask));
		m_fSearchSpeed				= tFileStream.Rfloat();
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		CALifeMonsterAbstract::Init	(tSpawnID,tpSpawnPoints);
		CALifeTraderAbstract::Init	(tSpawnID,tpSpawnPoints);
		m_tpaVertices.clear			();
		m_baVisitedVertices.clear	();
		m_tpTasks.clear				();
		m_tTaskState				= eTaskStateNoTask;
		memset						(&m_tCurTask,0,sizeof(m_tCurTask));
		m_tCurTask.m_tTaskID		= u32(-1);
		m_dwCurNode					= u32(-1);
		m_dwCurTaskLocation			= u32(-1);
		m_fSearchSpeed				= pSettings->ReadFLOAT				(tpSpawnPoints[tSpawnID]->m_caModel, "search_speed");
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

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		CALifeHumanAbstract::Init	(tSpawnID,tpSpawnPoints);
		CALifeHumanParams::Init		(tSpawnID,tpSpawnPoints);
	};
};

class CALifeHumanGroup : public CALifeHumanAbstract {
public:
	typedef	CALifeHumanAbstract inherited;

	HUMAN_PARAMS_P_VECTOR			m_tpMembers;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		save_vector					(m_tpMembers,tMemoryStream);
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		load_vector					(m_tpMembers,tFileStream);
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		inherited::Init				(tSpawnID,tpSpawnPoints);
		m_tpMembers.resize			(m_wCount);
		HUMAN_PARAMS_P_IT			I = m_tpMembers.begin();
		HUMAN_PARAMS_P_IT			E = m_tpMembers.end();
		for ( ; I != E; I++)
			(*I)->Init				(tSpawnID,tpSpawnPoints);
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

	IC bool operator()(const CALifePersonalEvent *tPersonalEvent)  const
	{
		return(m_tpMap->find(tPersonalEvent->m_tEventID) == m_tpMap->end());
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
