////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.h
//	Created 	: 05.01.2003
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_templates.h"

class IPureALifeLObject {
public:
	virtual void					Load(CStream	&tFileStream)	= 0;
};

class IPureALifeSObject {
public:
	virtual void					Save(CFS_Memory	&tMemoryStream) = 0;
};

class IPureALifeIObject {
public:
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints) = 0;
};

class IPureALifeLSObject : public IPureALifeLObject, public IPureALifeSObject {
};

class IPureALifeLSIObject : public IPureALifeLSObject, public IPureALifeIObject {
};

class CALifeGameTime : public IPureALifeLSObject {
public:
	_TIME_ID						m_tGameTime;
	_TIME_ID						m_tTimeAfterSurge;
	u32								m_dwStartTime;
	float							m_fTimeFactor;

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.open_chunk	(GAME_TIME_CHUNK_DATA);
		tMemoryStream.write			(&m_tGameTime,		sizeof(m_tGameTime));
		tMemoryStream.write			(&m_tTimeAfterSurge,sizeof(m_tTimeAfterSurge));
		tMemoryStream.Wdword		(m_dwStartTime);
		tMemoryStream.Wfloat		(m_fTimeFactor);
		tMemoryStream.close_chunk	();
	};
	
	virtual void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(GAME_TIME_CHUNK_DATA));
		tFileStream.Read			(&m_tGameTime,		sizeof(m_tGameTime));
		tFileStream.Read			(&m_tTimeAfterSurge,sizeof(m_tTimeAfterSurge));
		m_dwStartTime				= tFileStream.Rdword();
		m_fTimeFactor				= tFileStream.Rfloat();
	};
	
	IC void							vfSetTimeFactor(float fTimeFactor)
	{
		m_tGameTime		= tfGetGameTime();
		m_dwStartTime	= Level().timeServer();
		m_fTimeFactor	= fTimeFactor;
	};

	IC _TIME_ID						tfGetGameTime()
	{
		return(m_tGameTime + iFloor(m_fTimeFactor*float(Level().timeServer() - m_dwStartTime)));
	};
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
	GRAPH_VECTOR					m_tpRouteGraphPoints;
	
	virtual void					Load(CStream	&tFileStream)
	{
		m_tNearestGraphPointID		= tFileStream.Rword();
		tFileStream.Rstring			(m_caModel);
		m_ucTeam					= tFileStream.Rbyte();
		m_ucSquad					= tFileStream.Rbyte();
		m_ucGroup					= tFileStream.Rbyte();
		m_wGroupID					= tFileStream.Rword();
		m_wCount					= tFileStream.Rword();
		m_fBirthRadius				= tFileStream.Rfloat();
		m_fBirthProbability			= tFileStream.Rfloat();
		m_fIncreaseCoefficient		= tFileStream.Rfloat();
		m_fAnomalyDeathProbability	= tFileStream.Rfloat();
		load_base_vector			(m_tpRouteGraphPoints, tFileStream);
	};
};

class CALifeHeader : public IPureALifeLSObject {
public:
	u32								m_tALifeVersion;
	
	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.open_chunk	(ALIFE_CHUNK_DATA);
		tMemoryStream.write			(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		tMemoryStream.close_chunk	();
	}
	
