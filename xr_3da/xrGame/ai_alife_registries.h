////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_objects.h"
#include "game_graph.h"

class CSE_ALifeCreatureActor;
class CSE_ALifeHumanAbstract;

class CSE_ALifeObjectRegistry : public IPureALifeLSObject {
public:
	ALife::D_OBJECT_MAP				m_tObjectRegistry;			// список событий игры

									CSE_ALifeObjectRegistry		();
	virtual							~CSE_ALifeObjectRegistry	();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual void					Load						(IReader &tFileStream);
			void					Add							(CSE_ALifeDynamicObject *tpALifeDynamicObject);

	IC		CSE_ALifeDynamicObject	*tpfGetObjectByID			(ALife::_OBJECT_ID tObjectID, bool bNoAssert = false)
	{
		ALife::D_OBJECT_PAIR_IT		I = m_tObjectRegistry.find(tObjectID);
		
		if (!bNoAssert) {
			R_ASSERT2				(m_tObjectRegistry.end() != I,"Specified object hasn't been found in the Object registry!");
		}
		else
			if (m_tObjectRegistry.end() == I)
				return				(0);

		return						((*I).second);
	}
};

class CSE_ALifeEventRegistry : public IPureALifeLSObject {
public:
	ALife::_EVENT_ID				m_tEventID;					// идентификатор карты событий
	ALife::EVENT_MAP				m_tEventRegistry;			// список событий игры

									CSE_ALifeEventRegistry		();
	virtual							~CSE_ALifeEventRegistry		();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
	virtual	void					Add							(CSE_ALifeEvent	*tpEvent);
	
	IC		CSE_ALifeEvent			*tpfGetEventByID			(ALife::_EVENT_ID tEventID, bool bNoAssert = false)
	{
		ALife::EVENT_PAIR_IT		I = m_tEventRegistry.find(tEventID);

		if (!bNoAssert)
			R_ASSERT2				(m_tEventRegistry.end() != I,"Specified event hasn't been found in the Event registry!");
		else
			if (m_tEventRegistry.end() == I)
				return				(0);

		return						((*I).second);
	}
};

class CSE_ALifeTaskRegistry : public IPureALifeLSObject {
public:
	ALife::_TASK_ID					m_tTaskID;					// идентификатор карты событий
	ALife::TASK_MAP					m_tTaskRegistry;			// список событий игры
	ALife::OBJECT_TASK_MAP			m_tTaskCrossMap;

									CSE_ALifeTaskRegistry		();
	virtual							~CSE_ALifeTaskRegistry		();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
	virtual	void					Add							(CSE_ALifeTask *tpTask);
	virtual	void					Update						(CSE_ALifeTask *tpTask);

	IC		CSE_ALifeTask			*tpfGetTaskByID				(ALife::_TASK_ID tTaskID, bool bNoAssert = false)
	{
		ALife::TASK_PAIR_IT			I = m_tTaskRegistry.find(tTaskID);

		if (!bNoAssert)
			R_ASSERT2				(m_tTaskRegistry.end() != I,"Specified task hasn't been found in the Task registry!");
		else
			if (m_tTaskRegistry.end() == I)
				return				(0);

		return						((*I).second);
	}
};

class CSE_ALifeGraphRegistry {
public:
	CSE_ALifeCreatureActor			*m_tpActor;
	bool							m_bSwitchChanged;
	ALife::_OBJECT_ID				m_tNextFirstSwitchObjectID;
	ALife::_LEVEL_ID				m_tCurrentLevelID;
	ALife::D_OBJECT_MAP				*m_tpCurrentLevel;
	ALife::GRAPH_POINT_VECTOR		m_tpGraphObjects;									// по точке графа получить все 
	ALife::GRAPH_VECTOR				m_tpTerrain[LOCATION_TYPE_COUNT][ALife::LOCATION_COUNT];	
																						// массив списков: по идетнификатору 
																						//	местности получить список точек 
																						//  графа


