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
	_TIME_ID						m_tGameTime;
	u32								m_dwStartTime;
	float							m_fTimeFactor;
	bool							m_bLoaded;
	
	SSpawnHeader					m_tSpawnHeader;
	SALifeHeader					m_tALifeHeader;

	// buffer for union operations
	TASK_VECTOR						m_tpBufferTaskIDs;
	
	// static
	SPAWN_VECTOR					m_tpSpawnPoints;		// ������ spawn-point-��

	// automatic
	GRAPH_VECTOR_SVECTOR			m_tpTerrain;			// ������ �������: �� �������������� 
															//	��������� �������� ������ ����� 
															//  �����
	ALIFE_MONSTER_P_VECTOR_VECTOR	m_tpLocationOwners;		// ������ ������� : �� ����� ����� 
															//  �������� ������ � ����������
	GRAPH_POINT_VECTOR				m_tpGraphObjects;		// �� ����� ����� �������� ��� 
															//  ������������ �������
	ALIFE_MONSTER_P_VECTOR			m_tpScheduledObjects;	// ������ ����������� ��������
	HUMAN_P_VECTOR					m_tpTraders;			// ������ ���������
	
	// dynamic
	CALifeObjectRegistry			m_tObjectRegistry;		// ����� ��������
	CALifeEventRegistry				m_tEventRegistry;		// ����� �������
	CALifeTaskRegistry				m_tTaskRegistry;		// ����� �������

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
				m_tpGraphObjects[tpALifeItem->m_tGraphID].tpObjects.push_back(tpALifeItem);
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
			OBJECT_PAIR_IT				I = m_tObjectRegistry.m_tppMap.begin();
			OBJECT_PAIR_IT				E = m_tObjectRegistry.m_tppMap.end();
			for ( ; I != E; I++)
				vfUpdateDynamicData((*I).second);
		}
		{
			EVENT_PAIR_IT	I = m_tEventRegistry.m_tpMap.begin();
			EVENT_PAIR_IT	E = m_tEventRegistry.m_tpMap.end();
			for ( ; I != E; I++)
				m_tpGraphObjects[(*I).second.tGraphID].tpEvents.push_back(&((*I).second));
		}
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
				if (pSettings->LineExists((*I).caModel, "monster") && pSettings->ReadBOOL((*I).caModel, "monster"))
					if (((*I).wCount > 1) && pSettings->LineExists((*I).caModel, "single") && pSettings->ReadBOOL((*I).caModel, "single"))
						tpALifeDynamicObject	= new CALifeMonsterGroup;
					else
						tpALifeDynamicObject	= new CALifeMonster;
				else
					if (pSettings->LineExists((*I).caModel, "zone") && pSettings->ReadBOOL((*I).caModel, "zone"))
						tpALifeDynamicObject	= new CALifeAnomalousZone;
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
				tTask.tLocationID			= Level().AI.m_tpaGraph[tpALifeItem->m_tGraphID].tVertexType;
				tTask.tObjectID				= tpALifeItem->m_tObjectID;
				tTask.tTimeID				= tfGetGameTime();
				tTask.tTaskType				= eTaskTypeSearchForItemOL;
				m_tTaskRegistry.Add			(tTask);
				tpTrader->m_tpTaskIDs.push_back(tTask.tTaskID);
				break;
			}
		}
	}

	IC bool bfCheckIfTaskCompleted(CALifeHumanParams &tHumanParams, CALifeHumanAbstract *tpALifeHumanAbstract, OBJECT_IT &I)
	{
		I = tHumanParams.m_tpItemIDs.begin();
		OBJECT_IT	E = tHumanParams.m_tpItemIDs.end();
		for ( ; I != E; I++) {
			switch (tpALifeHumanAbstract->m_tCurTask.tTaskType) {
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemCG : {
					if (m_tObjectRegistry.m_tppMap[*I]->m_tClassID == tpALifeHumanAbstract->m_tCurTask.tClassID)
						return(true);
					break;
				}
				case eTaskTypeSearchForItemOL :
				case eTaskTypeSearchForItemOG : {
					if (m_tObjectRegistry.m_tppMap[*I]->m_tObjectID == tpALifeHumanAbstract->m_tCurTask.tObjectID)
						return(true);
					break;
				}
			};
		}
		return(false);
	}

	IC bool bfCheckIfTaskCompleted(CALifeHuman *tpALifeHuman, OBJECT_IT &I)
	{
		return(bfCheckIfTaskCompleted(tpALifeHuman->m_tHumanParams,tpALifeHuman,I));
	}

	IC bool bfCheckIfTaskCompleted(CALifeHuman *tpALifeHuman)
	{
		OBJECT_IT I;
		return(bfCheckIfTaskCompleted(tpALifeHuman,I));
	}

	void							vfAttachItem			(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID);
	void							vfDetachItem			(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID);
	void							vfCommunicateWithTrader	(CALifeHuman *tpALifeHuman, CALifeHuman *tpTrader);
	CALifeHuman *					tpfGetNearestSuitableTrader(CALifeHuman *tpALifeHuman);
	void							vfUpdateMonster			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfUpdateTrader			(CALifeHuman			*tpALifeHuman);
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