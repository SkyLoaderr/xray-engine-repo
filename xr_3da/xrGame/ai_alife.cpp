////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "game_sv_single.h"
#include "GameObject.h"

//#define DYNAMIC_ALLOCATION

// temporary
#define TOTAL_COUNT			  50
#define MONSTER_FACTOR		.40f
#define HUMAN_FACTOR		.10f
#define WEAPON_FACTOR		.10f
#define WEAPON_CH_FACTOR	.10f
#define EQUIPMENT_FACTOR	.10f
#define ARTEFACT_FACTOR		.20f

FLOAT_VECTOR				fpFactors;
// end of temporary

CAI_ALife::CAI_ALife()
{
	m_bLoaded			= false;
	m_tpActor			= 0;
}

CAI_ALife::~CAI_ALife()
{
	shedule_Unregister	();
	//Save				();
}

void CAI_ALife::vfInitTerrain()
{
	m_tpTerrain.resize(LOCATION_COUNT);
	{
		GRAPH_VECTOR_IT		I = m_tpTerrain.begin();
		GRAPH_VECTOR_IT		E = m_tpTerrain.end();
		for ( ; I != E; I++)
			(*I).clear();
	}
	for (_GRAPH_ID i=0; i<(_GRAPH_ID)Level().AI.GraphHeader().dwVertexCount; i++)
		m_tpTerrain[Level().AI.m_tpaGraph[i].tVertexType].push_back(i);
}

void CAI_ALife::vfNewGame()
{
	CALifeGraphRegistry::Init	();
	CALifeTraderRegistry::Init	();
	CALifeScheduleRegistry::Init();

	ALIFE_ENTITY_P_IT			B = m_tpSpawnPoints.begin();
	ALIFE_ENTITY_P_IT			E = m_tpSpawnPoints.end();
	for (ALIFE_ENTITY_P_IT I = B ; I != E; ) {
		u32	wGroupID = (*I)->m_dwSpawnGroup;
		float fSum = (*I)->m_ucProbability;
		for (ALIFE_ENTITY_P_IT j= I + 1; (j != E) && ((*j)->m_dwSpawnGroup == wGroupID); j++)
			fSum += (*j)->m_ucProbability;
		float fProbability = ::Random.randF(0,fSum);
		fSum = (*I)->m_ucProbability;
		ALIFE_ENTITY_P_IT m = j, k = I;
		for ( j= I + 1; (j != E) && ((*j)->m_dwSpawnGroup == wGroupID); j++) {
			fSum += (*j)->m_ucProbability;
			if (fSum > fProbability) {
				k = j;
				break;
			}
		}
		xrServerEntity			*tpServerEntity = F_entity_Create	((*I)->s_name);
		R_ASSERT2				(tpServerEntity,"Can't create entity.");
		CALifeObject			*i = dynamic_cast<CALifeObject*>(tpServerEntity);
		R_ASSERT				(i);
		(*I)->m_tObjectID = (*I)->ID;
		*i = **I;
		I = m;	
		
		vfCreateObject(i);
		m_tObjectRegistry.insert(make_pair(i->m_tObjectID,i));
	}
}

void CAI_ALife::Save()
{
	CFS_Memory					tStream;
	CALifeHeader::Save			(tStream);
	CALifeGameTime::Save		(tStream);
	CALifeObjectRegistry::Save	(tStream);
	CALifeEventRegistry::Save	(tStream);
	CALifeTaskRegistry::Save	(tStream);
	tStream.SaveTo				("game.sav",0);
}

void CAI_ALife::Load()
{
	shedule_Min					=     1;
	shedule_Max					=    20;
	m_dwObjectsBeingProcessed	=     0;
	m_qwMaxProcessTime			=  100*CPU::cycles_per_microsec;
	m_fOnlineDistance			=    10.f;

	if (!Level().AI.m_tpaGraph)
		return;

#ifdef USE_SINGLE_PLAYER
	vfInitTerrain				();
	CStream						*tpStream;
	FILE_NAME					caFileName;
	R_ASSERT					(Engine.FS.Exist(caFileName, ::Path.GameData, "game.spawn"));
	tpStream					= Engine.FS.Open(caFileName);
	CALifeSpawnRegistry::Load	(*tpStream);
	Engine.FS.Close				(tpStream);

	if (!Engine.FS.Exist(caFileName, ::Path.GameData, "game.sav")) {
		vfNewGame();
		Save();
		R_ASSERT(false);
	}
	else {
		tpStream					= Engine.FS.Open(caFileName);
		CALifeHeader::Load			(*tpStream);
		CALifeGameTime::Load		(*tpStream);
		CALifeObjectRegistry::Load	(*tpStream);
		CALifeEventRegistry::Load	(*tpStream);
		CALifeTaskRegistry::Load	(*tpStream);
	}
	vfUpdateDynamicData();

	m_bLoaded = true;
#endif
	return;
}

void CAI_ALife::vfUpdateDynamicData(CALifeObject *tpALifeObject)
{
	vfCreateObject					(tpALifeObject);
	tpALifeObject->m_bOnline		= true;
	CALifeGraphRegistry::Update		(tpALifeObject);
	CALifeTraderRegistry::Update	(tpALifeObject);
	CALifeScheduleRegistry::Update	(tpALifeObject);
}

