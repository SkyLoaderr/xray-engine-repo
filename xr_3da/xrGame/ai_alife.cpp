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

CAI_ALife::CAI_ALife(xrServer *tpServer)
{
	m_bLoaded			= false;
	m_tpActor			= 0;
	m_tpServer			= tpServer;
}

CAI_ALife::~CAI_ALife()
{
	shedule_Unregister	();
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
	for (_GRAPH_ID i=0; i<(_GRAPH_ID)CALifeGraph::Header().dwVertexCount; i++)
		m_tpTerrain[m_tpaGraph[i].tVertexType].push_back(i);
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
		CALifeDynamicObject			*i = dynamic_cast<CALifeDynamicObject*>(tpServerEntity);
		R_ASSERT				(i);
		(*I)->m_tObjectID		= (*I)->ID;
		*i						= **I;
		I						= m;	
		i->ID					= 0xffff;
		vfCreateObject			(i);
		i->m_tObjectID			= i->ID;
		m_tObjectRegistry.insert(make_pair(i->m_tObjectID,i));
	}
	m_tALifeVersion				= ALIFE_VERSION;
	m_tGameTime					= u64(Device.dwTimeGlobal);
}

void CAI_ALife::Save()
{
	CFS_Memory					tStream;
	CALifeHeader::Save			(tStream);
	CALifeGameTime::Save		(tStream);
	CALifeObjectRegistry::Save	(tStream);
	CALifeEventRegistry::Save	(tStream);
	CALifeTaskRegistry::Save	(tStream);
	string256					S;
	strconcat					(S,SAVE_PATH,SAVE_NAME);
	tStream.SaveTo				(S,0);
}

void CAI_ALife::Load()
{
	shedule_Min					=     1;
	shedule_Max					=    20;
	m_dwObjectsBeingProcessed	=     0;
	m_dwObjectsBeingSwitched	=	  0;
	m_qwMaxProcessTime			=  100*CPU::cycles_per_microsec;
	m_fOnlineDistance			=    10.f;
	m_tpActor					=	  0;
	m_tALifeVersion				= ALIFE_VERSION;
	m_tGameTime					=	  0;

#ifdef USE_SINGLE_PLAYER
	FILE_NAME					caFileName;
	if (!Engine.FS.Exist(caFileName,SAVE_PATH,SAVE_NAME)) {
		CStream					*tpStream;
		R_ASSERT				(Engine.FS.Exist(caFileName, ::Path.GameData, SPAWN_NAME));
		tpStream				= Engine.FS.Open(caFileName);
		Log						("* Loading spawn registry");
		CALifeSpawnRegistry::Load(*tpStream);
		Engine.FS.Close			(tpStream);
		vfNewGame();
		Save();
		R_ASSERT(false);
	}
	tpStream					= Engine.FS.Open(caFileName);
	R_ASSERT					(tpStream);
	CALifeHeader::Load			(*tpStream);
	CALifeGameTime::Load		(*tpStream);
	CALifeObjectRegistry::Load	(*tpStream);
	CALifeEventRegistry::Load	(*tpStream);
	CALifeTaskRegistry::Load	(*tpStream);
	vfUpdateDynamicData			();
	vfInitTerrain				();

	m_bLoaded = true;
#endif
}

void CAI_ALife::vfUpdateDynamicData(CALifeDynamicObject *tpALifeObject)
{
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

void CAI_ALife::vfCreateNewTask(CALifeTrader *tpTrader)
{
	OBJECT_PAIR_IT	I = m_tObjectRegistry.begin();
	OBJECT_PAIR_IT	E = m_tObjectRegistry.end();
	for ( ; I != E; I++) {
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>((*I).second);
		if (tpALifeItem && !tpALifeItem->m_bAttached) {
			CALifeTask					*tpTask = xr_new<CALifeTask>();
			tpTask->m_tCustomerID		= tpTrader->m_tObjectID;
			tpTask->m_tLocationID		= _LOCATION_ID(m_tpaGraph[tpALifeItem->m_tGraphID].tVertexType);
			tpTask->m_tObjectID			= tpALifeItem->m_tObjectID;
			tpTask->m_tTimeID			= tfGetGameTime();
			tpTask->m_tTaskType			= eTaskTypeSearchForItemOL;
			CALifeTaskRegistry::Add		(tpTask);
			tpTrader->m_tpTaskIDs.push_back(tpTask->m_tTaskID);
			break;
		}
	}
}

CALifeTrader* CAI_ALife::tpfGetNearestSuitableTrader(CALifeHuman *tpALifeHuman)
{
	float			fBestDistance = MAX_NODE_ESTIMATION_COST;
	CALifeTrader *	tpBestTrader = 0;
	TRADER_P_IT		I = m_tpTraders.begin();
	TRADER_P_IT		E = m_tpTraders.end();
	Fvector			&tGlobalPoint = m_tpaGraph[tpALifeHuman->m_tGraphID].tGlobalPoint;
	for ( ; I != E; I++) {
		if ((*I)->m_tRank != tpALifeHuman->m_tRank)
			break;
		float fCurDistance = m_tpaGraph[(*I)->m_tGraphID].tGlobalPoint.distance_to(tGlobalPoint);
		if (fCurDistance < fBestDistance) {
			fBestDistance = fCurDistance;
			tpBestTrader = *I;
		}
	}
	return(tpBestTrader);
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