									CSE_ALifeGraphRegistry		();
	virtual							~CSE_ALifeGraphRegistry		();
			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfAssignGraphPosition		(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
			void					vfAddObjectToCurrentLevel	(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfRemoveObjectFromCurrentLevel(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfAddObjectToGraphPoint		(CSE_ALifeDynamicObject *tpALifeDynamicObject,	ALife::_GRAPH_ID		tNextGraphPointID,		bool bUpdateSwitchObjects = true);
			void					vfRemoveObjectFromGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject,	ALife::_GRAPH_ID		tGraphID,				bool bUpdateSwitchObjects = true);
			void					vfChangeObjectGraphPoint	(CSE_ALifeDynamicObject *tpALifeDynamicObject,	ALife::_GRAPH_ID		tGraphPointID,			ALife::_GRAPH_ID tNextGraphPointID);
			// events
			void					vfRemoveEventFromGraphPoint	(CSE_ALifeEvent			*tpEvent,				ALife::_GRAPH_ID		tGraphID);
			void					vfAddEventToGraphPoint		(CSE_ALifeEvent			*tpEvent,				ALife::_GRAPH_ID		tNextGraphPointID);
			void					vfChangeEventGraphPoint		(CSE_ALifeEvent			*tpEvent,				ALife::_GRAPH_ID		tGraphPointID,			ALife::_GRAPH_ID tNextGraphPointID);
			void					vfAttachItem				(CSE_Abstract			&tAbstract,				CSE_ALifeInventoryItem	*tpALifeInventoryItem,	ALife::_GRAPH_ID tGraphID,			bool bALifeRequest = true);
			void					vfDetachItem				(CSE_Abstract			&tAbstract,				CSE_ALifeInventoryItem	*tpALifeInventoryItem,	ALife::_GRAPH_ID tGraphID,			bool bALifeRequest = true);
};

class CSE_ALifeTraderRegistry {
public:
	ALife::TRADER_P_VECTOR			m_tpTraders;			// массив торговцев
	ALife::TRADER_SET_MAP			m_tpCrossTraders;

	virtual							~CSE_ALifeTraderRegistry	(){}
			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			CSE_ALifeTrader			*tpfGetNearestSuitableTrader(CSE_ALifeHumanAbstract *tpALifeHumanAbstract);
};

class CSE_ALifeScheduleRegistry {
public:
	ALife::SCHEDULE_P_MAP			m_tpScheduledObjects;	// массив обновляемых объектов
	ALife::_OBJECT_ID				m_tNextFirstProcessObjectID;
	bool							m_bUpdateChanged;

	virtual							~CSE_ALifeScheduleRegistry	(){}
			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfAddObjectToScheduled		(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bUpdateSchedulableObjects = true);
			void					vfRemoveObjectFromScheduled	(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bUpdateSchedulableObjects = true);
};

class CSE_ALifeSpawnRegistry : public CSE_ALifeSpawnHeader {
public:
	typedef CSE_ALifeSpawnHeader inherited;
	
	ALife::D_OBJECT_P_VECTOR		m_tpSpawnPoints;
	CGameGraph::LEVEL_POINT_VECTOR	m_tpArtefactSpawnPositions;
	xr_vector<bool>					m_baAliveSpawnObjects;
	ALife::ITEM_SET_MAP				m_tArtefactAnomalyMap;


									CSE_ALifeSpawnRegistry		();
	virtual							~CSE_ALifeSpawnRegistry		();
			void					Init						();
	virtual void					Load						(IReader &tFileStream);
};

class CSE_ALifeAnomalyRegistry : public IPureALifeLSObject {
public:
	ALife::ANOMALY_P_VECTOR_VECTOR	m_tpAnomalies;
	ALife::ANOMALY_P_VECTOR_SVECTOR	m_tpCrossAnomalies;

									CSE_ALifeAnomalyRegistry	();
	virtual							~CSE_ALifeAnomalyRegistry	();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
};

class CSE_ALifeOrganizationRegistry : public IPureALifeLSObject {
public:
	ALife::ORGANIZATION_P_MAP		m_tOrganizationRegistry;
	ALife::DISCOVERY_P_MAP			m_tDiscoveryRegistry;
	ALife::LPSTR_BOOL_MAP			m_tArtefactRegistry;

									CSE_ALifeOrganizationRegistry();
	virtual							~CSE_ALifeOrganizationRegistry();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
};
