////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_space.h"
#include "ai_alife_objects.h"
#include "ai_alife_registries.h"

#define ALIFE_SUPPORT_CONSOLE_COMMANDS

class CCompareTraderRanksPredicate {
public:
	bool operator()(const CALifeHuman *tpALifeHuman1, const CALifeHuman *tpALifeHuman2) const
	{
		return(tpALifeHuman1->m_tHumanParams.m_tRank < tpALifeHuman2->m_tHumanParams.m_tRank);
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

	// buffer for union operations
	TASK_VECTOR						m_tpBufferTaskIDs;
	
	// static
	SPAWN_VECTOR					m_tpSpawnPoints;		// массив spawn-point-ов

	// auto
	GRAPH_VECTOR_SVECTOR			m_tpTerrain;			// массив списков: по идетнификатору 
															//	местности получить список точек 
															//  графа
	ALIFE_MONSTER_P_VECTOR_VECTOR	m_tpLocationOwners;		// массив списков : по точке графа 
															//  получить список её владельцев
	GRAPH_POINT_VECTOR				m_tpGraphObjects;		// по точке графа получить все 
															//  динамические объекты
	ALIFE_MONSTER_P_VECTOR			m_tpScheduledObjects;	// массив обновляемых объектов
	HUMAN_P_VECTOR					m_tpTraders;			// массив торговцев
	
	// dynamic
	CALifeObjectRegistry			m_tObjectRegistry;		// карта объектов
	CALifeEventRegistry				m_tEventRegistry;		// карта событий
	CALifeTaskRegistry				m_tTaskRegistry;		// карта заданий

	// objects
	IC void vfRemoveObjectFromGraphPoint(CALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphID)
	{
		DYNAMIC_OBJECT_P_IT				I = m_tpGraphObjects[tGraphID].tpObjects.begin();
		DYNAMIC_OBJECT_P_IT				E = m_tpGraphObjects[tGraphID].tpObjects.end();
		for ( ; I != E; I++)
			if ((*I) == tpALifeDynamicObject) {
				m_tpGraphObjects[tGraphID].tpObjects.erase(I);
				break;
			}
	}
	
	IC void vfAddObjectToGraphPoint(CALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].tpObjects.push_back(tpALifeDynamicObject);
	}

	IC void vfChangeObjectGraphPoint(CALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveObjectFromGraphPoint	(tpALifeDynamicObject,tGraphPointID);
		vfAddObjectToGraphPoint			(tpALifeDynamicObject,tNextGraphPointID);
	}

	// events
	IC void vfRemoveEventFromGraphPoint(SEvent *tpEvent, _GRAPH_ID tGraphID)
	{
		EVENT_P_IT						I = m_tpGraphObjects[tGraphID].tpEvents.begin();
		EVENT_P_IT						E = m_tpGraphObjects[tGraphID].tpEvents.end();
		for ( ; I != E; I++)
			if ((*I) == tpEvent) {
				m_tpGraphObjects[tGraphID].tpEvents.erase(I);
				break;
			}
	}
	