void CAI_ALife::vfUpdateDynamicData()
{
	// initialize
	CALifeGraphRegistry::Init	();
	CALifeTraderRegistry::Init	();
	CALifeScheduleRegistry::Init();
	// update objects
	{
		OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
		OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
		for ( ; I != E; I++) {
			vfUpdateDynamicData((*I).second);
		}
	}
	// update events
	{
		EVENT_PAIR_IT	I = m_tEventRegistry.begin();
		EVENT_PAIR_IT	E = m_tEventRegistry.end();
		for ( ; I != E; I++)
			vfAddEventToGraphPoint((*I).second,(*I).second->m_tGraphID);
	}
}

void CAI_ALife::vfCreateNewDynamicObject(SPAWN_P_IT I, bool bUpdateDynamicData)
{
//	CALifeDynamicObject	*tpALifeDynamicObject = 0;
//	if (pSettings->LineExists((*I)->m_caModel, "scheduled") && pSettings->ReadBOOL((*I)->m_caModel, "scheduled")) {
//		if (pSettings->LineExists((*I)->m_caModel, "human") && pSettings->ReadBOOL((*I)->m_caModel, "human")) {
//			CALifeCreatureSpawnPoint *tpALifeCreatureSpawnPoint	= dynamic_cast<CALifeCreatureSpawnPoint *>(*I);
//			VERIFY(tpALifeCreatureSpawnPoint);
//			if ((tpALifeCreatureSpawnPoint->m_wCount > 1) && pSettings->LineExists(tpALifeCreatureSpawnPoint->m_caModel, "single") && pSettings->ReadBOOL(tpALifeCreatureSpawnPoint->m_caModel, "single"))
//				tpALifeDynamicObject	= xr_new<CALifeHumanGroup>();
//			else
//				if (pSettings->LineExists(tpALifeCreatureSpawnPoint->m_caModel, "trader") && pSettings->ReadBOOL(tpALifeCreatureSpawnPoint->m_caModel, "trader"))
//					tpALifeDynamicObject	= xr_new<CALifeTrader>();
//				else
//					tpALifeDynamicObject	= xr_new<CALifeHuman>();
//		}
//		else
//			if (pSettings->LineExists((*I)->m_caModel, "monster") && pSettings->ReadBOOL((*I)->m_caModel, "monster")) {
//				CALifeCreatureSpawnPoint *tpALifeCreatureSpawnPoint	= dynamic_cast<CALifeCreatureSpawnPoint *>(*I);
//				VERIFY(tpALifeCreatureSpawnPoint);
//				if ((tpALifeCreatureSpawnPoint->m_wCount > 1) && pSettings->LineExists(tpALifeCreatureSpawnPoint->m_caModel, "single") && pSettings->ReadBOOL(tpALifeCreatureSpawnPoint->m_caModel, "single"))
//					tpALifeDynamicObject	= xr_new<CALifeMonsterGroup>();
//				else
//					tpALifeDynamicObject	= xr_new<CALifeMonster>();
//			}
//			else
//				if (pSettings->LineExists((*I)->m_caModel, "zone") && pSettings->ReadBOOL((*I)->m_caModel, "zone"))
//					tpALifeDynamicObject	= xr_new<CALifeDynamicAnomalousZone>();
//				else {
//					Msg("!Unspecified ALife monster type!");
//					R_ASSERT(false);
//				}
//	}
//	else
//		if (pSettings->LineExists((*I)->m_caModel, "zone") && pSettings->ReadBOOL((*I)->m_caModel, "zone"))
//			tpALifeDynamicObject			= xr_new<CALifeAnomalousZone>();
//		else
//			tpALifeDynamicObject			= xr_new<CALifeItem>();
//
//	tpALifeDynamicObject->Init			(_SPAWN_ID(I - m_tpSpawnPoints.begin()),m_tpSpawnPoints);
//	CALifeObjectRegistry::Add			(tpALifeDynamicObject);
//	if (bUpdateDynamicData)
//		vfUpdateDynamicData				(tpALifeDynamicObject);
}

void CAI_ALife::vfCreateNewTask(CALifeTrader *tpTrader)
{
	OBJECT_PAIR_IT	I = m_tObjectRegistry.begin();
	OBJECT_PAIR_IT	E = m_tObjectRegistry.end();
	for ( ; I != E; I++) {
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>((*I).second);
		if (tpALifeItem && !tpALifeItem->m_bAttached) {
			CALifeTask					*tpTask = xr_new<CALifeTask>();
			tpTask->m_tCustomerID		= tpTrader->m_tObjectID;
			tpTask->m_tLocationID		= _LOCATION_ID(Level().AI.m_tpaGraph[tpALifeItem->m_tGraphID].tVertexType);
			tpTask->m_tObjectID			= tpALifeItem->m_tObjectID;
			tpTask->m_tTimeID			= tfGetGameTime();
			tpTask->m_tTaskType			= eTaskTypeSearchForItemOL;
			CALifeTaskRegistry::Add		(tpTask);
			tpTrader->m_tpTaskIDs.push_back(tpTask->m_tTaskID);
			break;
		}
	}
}

void CAI_ALife::vfGenerateAnomalousZones()
{

}

void CAI_ALife::vfGenerateArtefacts()
{
}

void CAI_ALife::vfSellArtefacts(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfUpdateArtefactOrders(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfGiveMilitariesBribe(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfBuySupplies(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfAssignPrices(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfBallanceCreatures()
{
}

void CAI_ALife::vfUpdateCreatures()
{
}