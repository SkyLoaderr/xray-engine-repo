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

class CAI_ALife : 
	public CALifeHeader, 
	public CALifeGameTime,
	public CALifeSpawnRegistry, 
	public CALifeObjectRegistry, 
	public CALifeEventRegistry, 
	public CALifeTaskRegistry, 
	public CALifeGraphRegistry,
	public CALifeOwnerRegistry,
	public CALifeTraderRegistry,
	public CSheduled,
	public CRandom 
{
public:
	u32								m_dwObjectsBeingProcessed;
	u64								m_qwMaxProcessTime;
	bool							m_bLoaded;
	EZoneState						m_tZoneState;
	
	// buffer for union operations
	TASK_VECTOR						m_tpBufferTaskIDs;
	
	// automatic
	GRAPH_VECTOR_SVECTOR			m_tpTerrain;			// массив списков: по идетнификатору 
															//	местности получить список точек 
															//  графа
	ALIFE_MONSTER_P_VECTOR			m_tpScheduledObjects;	// массив обновляемых объектов
	
	IC void vfUpdateDynamicData(CALifeDynamicObject *tpALifeDynamicObject)
	{
		CALifeGraphRegistry::Update(tpALifeDynamicObject);
		CALifeTraderRegistry::Update(tpALifeDynamicObject);
		CALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CALifeMonsterAbstract *>(tpALifeDynamicObject);
		if (tpALifeMonsterAbstract) {
			m_tpScheduledObjects.push_back	(tpALifeMonsterAbstract);
			GRAPH_IT			I = m_tpSpawnPoints[tpALifeMonsterAbstract->m_tSpawnID]->m_tpRouteGraphPoints.begin(); 
			GRAPH_IT			E = m_tpSpawnPoints[tpALifeMonsterAbstract->m_tSpawnID]->m_tpRouteGraphPoints.end(); 
			for ( ; I != E; I++)
				CALifeOwnerRegistry::Update(*I,tpALifeMonsterAbstract);
		}
	};
	
	IC void vfUpdateDynamicData()
	{
		// initialize
		CALifeOwnerRegistry::Init	();
		CALifeGraphRegistry::Init	();
		CALifeGraphRegistry::Init	();
		m_tpScheduledObjects.clear	();
		// update objects
		{
			OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
			OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
			for ( ; I != E; I++)
				vfUpdateDynamicData((*I).second);
		}
		// update events
		{
			EVENT_PAIR_IT	I = m_tEventRegistry.begin();
			EVENT_PAIR_IT	E = m_tEventRegistry.end();
			for ( ; I != E; I++)
				vfAddEventToGraphPoint((*I).second,(*I).second->m_tGraphID);
		}
	};

	IC void vfCreateNewDynamicObject(SPAWN_P_IT I, bool bUpdateDynamicData = false)
	{
		CALifeDynamicObject	*tpALifeDynamicObject = 0;
		if (pSettings->LineExists((*I)->m_caModel, "scheduled") && pSettings->ReadBOOL((*I)->m_caModel, "scheduled")) {
			if (pSettings->LineExists((*I)->m_caModel, "human") && pSettings->ReadBOOL((*I)->m_caModel, "human"))
				if (((*I)->m_wCount > 1) && pSettings->LineExists((*I)->m_caModel, "single") && pSettings->ReadBOOL((*I)->m_caModel, "single"))
					tpALifeDynamicObject	= xr_new<CALifeHumanGroup>();
				else
					if (pSettings->LineExists((*I)->m_caModel, "trader") && pSettings->ReadBOOL((*I)->m_caModel, "trader"))
						tpALifeDynamicObject	= xr_new<CALifeTrader>();
					else
						tpALifeDynamicObject	= xr_new<CALifeHuman>();
			else
				if (pSettings->LineExists((*I)->m_caModel, "monster") && pSettings->ReadBOOL((*I)->m_caModel, "monster"))
					if (((*I)->m_wCount > 1) && pSettings->LineExists((*I)->m_caModel, "single") && pSettings->ReadBOOL((*I)->m_caModel, "single"))
						tpALifeDynamicObject	= xr_new<CALifeMonsterGroup>();
					else
						tpALifeDynamicObject	= xr_new<CALifeMonster>();
				else
					if (pSettings->LineExists((*I)->m_caModel, "zone") && pSettings->ReadBOOL((*I)->m_caModel, "zone"))
						tpALifeDynamicObject	= xr_new<CALifeDynamicAnomalousZone>();
					else {
						Msg("!Unspecified ALife monster type!");
						R_ASSERT(false);
					}
		}
		else
			tpALifeDynamicObject			= xr_new<CALifeItem>();

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
				CALifeTask					*tpTask = xr_new<CALifeTask>();
				tpTask->m_tCustomerID		= tpTrader->m_tObjectID;
				tpTask->m_tLocationID		= Level().AI.m_tpaGraph[tpALifeItem->m_tGraphID].tVertexType;
				tpTask->m_tObjectID			= tpALifeItem->m_tObjectID;
				tpTask->m_tTimeID			= tfGetGameTime();
				tpTask->m_tTaskType			= eTaskTypeSearchForItemOL;
				CALifeTaskRegistry::Add		(tpTask);
				tpTrader->m_tpTaskIDs.push_back(tpTask->m_tTaskID);
				break;
			}
		}
	};

	// surge
	void							vfGenerateArtefacts		();
	void							vfCreateZoneShot		();
	void							vfSellArtefacts			(CALifeTrader &tTrader);
	void							vfUpdateArtefactOrders	(CALifeTrader &tTrader);
	void							vfGiveMilitariesBribe	(CALifeTrader &tTrader);
	void							vfBuySupplies			(CALifeTrader &tTrader);
	void							vfBuyZoneShot			(CALifeTrader &tTrader);
	void							vfAssignPrices			(CALifeTrader &tTrader);
	void							vfBallanceCreatures		();
	void							vfUpdateCreatures		();
	// after surge
	void							vfCommunicateWithTrader	(CALifeHuman *tpALifeHuman, CALifeTrader *tpTrader);
	void							vfUpdateMonster			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfUpdateHumanGroup		(CALifeHumanGroup		*tpALifeHumanGroup);
	void							vfUpdateHuman			(CALifeHuman			*tpALifeHuman);
	bool							bfProcessItems			(CALifeHumanParams		&tHumanParams, _GRAPH_ID tGraphID, float fMaxItemMass, float fProbability);
	bool							bfCheckForItems			(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForDeletedEvents	(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForTheBattle		(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfChooseNextRoutePoint	(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfProcessNPC			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfInitTerrain			();
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