////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"

CSE_ALifeSimulator::CSE_ALifeSimulator(xrServer *tpServer)
{
	m_tpServer			= tpServer;
	m_bLoaded			= false;
	m_tpActor			= 0;
}

CSE_ALifeSimulator::~CSE_ALifeSimulator()
{
	shedule_Unregister	();
}


float CSE_ALifeSimulator::shedule_Scale()
{
	return(.5f); // (schedule_min + schedule_max)*0.5f
}

BOOL CSE_ALifeSimulator::Ready()
{
	return(TRUE);
}

LPCSTR CSE_ALifeSimulator::cName()
{
	return("ALife Simulator");
}; 

void CSE_ALifeSimulator::Save(LPCSTR caSaveName)
{
	CMemoryWriter				tStream;
	CSE_ALifeHeader::Save		(tStream);
	CSE_ALifeGameTime::Save		(tStream);
	CSE_ALifeObjectRegistry::Save(tStream);
	CSE_ALifeEventRegistry::Save(tStream);
	CSE_ALifeTaskRegistry::Save	(tStream);
	CSE_ALifeAnomalyRegistry::Save(tStream);
	CSE_ALifeOrganizationRegistry::Save(tStream);
	string256					S;
	strconcat					(S,SAVE_PATH,caSaveName);
	tStream.save_to				(S);
}

void CSE_ALifeSimulator::Load(LPCSTR caSaveName)
{
	Memory.mem_compact			();
	u32							dwMemUsage = Memory.mem_usage();
	pApp->LoadTitle				("Loading a-life simulator...");
	m_tALifeVersion				= ALIFE_VERSION;
	m_tpActor					= 0;
	m_tGameTime					= 0;
	m_dwObjectsBeingProcessed	= 0;
	m_dwObjectsBeingSwitched	= 0;
	m_bActorEnabled				= true;
	
	Log							("* Loading parameters...");
	shedule.t_min				= pSettings->r_s32	("alife","schedule_min");
	shedule.t_max				= pSettings->r_s32	("alife","schedule_max");
	m_qwMaxProcessTime			= pSettings->r_s32	("alife","process_time")*CPU::cycles_per_microsec;
	m_fOnlineDistance			= pSettings->r_float("alife","online_distance");
	m_dwSwitchDelay				= pSettings->r_s32	("alife","switch_delay");
	m_fTimeFactor				= pSettings->r_float("alife","time_factor");

	string256					caFileName;
	IReader						*tpStream;
	
	// loading spawn registry
	R_ASSERT2					(FS.exist(caFileName, "$game_data$", SPAWN_NAME),"Can't find file 'game.spawn'");
	tpStream					= FS.r_open(caFileName);
	Log							("* Loading spawn registry");
	CSE_ALifeSpawnRegistry::Load(*tpStream);
	FS.r_close					(tpStream);

	// loading game
	if (!FS.exist(caFileName,"$game_saves$",caSaveName))
		// creating new game
		vfNewGame				();
	
	tpStream					= FS.r_open(caFileName);
	if (!tpStream) {
		string4096				S;
		sprintf					(S,"Can't open saved game file '%s'",caSaveName);
		R_ASSERT2				(false,S);
	}
	Log							("* Loading simulator...");
	CSE_ALifeHeader::Load		(*tpStream);
	CSE_ALifeGameTime::Load		(*tpStream);
	Log							("* Loading objects...");
	CSE_ALifeObjectRegistry::Load(*tpStream);
	Log							("* Loading events...");
	CSE_ALifeEventRegistry::Load(*tpStream);
	Log							("* Loading tasks...");
	CSE_ALifeTaskRegistry::Load	(*tpStream);
	Log							("* Loading anomly map...");
	CSE_ALifeAnomalyRegistry::Load(*tpStream);
	Log							("* Loading organizations and discoveries...");
	CSE_ALifeOrganizationRegistry::Load(*tpStream);
	Log							("* Building dynamic objects...");
	vfUpdateDynamicData			();
	m_tpChildren.reserve		(128);
	m_bLoaded					= true;
	Msg							("* Loading ALife Simulator is successfully completed (%7.3f Mb)",float(Memory.mem_usage() - dwMemUsage)/1048576.0);
}

void CSE_ALifeSimulator::vfUpdateDynamicData(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	CSE_ALifeGraphRegistry::Update		(tpALifeDynamicObject);
	CSE_ALifeTraderRegistry::Update		(tpALifeDynamicObject);
	CSE_ALifeScheduleRegistry::Update	(tpALifeDynamicObject);
}