	virtual void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(ALIFE_CHUNK_DATA));
		m_tALifeVersion				= tFileStream.Rdword();
		if (m_tALifeVersion != ALIFE_VERSION)
			THROW;
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
		save_base_vector			(m_tpItemIDs,tMemoryStream);
	};

	virtual void					Load(CStream	&tFileStream)
	{
		m_fCumulativeItemMass		= tFileStream.Rfloat();
		m_dwMoney					= tFileStream.Rdword();
		m_tRank						= EStalkerRank(tFileStream.Rdword());
		load_base_vector			(m_tpItemIDs,tFileStream);
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

	virtual							~CALifeTraderAbstract()
	{
		free_vector					(m_tpEvents);
	};
	
	virtual	void					Save(CFS_Memory	&tMemoryStream)
	{
		save_vector					(m_tpEvents,tMemoryStream);
		save_base_vector			(m_tpTaskIDs,tMemoryStream);
		tMemoryStream.Wfloat		(m_fMaxItemMass);
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		load_vector					(m_tpEvents,tFileStream);
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

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tEventID,		sizeof(m_tEventID));
		tMemoryStream.write			(&m_tTimeID,		sizeof(m_tTimeID));
		tMemoryStream.write			(&m_tGraphID,		sizeof(m_tGraphID));
		tMemoryStream.write			(&m_tBattleResult,	sizeof(m_tBattleResult));
		m_tpMonsterGroup1->Save		(tMemoryStream);
		m_tpMonsterGroup2->Save		(tMemoryStream);
	};
	
	virtual void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tEventID,		sizeof(m_tEventID));
		tFileStream.Read			(&m_tTimeID,		sizeof(m_tTimeID));
		tFileStream.Read			(&m_tGraphID,		sizeof(m_tGraphID));
		tFileStream.Read			(&m_tBattleResult,	sizeof(m_tBattleResult));
		m_tpMonsterGroup1->Load		(tFileStream);
		m_tpMonsterGroup2->Load		(tFileStream);
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

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tEventID,			sizeof(m_tEventID));
		tMemoryStream.write			(&m_tTimeID,			sizeof(m_tTimeID));
		tMemoryStream.write			(&m_tTaskID,			sizeof(m_tTaskID));
		tMemoryStream.Wdword		(m_iHealth);
		tMemoryStream.write			(&m_tRelation,			sizeof(m_tRelation));
		save_base_vector			(m_tpItemIDs, tMemoryStream);
	};
	
	virtual void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tEventID,		sizeof(m_tEventID));
		tFileStream.Read			(&m_tTimeID,		sizeof(m_tTimeID));
		tFileStream.Read			(&m_tTaskID,		sizeof(m_tTaskID));
		m_iHealth					= tFileStream.Rdword();
		tFileStream.Read			(&m_tRelation,		sizeof(m_tRelation));
		load_base_vector			(m_tpItemIDs, tFileStream);
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
			default : {
				break;
			}
		};
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
			default : {
				break;
			}
		};
	}
};

class CALifePersonalTask : public CALifeTask {
public:
	typedef CALifeTask inherited;
	u32								m_dwTryCount;

	CALifePersonalTask()
	{
		m_dwTryCount = 0;
	};
	
	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		inherited::Save				(tMemoryStream);
		tMemoryStream.Wdword		(m_dwTryCount);
	};

	virtual void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_dwTryCount				= tFileStream.Rdword();
	};
};

class CALifeAnomalousZone : public IPureALifeLSIObject {
public:
	EAnomalousZoneType				m_tAnomalousZone;

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.write			(&m_tAnomalousZone,sizeof(m_tAnomalousZone));
	};
	
	virtual void					Load(CStream	&tFileStream)
	{
		tFileStream.Read			(&m_tAnomalousZone,sizeof(m_tAnomalousZone));
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

	virtual							~CALifeMonsterGroup()
	{
		free_vector					(m_tpMembers);
	};
	
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
		init_vector					(m_tpMembers,tSpawnID,tpSpawnPoints);
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
	u32								m_dwCurTask;
	float							m_fSearchSpeed;

	virtual							~CALifeHumanAbstract()
	{
		free_vector					(m_tpTasks);
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		// calling inherited
		CALifeMonsterAbstract::Save	(tMemoryStream);
		CALifeTraderAbstract::Save	(tMemoryStream);
		save_base_vector			(m_tpaVertices,tMemoryStream);
		save_bool_vector			(m_baVisitedVertices,tMemoryStream);
		save_vector					(m_tpTasks,tMemoryStream);
		tMemoryStream.write			(&m_tTaskState,sizeof(m_tTaskState));
		tMemoryStream.Wdword		(m_dwCurNode);
		tMemoryStream.Wdword		(m_dwCurTaskLocation);
		tMemoryStream.Wdword		(m_dwCurTask);
		tMemoryStream.Wfloat		(m_fSearchSpeed);
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		// calling inherited
		CALifeMonsterAbstract::Load	(tFileStream);
		CALifeTraderAbstract::Load	(tFileStream);
		load_base_vector			(m_tpaVertices,tFileStream);
		load_bool_vector			(m_baVisitedVertices,tFileStream);
		load_vector					(m_tpTasks,tFileStream);
		tFileStream.Read			(&m_tTaskState,sizeof(m_tTaskState));
		m_dwCurNode					= tFileStream.Rdword();
		m_dwCurTaskLocation			= tFileStream.Rdword();
		m_dwCurTask					= tFileStream.Rdword();
		m_fSearchSpeed				= tFileStream.Rfloat();
	};

	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints)
	{
		CALifeMonsterAbstract::Init	(tSpawnID,tpSpawnPoints);
		CALifeTraderAbstract::Init	(tSpawnID,tpSpawnPoints);
		m_tpaVertices.clear			();
		m_baVisitedVertices.clear	();
		m_tpTasks.clear				();
		m_dwCurTask					= u32(-1);
		m_tTaskState				= eTaskStateNoTask;
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

	virtual							~CALifeHumanGroup()
	{
		free_vector					(m_tpMembers);
	};

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
		init_vector					(m_tpMembers,tSpawnID,tpSpawnPoints);
	};
};
