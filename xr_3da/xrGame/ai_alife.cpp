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


void CSE_ALifeSimulator::vfNewGame()
{
	CSE_ALifeGraphRegistry::Init	();
	CSE_ALifeTraderRegistry::Init	();
	CSE_ALifeScheduleRegistry::Init();

	u16							l_wGenID = 0x8000;
	ALIFE_ENTITY_P_IT			B = m_tpSpawnPoints.begin();
	ALIFE_ENTITY_P_IT			E = m_tpSpawnPoints.end();
	for (ALIFE_ENTITY_P_IT I = B ; I != E; ) {
		u32						wGroupID	= (*I)->m_dwSpawnGroup;
		float					fSum		= (*I)->m_ucProbability;
		
		for (ALIFE_ENTITY_P_IT j= I + 1; (j != E) && ((*j)->m_dwSpawnGroup == wGroupID); j++)
			fSum += (*j)->m_ucProbability;

		float					fProbability = ::Random.randF(0,fSum);
		fSum					= (*I)->m_ucProbability;
		ALIFE_ENTITY_P_IT		m = j;
		ALIFE_ENTITY_P_IT		k = I;

		for ( j= I + 1; (j != E) && ((*j)->m_dwSpawnGroup == wGroupID); j++) {
			fSum += (*j)->m_ucProbability;
			if (fSum > fProbability) {
				k = j;
				break;
			}
		}
		
		CSE_Abstract			*tpServerEntity = F_entity_Create	((*I)->s_name);
		R_ASSERT2				(tpServerEntity,"Can't create entity.");
		CSE_ALifeDynamicObject		*i = dynamic_cast<CSE_ALifeDynamicObject*>(tpServerEntity);
		R_ASSERT2				(i,"Non-ALife object in the 'game.spawn'");
		
		NET_Packet				tNetPacket;
		(*I)->Spawn_Write		(tNetPacket,TRUE);
		i->Spawn_Read			(tNetPacket);
		tNetPacket.w_begin		(M_UPDATE);
		(*I)->UPDATE_Write		(tNetPacket);
		u16						id;
		tNetPacket.r_begin		(id);
		i->UPDATE_Read			(tNetPacket);
		i->ID					= l_wGenID++;
		CSE_ALifeAbstractGroup		*tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(i);
		if (tpALifeAbstractGroup) {
			i->ID				= m_tpServer->PerformIDgen(l_wGenID++);
			i->m_tObjectID		= i->ID;
			m_tObjectRegistry.insert(std::make_pair(i->m_tObjectID,i));
			
			tpALifeAbstractGroup->m_tpMembers.resize(tpALifeAbstractGroup->m_wCount);
			
			OBJECT_IT			II = tpALifeAbstractGroup->m_tpMembers.begin();
			OBJECT_IT			EE = tpALifeAbstractGroup->m_tpMembers.end();
			for ( ; II != EE; II++) {
				NET_Packet			tNetPacket;
				LPCSTR				S = pSettings->r_string((*I)->s_name,"monster_section");
				CSE_Abstract		*tp1 = F_entity_Create	(S);
				R_ASSERT2			(tp1,"Can't create entity.");
				CSE_ALifeDynamicObject	*tp2 = dynamic_cast<CSE_ALifeDynamicObject*>(tp1);
				R_ASSERT2			(tp2,"Non-ALife object in the 'game.spawn'");
				
				(*I)->Spawn_Write	(tNetPacket,TRUE);
				tp2->Spawn_Read		(tNetPacket);
				tNetPacket.w_begin	(M_UPDATE);
				(*I)->UPDATE_Write	(tNetPacket);
				u16					id;
				tNetPacket.r_begin	(id);
				tp2->UPDATE_Read	(tNetPacket);
				Memory.mem_copy		(tp2->s_name,S,((int)strlen(S) + 1)*sizeof(char));
				tp2->m_bDirectControl	= false;
				tp2->ID				= l_wGenID++;
				vfCreateObject		(tp2);
				*II					= tp2->m_tObjectID = tp2->ID;
				m_tObjectRegistry.insert(std::make_pair(tp2->m_tObjectID,tp2));
				CSE_ALifeMonsterAbstract *tp3 = dynamic_cast<CSE_ALifeMonsterAbstract*>(tp2);
				if (tp3) 
					vfAssignGraphPosition(tp3);
			}
			
			CSE_ALifeMonsterAbstract	*tp3 = dynamic_cast<CSE_ALifeMonsterAbstract*>(i);
			
			if (tp3)
				vfAssignGraphPosition(tp3);
		}
		else {
            vfCreateObject			(i);
			i->m_tObjectID			= i->ID;
			m_tObjectRegistry.insert(std::make_pair(i->m_tObjectID,i));
			CSE_ALifeMonsterAbstract	*tp3 = dynamic_cast<CSE_ALifeMonsterAbstract*>(i);
			if (tp3)
				vfAssignGraphPosition(tp3);
		}
		I							= m;
	}
	m_tALifeVersion					= ALIFE_VERSION;
	m_tGameTime						= u64(m_dwStartTime = Device.dwTimeGlobal);
}

