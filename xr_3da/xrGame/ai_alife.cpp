////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_funcs.h"

CAI_ALife::CAI_ALife(xrServer *tpServer)
{
	m_tpServer			= tpServer;
	m_bLoaded			= false;
	m_tpActor			= 0;
	m_tpAI_DDD			= 0;
}

CAI_ALife::~CAI_ALife()
{
	shedule_Unregister	();
	xr_delete			(m_tpAI_DDD);
}


float CAI_ALife::shedule_Scale()
{
	return(.5f);
}

BOOL CAI_ALife::Ready()
{
	return(TRUE);
}

LPCSTR CAI_ALife::cName()
{
	return("ALife Simulator");
}; 


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
	Memory.mem_compact			();
	u32							dwMemUsage = Memory.mem_usage();
	Log							("* Loading ALife Simulator...");
	m_tALifeVersion				= ALIFE_VERSION;
	m_tpActor					= 0;
	m_tGameTime					= 0;
	m_dwObjectsBeingProcessed	= 0;
	m_dwObjectsBeingSwitched	= 0;
	
	Log							("* Loading parameters...");
	shedule_Min					= pSettings->ReadINT	("alife","schedule_min");
	shedule_Max					= pSettings->ReadINT	("alife","schedule_max");
	m_qwMaxProcessTime			= pSettings->ReadINT	("alife","procees_time")*CPU::cycles_per_microsec;
	m_fOnlineDistance			= pSettings->ReadFLOAT	("alife","online_distance");
	m_dwSwitchDelay				= pSettings->ReadINT	("alife","switch_delay");

	FILE_NAME					caFileName;
	CStream						*tpStream;
	if (!Engine.FS.Exist(caFileName,SAVE_PATH,SAVE_NAME)) {
		R_ASSERT				(Engine.FS.Exist(caFileName, ::Path.GameData, SPAWN_NAME));
		tpStream				= Engine.FS.Open(caFileName);
		Log						("* Loading spawn registry");
		CALifeSpawnRegistry::Load(*tpStream);
		Engine.FS.Close			(tpStream);
		vfNewGame				();
		Save					();
		R_ASSERT				(false);
	}
	tpStream					= Engine.FS.Open(caFileName);
	R_ASSERT					(tpStream);
	Log							("* Loading ALife Simulator...");
	CALifeHeader::Load			(*tpStream);
	CALifeGameTime::Load		(*tpStream);
	Log							("* Loading objects...");
	CALifeObjectRegistry::Load	(*tpStream);
	Log							("* Loading events...");
	CALifeEventRegistry::Load	(*tpStream);
	Log							("* Loading tasks...");
	CALifeTaskRegistry::Load	(*tpStream);
	Log							("* Loading static data objects and building dynamic data objects...");
	strconcat					(caFileName,::Path.Current,CROSS_TABLE_NAME);
	CALifeCrossTable::Load		(caFileName);
	vfUpdateDynamicData			();
	Log							("* Loading evaluation functions...");
	m_tpAI_DDD					= xr_new<CAI_DDD>();
	m_tpAI_DDD->vfLoad			();
	m_bLoaded					= true;
	Msg							("* Loading ALife Simulator is successfully completed (%7.3f Mb)",float(Memory.mem_usage() - dwMemUsage)/1048576.0);
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
			Memory.mem_copy				(tpTask->m_tLocationID,m_tpaGraph[tpALifeItem->m_tGraphID].tVertexTypes,LOCATION_TYPE_COUNT*sizeof(_LOCATION_ID));
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