////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_space.h"
#include "ai_alife_static_objects.h"
#include "ai_alife_objects.h"
#include "ai_alife_registries.h"

#define ALIFE_SUPPORT_CONSOLE_COMMANDS

class CCompareTraderRanksPredicate {
public:
	bool operator()(const CALifeTrader *tpALifeTrader1, const CALifeTrader *tpALifeTrader2) const
	{
		return(tpALifeTrader1->m_tRank < tpALifeTrader2->m_tRank);
	};
};

class CAI_ALife : public CALifeHeader, public CALifeSpawnRegistry, public CALifeObjectRegistry, public CALifeEventRegistry, public CALifeTaskRegistry, public CSheduled {
public:
	u32								m_dwObjectsBeingProcessed;
	u64								m_qwMaxProcessTime;
	u32								m_dwStartTime;
	float							m_fTimeFactor;
	bool							m_bLoaded;
	
	// buffer for union operations
	TASK_VECTOR						m_tpBufferTaskIDs;
	
	// automatic
	GRAPH_VECTOR_SVECTOR			m_tpTerrain;			// массив списков: по идетнификатору 
															//	местности получить список точек 
															//  графа
	ALIFE_MONSTER_P_VECTOR_VECTOR	m_tpLocationOwners;		// массив списков : по точке графа 
															//  получить список её владельцев
	GRAPH_POINT_VECTOR				m_tpGraphObjects;		// по точке графа получить все 
															//  динамические объекты
	ALIFE_MONSTER_P_VECTOR			m_tpScheduledObjects;	// массив обновляемых объектов
	TRADER_P_VECTOR					m_tpTraders;			// массив торговцев
	
	IC void vfSetTimeFactor(float fTimeFactor)
	{
		m_tGameTime		= tfGetGameTime();
		m_dwStartTime	= Level().timeServer();
		m_fTimeFactor	= fTimeFactor;
	}

	IC _TIME_ID tfGetGameTime()
	{
		return(m_tGameTime + iFloor(m_fTimeFactor*float(Level().timeServer() - m_dwStartTime)));
	}

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
	IC void vfRemoveEventFromGraphPoint(CALifeEvent *tpEvent, _GRAPH_ID tGraphID)
	{
		EVENT_P_IT						I = m_tpGraphObjects[tGraphID].tpEvents.begin();
		EVENT_P_IT						E = m_tpGraphObjects[tGraphID].tpEvents.end();
		for ( ; I != E; I++)
			if ((*I) == tpEvent) {
				m_tpGraphObjects[tGraphID].tpEvents.erase(I);
				break;
			}
	}
	
