////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_a_star.h"
#include "ai_alife_objects.h"
using namespace ALife;

class CSE_ALifeObjectRegistry : public IPureALifeLSObject {
public:
	D_OBJECT_MAP					m_tObjectRegistry;			// список событий игры

									CSE_ALifeObjectRegistry		();
	virtual							~CSE_ALifeObjectRegistry	();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual void					Load						(IReader &tFileStream);
			void					Add							(CSE_ALifeDynamicObject *tpALifeDynamicObject);

	IC		CSE_ALifeDynamicObject	*tpfGetObjectByID			(_OBJECT_ID tObjectID, bool bNoAssert = false)
	{
		D_OBJECT_PAIR_IT			I = m_tObjectRegistry.find(tObjectID);
		
		if (!bNoAssert)
			R_ASSERT2				(I != m_tObjectRegistry.end(),"Specified object hasn't been found in the Object registry!");
		else
			if (I == m_tObjectRegistry.end())
				return				(0);

		return						((*I).second);
	}
};

class CSE_ALifeEventRegistry : public IPureALifeLSObject {
public:
	_EVENT_ID						m_tEventID;					// идентификатор карты событий
	EVENT_MAP						m_tEventRegistry;			// список событий игры

									CSE_ALifeEventRegistry		();
	virtual							~CSE_ALifeEventRegistry		();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
	virtual	void					Add							(CSE_ALifeEvent	*tpEvent);
	
	IC		CSE_ALifeEvent			*tpfGetEventByID			(_EVENT_ID tEventID, bool bNoAssert = false)
	{
		EVENT_PAIR_IT				I = m_tEventRegistry.find(tEventID);

		if (!bNoAssert)
			R_ASSERT2				(I != m_tEventRegistry.end(),"Specified event hasn't been found in the Event registry!");
		else
			if (I == m_tEventRegistry.end())
				return				(0);

		return						((*I).second);
	}
};

class CSE_ALifeTaskRegistry : public IPureALifeLSObject {
public:
	_TASK_ID						m_tTaskID;					// идентификатор карты событий
	TASK_MAP						m_tTaskRegistry;			// список событий игры
	OBJECT_TASK_MAP					m_tTaskCrossMap;

									CSE_ALifeTaskRegistry		();
	virtual							~CSE_ALifeTaskRegistry		();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
	virtual	void					Add							(CSE_ALifeTask *tpTask);
	virtual	void					Update						(CSE_ALifeTask *tpTask);

	IC		CSE_ALifeTask			*tpfGetTaskByID				(_TASK_ID tTaskID, bool bNoAssert = false)
	{
		TASK_PAIR_IT				I = m_tTaskRegistry.find(tTaskID);

		if (!bNoAssert)
			R_ASSERT2				(I != m_tTaskRegistry.end(),"Specified task hasn't been found in the Task registry!");
		else
			if (I == m_tTaskRegistry.end())
				return				(0);

		return						((*I).second);
	}
};

class CSE_ALifeGraphRegistry : public CSE_ALifeAStar {
public:
	typedef CSE_ALifeAStar inherited;
	D_OBJECT_P_MAP_MAP				m_tLevelMap;
	CSE_ALifeCreatureActor			*m_tpActor;
	D_OBJECT_P_MAP					*m_tpCurrentLevel;
	GRAPH_POINT_VECTOR				m_tpGraphObjects;					// по точке графа получить все 
	GRAPH_VECTOR_SVECTOR			m_tpTerrain[LOCATION_TYPE_COUNT];	// массив списков: по идетнификатору 
       																	//	местности получить список точек 
																		//  графа
	_OBJECT_ID						m_tNextFirstSwitchObjectID;
	_LEVEL_ID						m_tCurrentLevelID;


									CSE_ALifeGraphRegistry		();
			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfRemoveObjectFromGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphID);
			void					vfAddObjectToGraphPoint		(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tNextGraphPointID);
			void					vfChangeObjectGraphPoint	(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID);
			// events
			void					vfRemoveEventFromGraphPoint	(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphID);
			void					vfAddEventToGraphPoint		(CSE_ALifeEvent *tpEvent, _GRAPH_ID tNextGraphPointID);
			void					vfChangeEventGraphPoint		(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID);
			void					vfAttachItem				(CSE_Abstract &CSE_Abstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bALifeRequest = true);
			void					vfDetachItem				(CSE_Abstract &CSE_Abstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bALifeRequest = true);
			void					vfRemoveObjectFromCurrentLevel(CSE_ALifeDynamicObject *tpALifeDynamicObject);
};

class CSE_ALifeTraderRegistry {
public:
	TRADER_P_VECTOR					m_tpTraders;			// массив торговцев
	TRADER_SET_MAP					m_tpCrossTraders;

			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
};

class CSE_ALifeScheduleRegistry {
public:
	MONSTER_P_MAP					m_tpScheduledObjects;	// массив обновляемых объектов
	_OBJECT_ID						m_tNextFirstProcessObjectID;

			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfAddObjectToScheduled		(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfRemoveObjectFromScheduled	(CSE_ALifeDynamicObject *tpALifeDynamicObject);
};

class CSE_ALifeSpawnRegistry : public CSE_ALifeSpawnHeader {
public:
	typedef CSE_ALifeSpawnHeader inherited;
	
	D_OBJECT_P_VECTOR				m_tpSpawnPoints;
	LEVEL_POINT_VECTOR				m_tpArtefactSpawnPositions;
	xr_vector<bool>					m_baAliveSpawnObjects;
	ITEM_SET_MAP					m_tArtefactAnomalyMap;


									CSE_ALifeSpawnRegistry		();
	virtual							~CSE_ALifeSpawnRegistry		();
	virtual void					Init						();
	virtual void					Load						(IReader &tFileStream);
};

class CSE_ALifeAnomalyRegistry : public IPureALifeLSObject {
public:
	ANOMALY_P_VECTOR_VECTOR			m_tpAnomalies;
	ANOMALY_P_VECTOR_SVECTOR		m_tpCrossAnomalies;

									CSE_ALifeAnomalyRegistry	();
	virtual							~CSE_ALifeAnomalyRegistry	();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
};

class CSE_ALifeOrganizationRegistry : public IPureALifeLSObject {
public:
	ORGANIZATION_P_MAP				m_tOrganizationRegistry;
	DISCOVERY_P_MAP					m_tDiscoveryRegistry;
	LPSTR_BOOL_MAP					m_tArtefactRegistry;

									CSE_ALifeOrganizationRegistry();
	virtual							~CSE_ALifeOrganizationRegistry();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
};
