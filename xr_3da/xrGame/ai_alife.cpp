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

void CSE_ALifeSimulator::vfCreateObjectFromSpawnPoint(CSE_ALifeDynamicObject *&i, CSE_ALifeDynamicObject *j, _SPAWN_ID tSpawnID)
{
	CSE_Abstract				*tpSE_Abstract = F_entity_Create	(j->s_name);
	R_ASSERT2					(tpSE_Abstract,"Can't create entity.");
	i							= dynamic_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
	R_ASSERT2					(i,"Non-ALife object in the 'game.spawn'");

	NET_Packet					tNetPacket;
	j->Spawn_Write				(tNetPacket,TRUE);
	i->Spawn_Read				(tNetPacket);
	tNetPacket.w_begin			(M_UPDATE);
	j->UPDATE_Write				(tNetPacket);
	u16							id;
	tNetPacket.r_begin			(id);
	i->UPDATE_Read				(tNetPacket);
	i->m_tSpawnID				= tSpawnID;
	i->ID						= m_tpServer->PerformIDgen(0xffff);
	m_tObjectRegistry.insert	(std::make_pair(i->ID,i));
	vfUpdateDynamicData			(i);
	
	CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(i);
	if (l_tpALifeMonsterAbstract)
		vfAssignGraphPosition	(l_tpALifeMonsterAbstract);
	
	CSE_ALifeAbstractGroup		*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(i);
	if (l_tpALifeAbstractGroup) {
		l_tpALifeAbstractGroup->m_tpMembers.resize(l_tpALifeAbstractGroup->m_wCount);
		OBJECT_IT				I = l_tpALifeAbstractGroup->m_tpMembers.begin();
		OBJECT_IT				E = l_tpALifeAbstractGroup->m_tpMembers.end();
		for ( ; I != E; I++) {
			LPCSTR						S = pSettings->r_string(i->s_name,"monster_section");
			CSE_Abstract				*l_tpAbstract = F_entity_Create	(S);
			R_ASSERT2					(l_tpAbstract,"Can't create entity.");
			CSE_ALifeDynamicObject		*k = dynamic_cast<CSE_ALifeDynamicObject*>(l_tpAbstract);
			R_ASSERT2					(k,"Non-ALife object in the 'game.spawn'");

			j->Spawn_Write				(tNetPacket,TRUE);
			k->Spawn_Read				(tNetPacket);
			tNetPacket.w_begin			(M_UPDATE);
			j->UPDATE_Write				(tNetPacket);
			u16							id;
			tNetPacket.r_begin			(id);
			k->UPDATE_Read				(tNetPacket);
			Memory.mem_copy				(k->s_name,S,((int)strlen(S) + 1)*sizeof(char));
			k->m_tSpawnID				= tSpawnID;
			k->ID						= m_tpServer->PerformIDgen(0xffff);
			*I							= k->ID;
			k->m_bDirectControl			= false;
			m_tObjectRegistry.insert	(std::make_pair(k->ID,k));
			vfUpdateDynamicData			(k);
		}
	}
}

