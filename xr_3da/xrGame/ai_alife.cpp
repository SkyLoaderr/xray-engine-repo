////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

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

void CAI_ALife::Load()
{
	shedule_Min					=     1;
	shedule_Max					=    20;
	m_dwObjectsBeingProcessed	=     0;
	m_qwMaxProcessTime			=  100*CPU::cycles_per_microsec;

	if (!Level().AI.m_tpaGraph)
		return;

	return;
//
//	vfInitTerrain();
//
//	CStream						*tpStream;
//	FILE_NAME					caFileName;
//	if (!Engine.FS.Exist(caFileName, ::Path.GameData, "game.spawn")) {
////		THROW;
//#ifdef DEBUG
//		fpFactors.push_back(MONSTER_FACTOR);
//		fpFactors.push_back(HUMAN_FACTOR);
//		fpFactors.push_back(WEAPON_FACTOR);
//		fpFactors.push_back(WEAPON_CH_FACTOR);
//		fpFactors.push_back(EQUIPMENT_FACTOR);
//		fpFactors.push_back(ARTEFACT_FACTOR);
//		vfGenerateSpawnPoints(TOTAL_COUNT,fpFactors);
//		vfSaveSpawnPoints();
//		vfRandomizeGraphTerrain();
//#else
//		return;
//#endif
//	}
//	else {
//		tpStream = Engine.FS.Open(caFileName);
//		CALifeSpawnRegistry::Load(*tpStream);
//		Engine.FS.Close(tpStream);
//	}
//
//	if (!Engine.FS.Exist(caFileName,::Path.GameData,"game.alife")) {
//		Generate();
//		Save();
//	}
//	
//	if (!Engine.FS.Exist(caFileName,::Path.GameData,"game.alife"))
//		THROW;
//	
//	tpStream = Engine.FS.Open	(caFileName);
//	CALifeHeader::Load			(*tpStream);
//	CALifeGameTime::Load		(*tpStream);
//	CALifeObjectRegistry::Load	(*tpStream);
//	CALifeEventRegistry::Load	(*tpStream);
//	CALifeTaskRegistry::Load	(*tpStream);
//	vfUpdateDynamicData			();
//	Engine.FS.Close				(tpStream);
//
//	m_bLoaded					= true;
}

void CAI_ALife::Save()
{
	CFS_Memory					tStream;
	CALifeHeader::Save			(tStream);
	CALifeGameTime::Save		(tStream);
	CALifeObjectRegistry::Save	(tStream);
	CALifeEventRegistry::Save	(tStream);
	CALifeTaskRegistry::Save	(tStream);
	tStream.SaveTo				("game.alife",0);
}

void CAI_ALife::vfUpdateDynamicData(CALifeDynamicObject *tpALifeDynamicObject)
{
	CALifeGraphRegistry::Update		(tpALifeDynamicObject);
	CALifeTraderRegistry::Update	(tpALifeDynamicObject);
	CALifeScheduleRegistry::Update	(tpALifeDynamicObject);
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
}

void CAI_ALife::vfCreateNewDynamicObject(SPAWN_P_IT I, bool bUpdateDynamicData)
{
	CALifeDynamicObject	*tpALifeDynamicObject = 0;
	if (pSettings->LineExists((*I)->m_caModel, "scheduled") && pSettings->ReadBOOL((*I)->m_caModel, "scheduled")) {
		if (pSettings->LineExists((*I)->m_caModel, "human") && pSettings->ReadBOOL((*I)->m_caModel, "human")) {
			CALifeCreatureSpawnPoint *tpALifeCreatureSpawnPoint	= dynamic_cast<CALifeCreatureSpawnPoint *>(*I);
			VERIFY(tpALifeCreatureSpawnPoint);
			if ((tpALifeCreatureSpawnPoint->m_wCount > 1) && pSettings->LineExists(tpALifeCreatureSpawnPoint->m_caModel, "single") && pSettings->ReadBOOL(tpALifeCreatureSpawnPoint->m_caModel, "single"))
				tpALifeDynamicObject	= xr_new<CALifeHumanGroup>();
			else
				if (pSettings->LineExists(tpALifeCreatureSpawnPoint->m_caModel, "trader") && pSettings->ReadBOOL(tpALifeCreatureSpawnPoint->m_caModel, "trader"))
					tpALifeDynamicObject	= xr_new<CALifeTrader>();
				else
					tpALifeDynamicObject	= xr_new<CALifeHuman>();
		}
		else
			if (pSettings->LineExists((*I)->m_caModel, "monster") && pSettings->ReadBOOL((*I)->m_caModel, "monster")) {
				CALifeCreatureSpawnPoint *tpALifeCreatureSpawnPoint	= dynamic_cast<CALifeCreatureSpawnPoint *>(*I);
				VERIFY(tpALifeCreatureSpawnPoint);
				if ((tpALifeCreatureSpawnPoint->m_wCount > 1) && pSettings->LineExists(tpALifeCreatureSpawnPoint->m_caModel, "single") && pSettings->ReadBOOL(tpALifeCreatureSpawnPoint->m_caModel, "single"))
					tpALifeDynamicObject	= xr_new<CALifeMonsterGroup>();
				else
					tpALifeDynamicObject	= xr_new<CALifeMonster>();
			}
			else
				if (pSettings->LineExists((*I)->m_caModel, "zone") && pSettings->ReadBOOL((*I)->m_caModel, "zone"))
					tpALifeDynamicObject	= xr_new<CALifeDynamicAnomalousZone>();
				else {
					Msg("!Unspecified ALife monster type!");
					R_ASSERT(false);
				}
	}
	else
		if (pSettings->LineExists((*I)->m_caModel, "zone") && pSettings->ReadBOOL((*I)->m_caModel, "zone"))
			tpALifeDynamicObject			= xr_new<CALifeAnomalousZone>();
		else
			tpALifeDynamicObject			= xr_new<CALifeItem>();

	tpALifeDynamicObject->Init			(_SPAWN_ID(I - m_tpSpawnPoints.begin()),m_tpSpawnPoints);
	CALifeObjectRegistry::Add			(tpALifeDynamicObject);
	if (bUpdateDynamicData)
		vfUpdateDynamicData				(tpALifeDynamicObject);
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
			tpTask->m_tLocationID		= Level().AI.m_tpaGraph[tpALifeItem->m_tGraphID].tVertexType;
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