void CSE_ALifeSimulator::vfUpdateDynamicData()
{
	// initialize
	CSE_ALifeGraphRegistry::Init	();
	CSE_ALifeTraderRegistry::Init	();
	CSE_ALifeScheduleRegistry::Init	();
	// update objects
	{
		OBJECT_PAIR_IT			I = m_tObjectRegistry.begin();
		OBJECT_PAIR_IT			E = m_tObjectRegistry.end();
		for ( ; I != E; I++)
			vfUpdateDynamicData	((*I).second);
	}
	// update events
	{
		EVENT_PAIR_IT			I = m_tEventRegistry.begin();
		EVENT_PAIR_IT			E = m_tEventRegistry.end();
		for ( ; I != E; I++)
			vfAddEventToGraphPoint((*I).second,(*I).second->m_tGraphID);
	}
}

void CSE_ALifeSimulator::vfCreateNewTask(CSE_ALifeTrader *tpTrader)
{
	OBJECT_PAIR_IT						I = m_tObjectRegistry.begin();
	OBJECT_PAIR_IT						E = m_tObjectRegistry.end();
	for ( ; I != E; I++) {
		CSE_ALifeItem *tpALifeItem = dynamic_cast<CSE_ALifeItem *>((*I).second);
		if (tpALifeItem && !tpALifeItem->bfAttached()) {
			CSE_ALifeTask				*tpTask = xr_new<CSE_ALifeTask>();
			tpTask->m_tCustomerID		= tpTrader->ID;
			Memory.mem_copy				(tpTask->m_tLocationID,getAI().m_tpaGraph[tpALifeItem->m_tGraphID].tVertexTypes,LOCATION_TYPE_COUNT*sizeof(_LOCATION_ID));
			tpTask->m_tObjectID			= tpALifeItem->ID;
			tpTask->m_tTimeID			= tfGetGameTime();
			tpTask->m_tTaskType			= eTaskTypeSearchForItemOL;
			CSE_ALifeTaskRegistry::Add	(tpTask);
			tpTrader->m_tpTaskIDs.push_back(tpTask->m_tTaskID);
			break;
		}
	}
}

CSE_ALifeTrader *CSE_ALifeSimulator::tpfGetNearestSuitableTrader(CSE_ALifeHumanAbstract *tpALifeHuman)
{
	float			fBestDistance = MAX_NODE_ESTIMATION_COST;
	CSE_ALifeTrader *tpBestTrader = 0;
	TRADER_P_IT		I = m_tpTraders.begin();
	TRADER_P_IT		E = m_tpTraders.end();
	Fvector			&tGlobalPoint = getAI().m_tpaGraph[tpALifeHuman->m_tGraphID].tGlobalPoint;
	for ( ; I != E; I++) {
		if ((*I)->m_tRank != tpALifeHuman->m_tRank)
			break;
		float		fCurDistance = getAI().m_tpaGraph[(*I)->m_tGraphID].tGlobalPoint.distance_to(tGlobalPoint);
		if (fCurDistance < fBestDistance) {
			fBestDistance = fCurDistance;
			tpBestTrader = *I;
		}
	}
	return			(tpBestTrader);
}

void CSE_ALifeSimulator::vfRemoveObject(CSE_Abstract *tpSE_Abstract)
{
	CSE_ALifeDynamicObject			*tpALifeDynamicObject = m_tObjectRegistry[tpSE_Abstract->ID];
	VERIFY						(tpALifeDynamicObject);
	m_tObjectRegistry.erase		(tpSE_Abstract->ID);
	
	vfRemoveObjectFromGraphPoint(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);

	{
		bool					bOk = false;
		ALIFE_ENTITY_P_IT		B = m_tpCurrentLevel->begin(), I = B;
		ALIFE_ENTITY_P_IT		E = m_tpCurrentLevel->end();
		for ( ; I != E; I++)
			if (*I == tpALifeDynamicObject) {
				if (I - B >= (int)m_dwObjectsBeingSwitched) {
					if (m_dwObjectsBeingSwitched)
						m_dwObjectsBeingSwitched--;
				}
				m_tpCurrentLevel->erase(I);
				bOk				= true;
				break;
			}
		VERIFY					(bOk);
	}

	{
		bool					bOk = false;
		ALIFE_MONSTER_P_IT		I = m_tpScheduledObjects.begin();
		ALIFE_MONSTER_P_IT		E = m_tpScheduledObjects.end();
		for ( ; I != E; I++)
			if (*I == tpALifeDynamicObject) {
				m_tpScheduledObjects.erase(I);
				bOk				= true;
				break;
			}
	}

	tpSE_Abstract->m_bALifeControl = false;
}