void CSE_ALifeSimulator::vfGenerateAnomalousZones()
{
	// deactivating all the anomalous zones
	OBJECT_PAIR_IT				B = m_tObjectRegistry.begin(), I = B, J;
	OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
	for ( ; I != E; ) {
		CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>((*I).second);
		if (l_tpALifeAnomalousZone)
			l_tpALifeAnomalousZone->m_maxPower = 0.f;
	}
	// for each spawn group activate a zone if any
	for (I = B; I != E; ) {
		CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>((*I).second);
		if (!l_tpALifeAnomalousZone)
			continue;

		// counting zones with the same group ID
		// !
		// spawn points are sorted according to their spawn group number in ascending order
		// this condition _must_ guarantee xrAI since it sorts spawn points during 'game.spawn' generation
		// we assume that zones in the spawn groups are generated in a row 
		// therefore they _must_ have their IDs in a row, since we starts their ID generation with a fullfilled
		// ID deque id_free in the class xrServer
		// here we use this information by iterating on vector m_tpSpawnPoints and map m_tObjectRegistry 
		// (because this map is ordered by ID in ascending order)
		// if this condition is _not_ guaranteed we have to rewrite this piece of code
		// !
		float					fSum = 0;
		ALIFE_ENTITY_P_IT		i = m_tpSpawnPoints.begin() + l_tpALifeAnomalousZone->m_tSpawnID, j = i, e = m_tpSpawnPoints.end(), b = m_tpSpawnPoints.begin();
		u32						l_dwGroupID = (*i)->m_dwSpawnGroup;
		for ( ; j != e; j++)
			if ((*j)->m_dwSpawnGroup != l_dwGroupID)
				break;
			else
				fSum += (*j)->m_fProbability;
		R_ASSERT2				(fSum > 1 + EPS_L,"Group probability more than 1!");
		
		// computing probability of the anomalous zone activation
		float					fProbability = randF(1.f);
		fSum					= 0.f;
		J						= I;
		ALIFE_ENTITY_P_IT		m = j;
		for ( j = i; (j != e) && ((*j)->m_dwSpawnGroup == l_dwGroupID); j++, I++) {
			fSum += (*j)->m_fProbability;
			if (fSum > fProbability)
				break;
		}
		
		// if random choosed a number due to which there is no active zones in the current group ID
		// then continue loop
		if (fSum <= fProbability)
			continue;
		
		// otherwise assign random anomaly power to the zone
		l_tpALifeAnomalousZone	= dynamic_cast<CSE_ALifeAnomalousZone*>((*I).second);
		R_ASSERT2				(l_tpALifeAnomalousZone,"Anomalous zones are grouped with incompatible objects!");
		CSE_ALifeAnomalousZone	*l_tpSpawnAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(*j);
		R_ASSERT2				(l_tpSpawnAnomalousZone,"Anomalous zones are grouped with incompatible objects!");
		l_tpALifeAnomalousZone->m_maxPower = randF(.5f*l_tpSpawnAnomalousZone->m_maxPower,1.5f*l_tpSpawnAnomalousZone->m_maxPower);
		
		// proceed random artefacts generation for the active zone
		fProbability			= randF(1.f);
		fSum					= 0;
		for (u32 ii=0, jj=iFloor(l_tpALifeAnomalousZone->m_maxPower*10/3); ii<jj; ii++) {
			for (u16 p=0; p<l_tpSpawnAnomalousZone->m_wItemCount; p++) {
				fSum			+= l_tpSpawnAnomalousZone->m_faWeights[p];
				if (fSum > fProbability)
					break;
			}
			if (p < l_tpSpawnAnomalousZone->m_wItemCount) {
				CSE_Abstract	*l_tpSE_Abstract = F_entity_Create	(l_tpSpawnAnomalousZone->m_cppArtefactSections[p]);
				R_ASSERT2		(l_tpSE_Abstract,"Can't create entity.");
				CSE_ALifeDynamicObject	*i = dynamic_cast<CSE_ALifeDynamicObject*>(l_tpSE_Abstract);
				R_ASSERT2		(i,"Non-ALife object in the 'game.spawn'");

				i->ID			= m_tpServer->PerformIDgen(0xffff);
				i->m_tSpawnID	= _SPAWN_ID(j - b);
				i->m_tGraphID	= l_tpSpawnAnomalousZone->m_tGraphID;
				u32				l_dwIndex = l_tpSpawnAnomalousZone->m_dwStartIndex + randI(l_tpSpawnAnomalousZone->m_wArtefactSpawnCount);
				i->o_Position	= m_tpArtefactSpawnPositions[l_dwIndex].tPoint;
				i->m_tNodeID	= m_tpArtefactSpawnPositions[l_dwIndex].tNodeID;
				i->m_fDistance	= m_tpArtefactSpawnPositions[l_dwIndex].fDistance;
				
				CSE_ALifeItemArtefact *l_tpALifeItemArtefact = dynamic_cast<CSE_ALifeItemArtefact*>(i);
				R_ASSERT2		(l_tpALifeItemArtefact,"Anomalous zone can't generate non-artefact objects since they don't have an 'anomaly property'!");

				l_tpALifeItemArtefact->m_fAnomalyValue = l_tpALifeAnomalousZone->m_maxPower*(1.f - i->o_Position.distance_to(l_tpSpawnAnomalousZone->o_Position)/l_tpSpawnAnomalousZone->m_fRadius);

				m_tObjectRegistry.insert(std::make_pair(i->ID,i));
				vfUpdateDynamicData(i);
			}
		}
	}
}