	IC void vfAddEventToGraphPoint(CALifeEvent *tpEvent, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].tpEvents.push_back(tpEvent);
	}

	IC void vfChangeEventGraphPoint(CALifeEvent *tpEvent, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveEventFromGraphPoint	(tpEvent,tGraphPointID);
		vfAddEventToGraphPoint		(tpEvent,tNextGraphPointID);
	}

	IC void vfUpdateDynamicData(CALifeDynamicObject *tpALifeDynamicObject)
	{
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem) {
			if (!tpALifeItem->m_bAttached)
				m_tpGraphObjects[tpALifeItem->m_tGraphID].tpObjects.push_back(tpALifeItem);
			return;
		}
		CALifeTrader *tpALifeTrader = dynamic_cast<CALifeTrader *>(tpALifeDynamicObject);
		if (tpALifeTrader) {
			m_tpTraders.push_back(tpALifeTrader);
			sort(m_tpTraders.begin(),m_tpTraders.end(),CCompareTraderRanksPredicate());
		}
		
		m_tpGraphObjects[tpALifeDynamicObject->m_tGraphID].tpObjects.push_back(tpALifeDynamicObject);
		CALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CALifeMonsterAbstract *>(tpALifeDynamicObject);
		if (tpALifeMonsterAbstract) {
			m_tpScheduledObjects.push_back	(tpALifeMonsterAbstract);
			GRAPH_IT			I = m_tpSpawnPoints[tpALifeMonsterAbstract->m_tSpawnID]->m_tpRouteGraphPoints.begin(); 
			GRAPH_IT			E = m_tpSpawnPoints[tpALifeMonsterAbstract->m_tSpawnID]->m_tpRouteGraphPoints.end(); 
			for ( ; I != E; I++)
				m_tpLocationOwners[*I].push_back(tpALifeMonsterAbstract);
		}

	}
	
	IC void vfUpdateDynamicData()
	{
		m_tpLocationOwners.resize	(Level().AI.GraphHeader().dwVertexCount);
		{
			ALIFE_MONSTER_P_VECTOR_IT	I = m_tpLocationOwners.begin();
			ALIFE_MONSTER_P_VECTOR_IT	E = m_tpLocationOwners.end();
			for ( ; I != E; I++)
				(*I).clear();
		}
		m_tpGraphObjects.resize		(Level().AI.GraphHeader().dwVertexCount);
		{
			GRAPH_POINT_IT				I = m_tpGraphObjects.begin();
			GRAPH_POINT_IT				E = m_tpGraphObjects.end();
			for ( ; I != E; I++) {
				(*I).tpObjects.clear();
				(*I).tpEvents.clear();
			}
		}
		m_tpScheduledObjects.clear	();
		m_tpTraders.clear			();
		{
			OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
			OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
			for ( ; I != E; I++)
				vfUpdateDynamicData((*I).second);
		}
		{
			EVENT_PAIR_IT	I = m_tEventRegistry.begin();
			EVENT_PAIR_IT	E = m_tEventRegistry.end();
			for ( ; I != E; I++)
				m_tpGraphObjects[(*I).second->m_tGraphID].tpEvents.push_back((*I).second);
		}
	}

	IC void vfCreateNewDynamicObject(SPAWN_P_IT I, bool bUpdateDynamicData = false)
	{
		CALifeDynamicObject	*tpALifeDynamicObject = 0;
		if (pSettings->LineExists((*I)->m_caModel, "scheduled") && pSettings->ReadBOOL((*I)->m_caModel, "scheduled")) {
			if (pSettings->LineExists((*I)->m_caModel, "human") && pSettings->ReadBOOL((*I)->m_caModel, "human"))
				if (((*I)->m_wCount > 1) && pSettings->LineExists((*I)->m_caModel, "single") && pSettings->ReadBOOL((*I)->m_caModel, "single"))
					tpALifeDynamicObject	= new CALifeHumanGroup;
				else
					if (pSettings->LineExists((*I)->m_caModel, "trader") && pSettings->ReadBOOL((*I)->m_caModel, "trader"))
						tpALifeDynamicObject	= new CALifeTrader;
					else
						tpALifeDynamicObject	= new CALifeHuman;
			else
				if (pSettings->LineExists((*I)->m_caModel, "monster") && pSettings->ReadBOOL((*I)->m_caModel, "monster"))
					if (((*I)->m_wCount > 1) && pSettings->LineExists((*I)->m_caModel, "single") && pSettings->ReadBOOL((*I)->m_caModel, "single"))
						tpALifeDynamicObject	= new CALifeMonsterGroup;
					else
						tpALifeDynamicObject	= new CALifeMonster;
				else
					if (pSettings->LineExists((*I)->m_caModel, "zone") && pSettings->ReadBOOL((*I)->m_caModel, "zone"))
						tpALifeDynamicObject	= new CALifeDynamicAnomalousZone;
					else {
						Msg("!Unspecified ALife monster type!");
						R_ASSERT(false);
					}
		}
		else
			tpALifeDynamicObject			= new CALifeItem;

		tpALifeDynamicObject->Init			(_SPAWN_ID(I - m_tpSpawnPoints.begin()),m_tpSpawnPoints);
		CALifeObjectRegistry::Add			(tpALifeDynamicObject);
		if (bUpdateDynamicData)
			vfUpdateDynamicData				(tpALifeDynamicObject);
	};

	IC void vfCreateNewTask(CALifeTrader *tpTrader)
	{
		OBJECT_PAIR_IT	I = m_tObjectRegistry.begin();
		OBJECT_PAIR_IT	E = m_tObjectRegistry.end();
		for ( ; I != E; I++) {
			CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>((*I).second);
			if (tpALifeItem && !tpALifeItem->m_bAttached) {
				CALifeTask					tTask;
				tTask.m_tCustomerID			= tpTrader->m_tObjectID;
				tTask.m_tLocationID			= Level().AI.m_tpaGraph[tpALifeItem->m_tGraphID].tVertexType;
				tTask.m_tObjectID			= tpALifeItem->m_tObjectID;
				tTask.m_tTimeID				= tfGetGameTime();
				tTask.m_tTaskType			= eTaskTypeSearchForItemOL;
				CALifeTaskRegistry::Add		(&tTask);
				tpTrader->m_tpTaskIDs.push_back(tTask.m_tTaskID);
				break;
			}
		}
	}

	IC bool bfCheckIfTaskCompleted(CALifeHumanParams &tHumanParams, CALifeHumanAbstract *tpALifeHumanAbstract, OBJECT_IT &I)
	{
		I = tHumanParams.m_tpItemIDs.begin();
		OBJECT_IT	E = tHumanParams.m_tpItemIDs.end();
		for ( ; I != E; I++) {
			switch (tpALifeHumanAbstract->m_tCurTask.m_tTaskType) {
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemCG : {
					if (m_tObjectRegistry[*I]->m_tClassID == tpALifeHumanAbstract->m_tCurTask.m_tClassID)
						return(true);
					break;
				}
				case eTaskTypeSearchForItemOL :
				case eTaskTypeSearchForItemOG : {
					if (m_tObjectRegistry[*I]->m_tObjectID == tpALifeHumanAbstract->m_tCurTask.m_tObjectID)
						return(true);
					break;
				}
			};
		}
		return(false);
	}

	IC bool bfCheckIfTaskCompleted(CALifeHuman *tpALifeHuman, OBJECT_IT &I)
	{
		return(bfCheckIfTaskCompleted(*tpALifeHuman,tpALifeHuman,I));
	}

	IC bool bfCheckIfTaskCompleted(CALifeHuman *tpALifeHuman)
	{
		OBJECT_IT I;
		return(bfCheckIfTaskCompleted(tpALifeHuman,I));
	}

	void							vfAttachItem			(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID);
	void							vfDetachItem			(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID);
	void							vfCommunicateWithTrader	(CALifeHuman *tpALifeHuman, CALifeTrader *tpTrader);
	CALifeTrader *					tpfGetNearestSuitableTrader(CALifeHuman *tpALifeHuman);
	void							vfUpdateMonster			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfUpdateHumanGroup		(CALifeHumanGroup		*tpALifeHumanGroup);
	void							vfUpdateHuman			(CALifeHuman			*tpALifeHuman);
	bool							bfProcessItems			(CALifeHumanParams		&tHumanParams, _GRAPH_ID tGraphID, float fMaxItemMass);
	bool							bfCheckForItems			(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForDeletedEvents	(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForTheBattle		(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfChooseNextRoutePoint	(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfProcessNPC			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfInitTerrain			();
	void							vfLoadSpawnPoints		(CStream *tpStream);
	// temporary
	void							vfRandomizeGraphTerrain	();
	void							vfSaveSpawnPoints		();
	void							vfGenerateSpawnPoints	(const u32 dwTotalCount, FLOAT_VECTOR &fpFactors);
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
			void					vfGraphVertexInfo		(_GRAPH_ID &tGraphID);
#endif
};