	IC void vfAddEventToGraphPoint(SEvent *tpEvent, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].tpEvents.push_back(tpEvent);
	}

	IC void vfChangeEventGraphPoint(SEvent *tpEvent, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveEventFromGraphPoint	(tpEvent,tGraphPointID);
		vfAddEventToGraphPoint		(tpEvent,tNextGraphPointID);
	}

	IC void vfUpdateDynamicData(CALifeDynamicObject *tpALifeDynamicObject)
	{
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem) {
			if (!tpALifeItem->m_bAttached)
				m_tpGraphObjects[tpALifeDynamicObject->m_tGraphID].tpObjects.push_back(tpALifeDynamicObject);
			return;
		}
		m_tpGraphObjects[tpALifeDynamicObject->m_tGraphID].tpObjects.push_back(tpALifeDynamicObject);
		CALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CALifeMonsterAbstract *>(tpALifeDynamicObject);
		if (tpALifeMonsterAbstract) {
			m_tpScheduledObjects.push_back	(tpALifeMonsterAbstract);
			CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeDynamicObject);
			if (tpALifeHuman) {
				if (tpALifeHuman->m_bIsTrader) {
					m_tpTraders.push_back(tpALifeHuman);
					sort(m_tpTraders.begin(),m_tpTraders.end(),CCompareTraderRanksPredicate());
				}
			}
			GRAPH_IT			I = m_tpSpawnPoints[tpALifeMonsterAbstract->m_tSpawnID].tpRouteGraphPoints.begin(); 
			GRAPH_IT			E = m_tpSpawnPoints[tpALifeMonsterAbstract->m_tSpawnID].tpRouteGraphPoints.end(); 
			for ( ; I != E; I++)
				m_tpLocationOwners[*I].push_back(tpALifeMonsterAbstract);
		}
	}
	
	IC void vfUpdateDynamicData()
	{
		OBJECT_PAIR_IT			I = m_tObjectRegistry.m_tppMap.begin();
		OBJECT_PAIR_IT			E = m_tObjectRegistry.m_tppMap.end();
		for ( ; I != E; I++)
			vfUpdateDynamicData((*I).second);
	}

	IC void vfCreateNewDynamicObject(SPAWN_IT I, bool bUpdateDynamicData = false)
	{
		CALifeDynamicObject	*tpALifeDynamicObject;
		if (pSettings->LineExists((*I).caModel, "scheduled") && pSettings->ReadBOOL((*I).caModel, "scheduled")) {
			if (pSettings->LineExists((*I).caModel, "human") && pSettings->ReadBOOL((*I).caModel, "human"))
				if (((*I).wCount > 1) && pSettings->LineExists((*I).caModel, "single") && pSettings->ReadBOOL((*I).caModel, "single"))
					tpALifeDynamicObject	= new CALifeHumanGroup;
				else
					tpALifeDynamicObject	= new CALifeHuman;
			else
				if (((*I).wCount > 1) && pSettings->LineExists((*I).caModel, "single") && pSettings->ReadBOOL((*I).caModel, "single"))
					tpALifeDynamicObject	= new CALifeMonsterGroup;
				else
					tpALifeDynamicObject	= new CALifeMonster;
		}
		else
			tpALifeDynamicObject			= new CALifeItem;

		tpALifeDynamicObject->Init			(_SPAWN_ID(I - m_tpSpawnPoints.begin()),m_tpSpawnPoints);
		m_tObjectRegistry.Add				(tpALifeDynamicObject);
		if (bUpdateDynamicData)
			vfUpdateDynamicData				(tpALifeDynamicObject);
	};

	IC void vfCreateNewTask(CALifeHuman *tpTrader)
	{
		OBJECT_PAIR_IT	I = m_tObjectRegistry.m_tppMap.begin();
		OBJECT_PAIR_IT	E = m_tObjectRegistry.m_tppMap.end();
		for ( ; I != E; I++) {
			CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>((*I).second);
			if (tpALifeItem && !tpALifeItem->m_bAttached) {
				STask						tTask;
				tTask.tCustomerID			= tpTrader->m_tObjectID;
				tTask.tGraphID				= tpALifeItem->m_tGraphID;
				tTask.tClassID				= tpALifeItem->m_tClassID;
				tTask.tTimeID				= Level().timeServer();
				tTask.tTaskType				= eTaskTypeSearchForArtefact;
				m_tTaskRegistry.Add			(tTask);
				tpTrader->m_tpTaskIDs.push_back(tTask.tTaskID);
				break;
			}
		}
	}

	void							vfAttachItem			(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID);
	void							vfDetachItem			(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID);
	void							vfCommunicateWithTrader	(CALifeHuman *tpALifeHuman, CALifeHuman *tpTrader);
	CALifeHuman *					tpfGetNearestSuitableTrader(CALifeHuman *tpALifeHuman);
	void							vfUpdateMonster			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfUpdateTrader			(CALifeHuman			*tpALifeHuman);
	void							vfUpdateHumanGroup		(CALifeHumanGroup		*tpALifeHumanGroup);
	void							vfUpdateHuman			(CALifeHuman			*tpALifeHuman);
	void							vfProcessItems			(CALifeHumanParams		&tHumanParams, _GRAPH_ID tGraphID, float fMaxItemMass);
	void							vfCheckForItems			(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForDeletedEvents	(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForTheBattle		(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfChooseNextRoutePoint	(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfProcessNPC			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfInitTerrain			();
	void							vfLoadSpawnPoints		(CStream *tpStream);
	// temporary
	void							vfGenerateSpawnPoints	(const u32 dwTotalCount, FLOAT_VECTOR &fpFactors);
	void							vfSaveSpawnPoints		();
	//
public:
									CAI_ALife				();
	virtual							~CAI_ALife				();
	virtual float					shedule_Scale			()					{return .5f;};
	virtual BOOL					Ready					()					{return TRUE;};
	virtual LPCSTR					cName					()					{return "ALife Simulator";}; 
	virtual void					Load					();
	virtual void					Update					(u32 dt);	
			void					Save					();
			void					Generate				();
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
			void					vfListObjects			();
			void					vfListEvents			();
			void					vfListTasks				();
			void					vfListLocations			();
			void					vfListTerrain			();
			void					vfListSpawnPoints		();
			void					vfObjectInfo			(_OBJECT_ID	&tObjectID);
			void					vfEventInfo				(_EVENT_ID &tEventID);
			void					vfTaskInfo				(_TASK_ID &tTaskID);
			void					vfSpawnPointInfo		(_SPAWN_ID &tSpawnID);
#endif
};