void CSE_ALifeSimulator::vfGenerateAnomalyMap()
{
	m_tpAnomalies.resize		(getAI().GraphHeader().dwVertexCount);
	OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
	OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
	for ( ; I != E; I++) {
		CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>((*I).second);
		if (!l_tpALifeAnomalousZone || !randI(20))
			continue;
		
		CSE_ALifeKnownAnomaly *l_tpALifeKnownAnomaly	= xr_new<CSE_ALifeKnownAnomaly>();
		l_tpALifeKnownAnomaly->m_tAnomalousZoneType		= randI(10) ? l_tpALifeAnomalousZone->m_tAnomalyType : EAnomalousZoneType(randI(eAnomalousZoneTypeDummy));
		l_tpALifeKnownAnomaly->m_fAnomalyPower			= randF(l_tpALifeAnomalousZone->m_maxPower*.5f,l_tpALifeAnomalousZone->m_maxPower*1.5f);
		l_tpALifeKnownAnomaly->m_fDistance				= randF(l_tpALifeAnomalousZone->m_fDistance*.5f,l_tpALifeAnomalousZone->m_fDistance*1.5f);
		m_tpAnomalies[l_tpALifeAnomalousZone->m_tGraphID].push_back(l_tpALifeKnownAnomaly);
	}
}

void CSE_ALifeSimulator::vfNewGame()
{
	CSE_ALifeGraphRegistry::Init	();
	CSE_ALifeTraderRegistry::Init	();
	CSE_ALifeScheduleRegistry::Init	();
	
	ALIFE_ENTITY_P_IT			B = m_tpSpawnPoints.begin();
	ALIFE_ENTITY_P_IT			E = m_tpSpawnPoints.end();
	for (ALIFE_ENTITY_P_IT I = B ; I != E; ) {
		u32						l_dwGroupID	= (*I)->m_dwSpawnGroup;
		for (ALIFE_ENTITY_P_IT m = I + 1, j = I; (m != E) && ((*m)->m_dwSpawnGroup == l_dwGroupID); m++) ;

		CSE_Abstract			*tpSE_Abstract = F_entity_Create	((*I)->s_name);
		R_ASSERT2				(tpSE_Abstract,"Can't create entity.");
		
		CSE_ALifeDynamicObject	*i = dynamic_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
		R_ASSERT2				(i,"Non-ALife object in the 'game.spawn'");
		
		CSE_ALifeAnomalousZone	*tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(i);
		if (tpALifeAnomalousZone) {
			for ( ; j != m; j++) {
				CSE_ALifeAnomalousZone	*tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(*j);
				R_ASSERT2				(tpALifeAnomalousZone,"Anomalous zones are grouped with incompatible objects!");
				vfCreateObjectFromSpawnPoint(i,*j,_SPAWN_ID(j - B));
			}
		}
		I = m;
	}
	
	m_tGameTime					= u64(m_dwStartTime = Device.TimerAsync());
	m_tZoneState				= eZoneStateSurge;
	_TIME_ID					l_tFinishTime = m_tGameTime + 3*7*24*3600*1000; // 3 weeks in milliseconds
	float						l_fTimeFactor = m_fTimeFactor;
	m_fTimeFactor				= 100;
	
	// simulating ALife during the given time period
	while (tfGetGameTime() < l_tFinishTime) {
		switch (m_tZoneState) {
			case eZoneStateSurge : {
				vfPerformSurge		();
				m_tLastSurgeTime	= tfGetGameTime();
				m_tNextSurgeTime	= m_tLastSurgeTime + 7*24*3600*1000; // a week in milliseconds
				m_tZoneState		= eZoneStateAfterSurge;
				break;
			}
			case eZoneStateAfterSurge : {
				if (tfGetGameTime() > m_tNextSurgeTime) {
					m_tZoneState	= eZoneStateSurge;
					break;
				}
				
				ALIFE_MONSTER_P_IT	B = m_tpScheduledObjects.begin(), I = B;
				ALIFE_MONSTER_P_IT	E = m_tpScheduledObjects.end();
				for ( ; I != E; I++)
					vfProcessNPC	(*I);
				break;
			}
			default : NODEFAULT;
		}
	}

	m_fTimeFactor				= l_fTimeFactor;
	Save						();
}