void CSE_ALifeSimulator::Save(LPCSTR caSaveName)
{
	CMemoryWriter				tStream;
	CSE_ALifeHeader::Save			(tStream);
	CSE_ALifeGameTime::Save		(tStream);
	CSE_ALifeObjectRegistry::Save	(tStream);
	CSE_ALifeEventRegistry::Save	(tStream);
	CSE_ALifeTaskRegistry::Save	(tStream);
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
	
	Log							("* Loading parameters...");
	shedule.t_min				= pSettings->r_s32	("alife","schedule_min");
	shedule.t_max				= pSettings->r_s32	("alife","schedule_max");
	m_qwMaxProcessTime			= pSettings->r_s32	("alife","process_time")*CPU::cycles_per_microsec;
	m_fOnlineDistance			= pSettings->r_float("alife","online_distance");
	m_dwSwitchDelay				= pSettings->r_s32	("alife","switch_delay");
	m_fTimeFactor				= pSettings->r_float("alife","time_factor");

	string256					caFileName;
	IReader						*tpStream;
	if (!FS.exist(SAVE_NAME)) {
		R_ASSERT2				(FS.exist(caFileName, "$game_data$", SPAWN_NAME),"Can't find file 'game.spawn'");
		tpStream				= FS.r_open(caFileName);
		Log						("* Loading spawn registry");
		CSE_ALifeSpawnRegistry::Load(*tpStream);
		FS.r_close				(tpStream);
		vfNewGame				();
		Save					();
		R_ASSERT2				(false,"New game has been generated successfully.\nYou have to restart game");
	}
	tpStream					= FS.r_open(caSaveName);
	if (!tpStream) {
		string4096				S;
		sprintf					(S,"Can't open saved game file '%s'",caSaveName);
		R_ASSERT2				(false,S);
	}
	Log							("* Loading simulator...");
	CSE_ALifeHeader::Load			(*tpStream);
	CSE_ALifeGameTime::Load		(*tpStream);
	Log							("* Loading objects...");
	CSE_ALifeObjectRegistry::Load	(*tpStream);
	Log							("* Loading events...");
	CSE_ALifeEventRegistry::Load	(*tpStream);
	Log							("* Loading tasks...");
	CSE_ALifeTaskRegistry::Load	(*tpStream);
	Log							("* Building dynamic objects...");
	vfUpdateDynamicData			();
	m_tpChildren.reserve		(128);
	m_bLoaded					= true;
	Msg							("* Loading ALife Simulator is successfully completed (%7.3f Mb)",float(Memory.mem_usage() - dwMemUsage)/1048576.0);
}

void CSE_ALifeSimulator::vfUpdateDynamicData(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	CSE_ALifeGraphRegistry::Update		(tpALifeDynamicObject);
	CSE_ALifeTraderRegistry::Update	(tpALifeDynamicObject);
	CSE_ALifeScheduleRegistry::Update	(tpALifeDynamicObject);
}

void CSE_ALifeSimulator::vfUpdateDynamicData()
{
	// initialize
	CSE_ALifeGraphRegistry::Init	();
	CSE_ALifeTraderRegistry::Init	();
	CSE_ALifeScheduleRegistry::Init();
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
			CSE_ALifeTask					*tpTask = xr_new<CSE_ALifeTask>();
			tpTask->m_tCustomerID		= tpTrader->m_tObjectID;
			Memory.mem_copy				(tpTask->m_tLocationID,getAI().m_tpaGraph[tpALifeItem->m_tGraphID].tVertexTypes,LOCATION_TYPE_COUNT*sizeof(_LOCATION_ID));
			tpTask->m_tObjectID			= tpALifeItem->m_tObjectID;
			tpTask->m_tTimeID			= tfGetGameTime();
			tpTask->m_tTaskType			= eTaskTypeSearchForItemOL;
			CSE_ALifeTaskRegistry::Add		(tpTask);
			tpTrader->m_tpTaskIDs.push_back(tpTask->m_tTaskID);
			break;
		}
	}
}

CSE_ALifeTrader* CSE_ALifeSimulator::tpfGetNearestSuitableTrader(CSE_ALifeHumanAbstract *tpALifeHuman)
{
	float			fBestDistance = MAX_NODE_ESTIMATION_COST;
	CSE_ALifeTrader *	tpBestTrader = 0;
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

void CSE_ALifeSimulator::vfRemoveObject(CSE_Abstract *tpServerEntity)
{
	CSE_ALifeDynamicObject			*tpALifeDynamicObject = m_tObjectRegistry[tpServerEntity->ID];
	VERIFY						(tpALifeDynamicObject);
	m_tObjectRegistry.erase		(tpServerEntity->ID);
	
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

	tpServerEntity->m_bALifeControl = false;
}


void CSE_ALifeSimulator::vfGenerateAnomalousZones()
{
}

void CSE_ALifeSimulator::vfGenerateArtefacts()
{
}

void CSE_ALifeSimulator::vfSellArtefacts(CSE_ALifeTrader &tTrader)
{
}

void CSE_ALifeSimulator::vfUpdateArtefactOrders(CSE_ALifeTrader &tTrader)
{
}

void CSE_ALifeSimulator::vfGiveMilitariesBribe(CSE_ALifeTrader &tTrader)
{
}

void CSE_ALifeSimulator::vfBuySupplies(CSE_ALifeTrader &tTrader)
{
}

void CSE_ALifeSimulator::vfAssignPrices(CSE_ALifeTrader &tTrader)
{
}

void CSE_ALifeSimulator::vfBallanceCreatures()
{
}

void CSE_ALifeSimulator::vfUpdateCreatures()
{
}