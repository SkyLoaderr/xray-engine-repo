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

CAI_ALife::CAI_ALife(xrServer *tpServer)
{
	m_tpServer			= tpServer;
	m_bLoaded			= false;
	m_tpActor			= 0;
}

CAI_ALife::~CAI_ALife()
{
	shedule_Unregister	();
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
	u16							l_wGenID = 0x8000;
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
		CALifeDynamicObject		*i = dynamic_cast<CALifeDynamicObject*>(tpServerEntity);
		R_ASSERT				(i);
		NET_Packet				tNetPacket;
		(*I)->Spawn_Write		(tNetPacket,TRUE);
		i->Spawn_Read			(tNetPacket);
		tNetPacket.w_begin		(M_UPDATE);
		(*I)->UPDATE_Write		(tNetPacket);
		u16						id;
		tNetPacket.r_begin		(id);
		i->UPDATE_Read			(tNetPacket);
		i->ID					= l_wGenID++;
		CALifeAbstractGroup		*tpALifeAbstractGroup = dynamic_cast<CALifeAbstractGroup*>(i);
		if (tpALifeAbstractGroup) {
			i->ID				= m_tpServer->PerformIDgen(l_wGenID++);
			i->m_tObjectID		= i->ID;
			m_tObjectRegistry.insert(make_pair(i->m_tObjectID,i));
			
			tpALifeAbstractGroup->m_tpMembers.resize(tpALifeAbstractGroup->m_wCount);
			OBJECT_IT			II = tpALifeAbstractGroup->m_tpMembers.begin();
			OBJECT_IT			EE = tpALifeAbstractGroup->m_tpMembers.end();
			for ( ; II != EE; II++) {
				NET_Packet			tNetPacket;
				LPCSTR				S = pSettings->r_string((*I)->s_name,"monster_section");
				xrServerEntity		*tp1 = F_entity_Create	(S);
				R_ASSERT2			(tp1,"Can't create entity.");
				CALifeDynamicObject	*tp2 = dynamic_cast<CALifeDynamicObject*>(tp1);
				R_ASSERT			(tp2);
				(*I)->Spawn_Write	(tNetPacket,TRUE);
				tp2->Spawn_Read		(tNetPacket);
				tNetPacket.w_begin	(M_UPDATE);
				(*I)->UPDATE_Write	(tNetPacket);
				u16					id;
				tNetPacket.r_begin	(id);
				tp2->UPDATE_Read	(tNetPacket);
				Memory.mem_copy		(tp2->s_name,S,(strlen(S) + 1)*sizeof(char));
				tp2->m_bDirectControl	= false;
				tp2->ID				= l_wGenID++;
				vfCreateObject		(tp2);
				*II					= tp2->m_tObjectID = tp2->ID;
				m_tObjectRegistry.insert(make_pair(tp2->m_tObjectID,tp2));
				CALifeMonsterAbstract *tp3 = dynamic_cast<CALifeMonsterAbstract*>(tp2);
				if (tp3) 
					vfAssignGraphPosition(tp3);
			}
			CALifeMonsterAbstract *tp3 = dynamic_cast<CALifeMonsterAbstract*>(i);
			if (tp3)
				vfAssignGraphPosition(tp3);
		}
		else {
            vfCreateObject		(i);
			i->m_tObjectID		= i->ID;
			m_tObjectRegistry.insert(make_pair(i->m_tObjectID,i));
			CALifeMonsterAbstract *tp3 = dynamic_cast<CALifeMonsterAbstract*>(i);
			if (tp3)
				vfAssignGraphPosition(tp3);
		}
		I						= m;
	}
	m_tALifeVersion				= ALIFE_VERSION;
	m_tGameTime					= u64(m_dwStartTime = Device.dwTimeGlobal);
}

void CAI_ALife::Save()
{
	CMemoryWriter				tStream;
	CALifeHeader::Save			(tStream);
	CALifeGameTime::Save		(tStream);
	CALifeObjectRegistry::Save	(tStream);
	CALifeEventRegistry::Save	(tStream);
	CALifeTaskRegistry::Save	(tStream);
	string256					S;
	strconcat					(S,SAVE_PATH,SAVE_NAME);
	tStream.save_to				(S);
}

void CAI_ALife::Load()
{
	Memory.mem_compact			();
	u32							dwMemUsage = Memory.mem_usage();
	pApp->LoadTitle				("Loading a-life simulator...");
	m_tALifeVersion				= ALIFE_VERSION;
	m_tpActor					= 0;
	m_tGameTime					= 0;
	m_dwObjectsBeingProcessed	= 0;
	m_dwObjectsBeingSwitched	= 0;
	
	Log							("* Loading parameters...");
	shedule_Min					= pSettings->r_s32	("alife","schedule_min");
	shedule_Max					= pSettings->r_s32	("alife","schedule_max");
	m_qwMaxProcessTime			= pSettings->r_s32	("alife","process_time")*CPU::cycles_per_microsec;
	m_fOnlineDistance			= pSettings->r_float("alife","online_distance");
	m_dwSwitchDelay				= pSettings->r_s32	("alife","switch_delay");
	m_fTimeFactor				= pSettings->r_float("alife","time_factor");

	string256					caFileName;
	IReader						*tpStream;
	if (!FS.exist(SAVE_NAME)) {
		R_ASSERT				(FS.exist(caFileName, "$game_data$", SPAWN_NAME));
		tpStream				= FS.r_open(caFileName);
		Log						("* Loading spawn registry");
		CALifeSpawnRegistry::Load(*tpStream);
		FS.r_close			(tpStream);
		vfNewGame				();
		Save					();
		R_ASSERT2				(false,"New game has been generated successfully.\nYou have to restart game");
	}
	tpStream					= FS.r_open(SAVE_NAME);
	R_ASSERT					(tpStream);
	Log							("* Loading simulator...");
	CALifeHeader::Load			(*tpStream);
	CALifeGameTime::Load		(*tpStream);
	Log							("* Loading objects...");
	CALifeObjectRegistry::Load	(*tpStream);
	Log							("* Loading events...");
	CALifeEventRegistry::Load	(*tpStream);
	Log							("* Loading tasks...");
	CALifeTaskRegistry::Load	(*tpStream);
	Log							("* Building dynamic objects...");
	vfUpdateDynamicData			();
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

void CAI_ALife::vfCreateNewTask(CALifeTrader *tpTrader)
{
	OBJECT_PAIR_IT	I = m_tObjectRegistry.begin();
	OBJECT_PAIR_IT	E = m_tObjectRegistry.end();
	for ( ; I != E; I++) {
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>((*I).second);
		if (tpALifeItem && !tpALifeItem->bfAttached()) {
			CALifeTask					*tpTask = xr_new<CALifeTask>();
			tpTask->m_tCustomerID		= tpTrader->m_tObjectID;
			Memory.mem_copy				(tpTask->m_tLocationID,getAI().m_tpaGraph[tpALifeItem->m_tGraphID].tVertexTypes,LOCATION_TYPE_COUNT*sizeof(_LOCATION_ID));
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
	Fvector			&tGlobalPoint = getAI().m_tpaGraph[tpALifeHuman->m_tGraphID].tGlobalPoint;
	for ( ; I != E; I++) {
		if ((*I)->m_tRank != tpALifeHuman->m_tRank)
			break;
		float fCurDistance = getAI().m_tpaGraph[(*I)->m_tGraphID].tGlobalPoint.distance_to(tGlobalPoint);
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