void CSE_ALifeSimulator::vfPerformSurge()
{
	vfGenerateAnomalousZones	();
	vfGenerateAnomalyMap		();
	TRADER_P_IT					I = m_tpTraders.begin();
	TRADER_P_IT					E = m_tpTraders.end();
	for ( ; I != E; I++) {
		vfSellArtefacts			(**I);
		vfUpdateArtefactOrders	(**I);
		vfBuySupplies			(**I);
		vfAssignPrices			(**I);
		vfGiveMilitariesBribe	(**I);
	}
	vfUpdateOrganizations		();
	vfKillCreatures				();
	vfBallanceCreatures			();
}

void CSE_ALifeSimulator::vfBallanceCreatures()
{
	// filling array of the survived creatures
	{
		OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
		OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
		for ( ; I != E; I++) {
			CSE_ALifeCreatureAbstract *l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>((*I).second);
			if (!l_tpALifeCreatureAbstract || (l_tpALifeCreatureAbstract->fHealth > 0.f))
				m_baAliveSpawnObjects[(*I).second->m_tSpawnID] = true;
		}
	}
	// balancing creatures by spawn groups
	// i.e. if there is no object being spawned by the particular spawn group
	// then we have to spawn an object from this spawn group
	{
		ALIFE_ENTITY_P_IT			B = m_tpSpawnPoints.begin(), I = B, J;
		ALIFE_ENTITY_P_IT			E = m_tpSpawnPoints.end();
		for ( ; I != E; ) {
			u32						l_dwSpawnGroup = (*I)->m_dwSpawnGroup;
			bool					bOk = false;
			J						= I;
			for ( ; (I != E) && (l_dwSpawnGroup == (*I)->m_dwSpawnGroup); I++)
				if (m_baAliveSpawnObjects[I - B]) {
					bOk = true;
					break;
				}
			if (!bOk) {
				// there is no object being spawned from this spawn group -> spawn it!
				float				l_fProbability = randF(0,1.f), l_fSum = 0.f;
				ALIFE_ENTITY_P_IT	j = J;
				ALIFE_ENTITY_P_IT	e = I;
				for ( ; (j != e); j++) {
					l_fSum			+= (*j)->m_fProbability;
					if (l_fSum > l_fProbability)
						break;
				}
				if (l_fSum > l_fProbability) {
					CSE_ALifeAnomalousZone		*l_tpALifeAnomalousZone		= dynamic_cast<CSE_ALifeAnomalousZone*>(*j);
					if (l_tpALifeAnomalousZone)
						continue;

					CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract	= dynamic_cast<CSE_ALifeCreatureAbstract*>(*j);
					if (l_tpALifeCreatureAbstract)
						continue;

					CSE_ALifeDynamicObject		*l_tpALifeDynamicObject;
					vfCreateObjectFromSpawnPoint(l_tpALifeDynamicObject,*j,_SPAWN_ID(j - B));
				}
			}
		}
	}
	
	// initialize array
	m_baAliveSpawnObjects.assign(m_baAliveSpawnObjects.size(),false);
}

void CSE_ALifeSimulator::vfKillCreatures()
{
	OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
	OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
	for ( ; I != E; I++) {
		CSE_ALifeCreatureAbstract *l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>((*I).second);
		if (l_tpALifeCreatureAbstract && (l_tpALifeCreatureAbstract->m_bDirectControl) && (l_tpALifeCreatureAbstract->fHealth > 0.f)) {
			CSE_ALifeAbstractGroup *l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>((*I).second);
			getAI().m_tpCurrentALifeObject = (*I).second;
			if (l_tpALifeAbstractGroup) {
				_GRAPH_ID			l_tGraphID = l_tpALifeCreatureAbstract->m_tGraphID;
				for (u32 i=0, N = (u32)l_tpALifeAbstractGroup->m_tpMembers.size(); i<N; i++) {
					OBJECT_PAIR_IT				J = m_tObjectRegistry.find(l_tpALifeAbstractGroup->m_tpMembers[i]);
					R_ASSERT2					(J != m_tObjectRegistry.end(),"There is no object being attached as a group member!");
					CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>((*J).second);
					R_ASSERT2					(l_tpALifeCreatureAbstract,"Group class differs from the member class!");
					getAI().m_tpCurrentALifeObject = l_tpALifeCreatureAbstract;
					if (randF(100) > getAI().m_pfSurgeDeathProbability->ffGetValue()) {
						
						l_tpALifeCreatureAbstract->m_bDirectControl	= true;
						l_tpALifeCreatureAbstract->fHealth			= 0.f;
						l_tpALifeAbstractGroup->m_tpMembers.erase	(l_tpALifeAbstractGroup->m_tpMembers.begin() + i);

						SLevelPoint*								l_tpaLevelPoints = (SLevelPoint*)(((u8*)getAI().m_tpaGraph) + getAI().m_tpaGraph[l_tGraphID].dwPointOffset);
						u32											l_dwDeathpointIndex = randI(getAI().m_tpaGraph[l_tGraphID].tDeathPointCount);
						l_tpALifeCreatureAbstract->m_tGraphID		= l_tGraphID;
						l_tpALifeCreatureAbstract->o_Position		= l_tpaLevelPoints[l_dwDeathpointIndex].tPoint;
						l_tpALifeCreatureAbstract->m_tNodeID		= l_tpaLevelPoints[l_dwDeathpointIndex].tNodeID;
						l_tpALifeCreatureAbstract->m_fDistance		= l_tpaLevelPoints[l_dwDeathpointIndex].fDistance;

						vfUpdateDynamicData							(l_tpALifeCreatureAbstract);
						i--;
						N--;
					}
				}
			}
			else
				if (randI(100) > getAI().m_pfSurgeDeathProbability->ffGetValue())
					l_tpALifeCreatureAbstract->fHealth = 0.f;
		}
	}
}

void CSE_ALifeSimulator::vfGiveMilitariesBribe(CSE_ALifeTrader &tTrader)
{
	tTrader.m_dwMoney = u32(float(tTrader.m_dwMoney)*.9f);
}

void CSE_ALifeSimulator::vfUpdateOrganizations()
{
	// iterating on all the organizations
	ORGANIZATION_P_PAIR_IT	I = m_tOrganizationRegistry.begin();
	ORGANIZATION_P_PAIR_IT	E = m_tOrganizationRegistry.end();
	for ( ; I != E; I++) {
		switch ((*I).second->m_tResearchState) {
			case eResearchStateLeft : {
				// asking if we have to join zone investigations
				if (randF(1) < (*I).second->m_fJoinProbability)
					(*I).second->m_tResearchState = eResearchStateJoin;
				break;
			}
			case eResearchStateJoin : {
				// asking if we have to left zone investigations
				if (!strlen((*I).second->m_caDiscoveryToInvestigate) && (*I).second->m_tpOrderedArtefacts.empty() && (randF(1) < (*I).second->m_fLeftProbability))
					(*I).second->m_tResearchState = eResearchStateLeft;
				else {
					// selecting discovery we would like to investigate
					CSE_ALifeDiscovery	*l_tpBestDiscovery = 0;
					bool				l_bGoToResearch = false;
					LPSTR_IT			i = (*I).second->m_tpPossibleDiscoveries.begin();
					LPSTR_IT			e = (*I).second->m_tpPossibleDiscoveries.end();
					for ( ; i != e; i++) {
						// getting pointer to discovery object from the discovery registry
						DISCOVERY_P_PAIR_IT			j = m_tDiscoveryRegistry.find(*i);
						R_ASSERT2					(j != m_tDiscoveryRegistry.end(),"Invalid discovery name in the possible iscoveries parameters in the 'system.ltx'!");
						
						// checking if discovery has not been invented yet
						if ((*j).second->m_bAlreadyInvented)
							continue;
						
						// checking if discovery depends on the non-invented discoveries
						LPSTR_IT					II = (*j).second->m_tpDependency.begin();
						LPSTR_IT					EE = (*j).second->m_tpDependency.end();
						bool						l_bFoundDiscovery = true;
						for ( ; II != EE; II++) {
							DISCOVERY_P_PAIR_IT		J = m_tDiscoveryRegistry.find(*II);
							R_ASSERT2				(J != m_tDiscoveryRegistry.end(),"Invalid discovery dependency!");
							if (!(*J).second->m_bAlreadyInvented) {
								l_bFoundDiscovery = false;
								break;
							}
						}
						if (!l_bFoundDiscovery)
							break;
						
						// checking if artefacts being used during discovery invention are known
						// and there are enough artefacts purchased for discovery
						ARTEFACT_ORDER_IT			ii = (*j).second->m_tpArtefactNeed.begin();
						ARTEFACT_ORDER_IT			ee = (*j).second->m_tpArtefactNeed.end();
						bool						l_bIsReadyForInvention = true;
						for ( ; ii != ee; ii++) {
							ARTEFACT_COUNT_PAIR_IT	jj = m_tArtefactRegistry.find((LPSTR)(*ii).m_caSection);
							if (jj == m_tArtefactRegistry.end()) {
								l_bFoundDiscovery	= false;
								break;
							}
							else {
								jj = (*I).second->m_tpPurchasedArtefacts.find((LPSTR)(*ii).m_caSection);
								if ((jj == (*I).second->m_tpPurchasedArtefacts.end()) || ((*jj).second < (*ii).m_dwCount))
									l_bIsReadyForInvention = false;
							}
						}
						if (l_bFoundDiscovery) {
							l_tpBestDiscovery		= (*j).second;
							l_bGoToResearch			= l_bIsReadyForInvention;
						}
					}
					// checking if we did select the discovery
					if (l_tpBestDiscovery) {
						strcpy((*I).second->m_caDiscoveryToInvestigate,  l_tpBestDiscovery->m_caDiscoveryIdentifier);
						// checking if we are ready to invent it
						if (l_bGoToResearch) {
							(*I).second->m_tResearchState = eResearchStateResearch;
							(*I).second->m_tpOrderedArtefacts.clear();
						}
						else {
							// if not - order artefacts needed for the discovery
							(*I).second->m_tpOrderedArtefacts = l_tpBestDiscovery->m_tpArtefactNeed;
						}
					}
					(*I).second->m_tpPurchasedArtefacts.clear();
				}
				break;
			}
			case eResearchStateResearch : {
				// getting pointer to discovery object from the discovery registry
				DISCOVERY_P_PAIR_IT	i = m_tDiscoveryRegistry.find((*I).second->m_caDiscoveryToInvestigate);
				R_ASSERT2(i != m_tDiscoveryRegistry.end(),"Unknown discovery!");
				
				// checking if we've got any result
				if (randF(1) < (*i).second->m_fResultProbability) {
					// checking if we've invented the discovery
					if (randF(1) < (*i).second->m_fSuccessProbability) {
						R_ASSERT2(!(*i).second->m_bAlreadyInvented,"Discovery has been already invented!");
						(*i).second->m_bAlreadyInvented = true;
						(*I).second->m_tResearchState = eResearchStateJoin;
						// ordering artefacts
						DEMAND_P_IT			ii = (*i).second->m_tpArtefactDemand.begin();
						DEMAND_P_IT			ee = (*i).second->m_tpArtefactDemand.end();
						(*I).second->m_tpOrderedArtefacts.resize(ee - ii);
						ARTEFACT_ORDER_IT	II = (*I).second->m_tpOrderedArtefacts.begin();
						for ( ; ii != ee; ii++, II++) {
							strcpy((*II).m_caSection,(*ii)->m_caSection);
							(*II).m_dwCount	= randI((*ii)->m_dwMinArtefactCount,(*ii)->m_dwMaxArtefactCount);
							(*II).m_dwPrice	= randI((*ii)->m_dwMinArtefactPrice,(*ii)->m_dwMaxArtefactPrice);
						}
					}
					else
						// checking if we've destroyed laboratory during investigations
						if (randF(1) < (*i).second->m_fDestroyProbability) {
							(*I).second->m_tResearchState = eResearchStateFreeze;
							(*I).second->m_tpOrderedArtefacts.clear();
						}
						else {
							// otherwise - we finished with investigations without any discovery;
							(*I).second->m_tResearchState = eResearchStateJoin;
						}
				}
				break;
			}
			case eResearchStateFreeze : {
				// getting pointer to discovery object from the discovery registry
				DISCOVERY_P_PAIR_IT	i = m_tDiscoveryRegistry.find((*I).second->m_caDiscoveryToInvestigate);
				R_ASSERT2(i != m_tDiscoveryRegistry.end(),"Unknown discovery!");
				// checking if we've restored
				if (randF(1) < (*i).second->m_fUnfreezeProbability)
					(*I).second->m_tResearchState = eResearchStateJoin;
				break;
			}
			default : NODEFAULT;
		}
	}
}

void CSE_ALifeSimulator::vfSellArtefacts(CSE_ALifeTrader &tTrader)
{
}

void CSE_ALifeSimulator::vfUpdateArtefactOrders(CSE_ALifeTrader &tTrader)
{
}

void CSE_ALifeSimulator::vfBuySupplies(CSE_ALifeTrader &tTrader)
{
}

void CSE_ALifeSimulator::vfAssignPrices(CSE_ALifeTrader &tTrader)
{
}
