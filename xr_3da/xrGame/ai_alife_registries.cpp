////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.cpp
//	Created 	: 15.01.2003
//  Modified 	: 23.06.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_registries.h"
#include "ai_alife_templates.h"
#include "ai_alife_predicates.h"
#include "ai_space.h"

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectRegistry::CSE_ALifeObjectRegistry()
{
	m_tObjectRegistry.clear		();
}

CSE_ALifeObjectRegistry::~CSE_ALifeObjectRegistry()
{
	free_map					(m_tObjectRegistry);
}

void CSE_ALifeObjectRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(OBJECT_CHUNK_DATA);
	tMemoryStream.w_u32			((u32)m_tObjectRegistry.size());
	OBJECT_PAIR_IT I			= m_tObjectRegistry.begin();
	OBJECT_PAIR_IT E			= m_tObjectRegistry.end();
	for ( ; I != E; I++) {
		NET_Packet				tNetPacket;
		// Spawn
		(*I).second->Spawn_Write(tNetPacket,TRUE);
		tMemoryStream.w_u16		(u16(tNetPacket.B.count));
		tMemoryStream.w			(tNetPacket.B.data,tNetPacket.B.count);

		// Update
		tNetPacket.w_begin		(M_UPDATE);
		(*I).second->UPDATE_Write(tNetPacket);

		tMemoryStream.w_u16		(u16(tNetPacket.B.count));
		tMemoryStream.w			(tNetPacket.B.data,tNetPacket.B.count);
	}
	tMemoryStream.close_chunk	();
}

void CSE_ALifeObjectRegistry::Load(IReader &tFileStream)
{ 
	R_ASSERT2					(tFileStream.find_chunk(OBJECT_CHUNK_DATA),"Can't find chunk OBJECT_CHUNK_DATA!");
	m_tObjectRegistry.clear();
	u32 dwCount = tFileStream.r_u32();
	for (u32 i=0; i<dwCount; i++) {
		NET_Packet				tNetPacket;
		u16						u_id;
		// Spawn
		tNetPacket.B.count		= tFileStream.r_u16();
		tFileStream.r			(tNetPacket.B.data,tNetPacket.B.count);
		tNetPacket.r_begin		(u_id);
		R_ASSERT2				(M_SPAWN==u_id,"Invalid packet ID (!= M_SPAWN)");

		string64				s_name;
		tNetPacket.r_string		(s_name);
		// create entity
		CSE_Abstract			*tpSE_Abstract = F_entity_Create	(s_name);
		R_ASSERT2				(tpSE_Abstract,"Can't create entity.");
		CSE_ALifeDynamicObject		*tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
		R_ASSERT2				(tpALifeDynamicObject,"Non-ALife object in the saved game!");
		tpALifeDynamicObject->Spawn_Read(tNetPacket);

		// Update
		tNetPacket.B.count		= tFileStream.r_u16();
		tFileStream.r			(tNetPacket.B.data,tNetPacket.B.count);
		tNetPacket.r_begin		(u_id);
		R_ASSERT2				(M_UPDATE==u_id,"Invalid packet ID (!= M_UPDATE)");
		tpALifeDynamicObject->UPDATE_Read(tNetPacket);
		tpALifeDynamicObject->Init(tpALifeDynamicObject->s_name);

		m_tObjectRegistry.insert(std::make_pair(tpALifeDynamicObject->ID,tpALifeDynamicObject));
	}
}

bool CSE_ALifeObjectRegistry::bfCheckIfTaskCompleted(CSE_Abstract &CSE_Abstract, CSE_ALifeHumanAbstract *tpALifeHumanAbstract, OBJECT_IT &I)
{
	if (tpALifeHumanAbstract->m_dwCurTask >= tpALifeHumanAbstract->m_tpTasks.size())
		return(false);
	I = CSE_Abstract.children.begin();
	OBJECT_IT	E = CSE_Abstract.children.end();
	CSE_ALifePersonalTask	&tPersonalTask = *(tpALifeHumanAbstract->m_tpTasks[tpALifeHumanAbstract->m_dwCurTask]);
	for ( ; I != E; I++) {
		switch (tPersonalTask.m_tTaskType) {
			case eTaskTypeSearchForItemCL :
			case eTaskTypeSearchForItemCG : {
				if (m_tObjectRegistry[*I]->m_tClassID == tPersonalTask.m_tClassID)
					return(true);
				break;
			}
			case eTaskTypeSearchForItemOL :
			case eTaskTypeSearchForItemOG : {
				if (m_tObjectRegistry[*I]->ID == tPersonalTask.m_tObjectID)
					return(true);
				break;
			}
		}
	}
	return(false);
}

bool CSE_ALifeObjectRegistry::bfCheckIfTaskCompleted(CSE_ALifeHumanAbstract *tpALifeHuman, OBJECT_IT &I)
{
	return(bfCheckIfTaskCompleted(*tpALifeHuman,tpALifeHuman,I));
}

bool CSE_ALifeObjectRegistry::bfCheckIfTaskCompleted(CSE_ALifeHumanAbstract *tpALifeHuman)
{
	OBJECT_IT I;
	return(bfCheckIfTaskCompleted(tpALifeHuman,I));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeEventRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeEventRegistry::CSE_ALifeEventRegistry()
{
	m_tEventID					= 0;
	m_tEventRegistry.clear		();
}

CSE_ALifeEventRegistry::~CSE_ALifeEventRegistry()
{
	free_map					(m_tEventRegistry);
}

void CSE_ALifeEventRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(EVENT_CHUNK_DATA);
	tMemoryStream.w				(&m_tEventID,sizeof(m_tEventID));
	//save_map					(m_tEventRegistry,tMemoryStream);
	tMemoryStream.close_chunk	();
}

void CSE_ALifeEventRegistry::Load(IReader	&tFileStream)
{
	R_ASSERT2					(tFileStream.find_chunk(EVENT_CHUNK_DATA),"Can't find chunk EVENT_CHUNK_DATA!");
	tFileStream.r				(&m_tEventID,sizeof(m_tEventID));
	//load_map					(m_tEventRegistry,tFileStream,tfChooseEventKeyPredicate);
}

void CSE_ALifeEventRegistry::Add(CSE_ALifeEvent	*tpEvent)
{
	m_tEventRegistry.insert		(std::make_pair(tpEvent->m_tEventID = m_tEventID++,tpEvent));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTaskRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeTaskRegistry::CSE_ALifeTaskRegistry()
{
	m_tTaskID					= 0;
	m_tTaskRegistry.clear		();
}

CSE_ALifeTaskRegistry::~CSE_ALifeTaskRegistry()
{
	free_map					(m_tTaskRegistry);
}

void CSE_ALifeTaskRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(TASK_CHUNK_DATA);
	tMemoryStream.w				(&m_tTaskID,sizeof(m_tTaskID));
	//save_map					(m_tTaskRegistry,tMemoryStream);
	tMemoryStream.close_chunk	();
}

void CSE_ALifeTaskRegistry::Load(IReader	&tFileStream)
{
	R_ASSERT2					(tFileStream.find_chunk(TASK_CHUNK_DATA),"Can't find chunk TASK_CHUNK_DATA");
	tFileStream.r				(&m_tTaskID,sizeof(m_tTaskID));
	//		load_map					(m_tTaskRegistry,tFileStream,tfChooseTaskKeyPredicate);
}

void CSE_ALifeTaskRegistry::Add	(CSE_ALifeTask	*tpTask)
{
	m_tTaskRegistry.insert		(std::make_pair(tpTask->m_tTaskID = m_tTaskID++,tpTask));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeGraphRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeGraphRegistry::CSE_ALifeGraphRegistry() : CSE_ALifeAStar()
{
}

void CSE_ALifeGraphRegistry::Init()
{
	inherited::Init				();

	for (int i=0; i<LOCATION_TYPE_COUNT; i++) {
		m_tpTerrain[i].resize	(LOCATION_COUNT);
		{
			GRAPH_VECTOR_IT			I = m_tpTerrain[i].begin();
			GRAPH_VECTOR_IT			E = m_tpTerrain[i].end();
			for ( ; I != E; I++)
				(*I).clear			();
		}
		for (_GRAPH_ID j=0; j<(_GRAPH_ID)getAI().GraphHeader().dwVertexCount; j++)
			m_tpTerrain[i][getAI().m_tpaGraph[j].tVertexTypes[i]].push_back(j);
	}

	m_tpGraphObjects.resize		(getAI().GraphHeader().dwVertexCount);
	{
		GRAPH_POINT_IT			I = m_tpGraphObjects.begin();
		GRAPH_POINT_IT			E = m_tpGraphObjects.end();
		for ( ; I != E; I++) {
			(*I).tpObjects.clear();
			(*I).tpEvents.clear	();
		}
	}
	m_tpCurrentLevel			= 0;
	m_tpActor					= 0;
}

void CSE_ALifeGraphRegistry::Update(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	if (!tpALifeDynamicObject->m_bDirectControl)
		return;
	if (tpALifeDynamicObject->s_flags.is(M_SPAWN_OBJECT_ASPLAYER))
		m_tpActor = tpALifeDynamicObject;

	u8 dwLevelID = getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tLevelID;

	ALIFE_ENTITY_P_VECTOR_PAIR_IT I = m_tLevelMap.find(dwLevelID);
	if (I == m_tLevelMap.end()) {
		ALIFE_ENTITY_P_VECTOR *tpTemp = xr_new<ALIFE_ENTITY_P_VECTOR>();
		tpTemp->push_back(tpALifeDynamicObject);
		m_tLevelMap.insert(std::make_pair(dwLevelID,tpTemp));
	}
	else
		(*I).second->push_back(tpALifeDynamicObject);

	if (m_tpActor && !m_tpCurrentLevel) {
		I = m_tLevelMap.find(getAI().m_tpaGraph[m_tpActor->m_tGraphID].tLevelID);
		R_ASSERT2(I != m_tLevelMap.end(),"Can't find corresponding to actor level!");
		m_tpCurrentLevel = (*I).second;
	}

	CSE_ALifeItem *tpALifeItem = dynamic_cast<CSE_ALifeItem *>(tpALifeDynamicObject);
	if (tpALifeItem) {
		if (!tpALifeItem->bfAttached())
			m_tpGraphObjects[tpALifeItem->m_tGraphID].tpObjects.push_back(tpALifeItem);
		return;
	}

	if (!dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject))
		m_tpGraphObjects[tpALifeDynamicObject->m_tGraphID].tpObjects.push_back(tpALifeDynamicObject);
}

void CSE_ALifeGraphRegistry::vfRemoveObjectFromGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphID)
{
	ALIFE_ENTITY_P_IT				I = m_tpGraphObjects[tGraphID].tpObjects.begin();
	ALIFE_ENTITY_P_IT				E = m_tpGraphObjects[tGraphID].tpObjects.end();
	bool							bOk = false;
	for ( ; I != E; I++)
		if ((*I) == tpALifeDynamicObject) {
			m_tpGraphObjects[tGraphID].tpObjects.erase(I);
			bOk = true;
			break;
		}
		VERIFY							(bOk);
		//.		Msg("ALife : removing object %s from graph point %d",tpALifeDynamicObject->s_name_replace,tGraphID);
}

void CSE_ALifeGraphRegistry::vfAddObjectToGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tNextGraphPointID)
{
	m_tpGraphObjects[tNextGraphPointID].tpObjects.push_back(tpALifeDynamicObject);
	tpALifeDynamicObject->m_tGraphID = tNextGraphPointID;
	//.		Msg("ALife : adding object %s to graph point %d",tpALifeDynamicObject->s_name_replace,tNextGraphPointID);
}

void CSE_ALifeGraphRegistry::vfChangeObjectGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
{
	vfRemoveObjectFromGraphPoint	(tpALifeDynamicObject,tGraphPointID);
	vfAddObjectToGraphPoint			(tpALifeDynamicObject,tNextGraphPointID);
	tpALifeDynamicObject->m_tGraphID	= tNextGraphPointID;
	tpALifeDynamicObject->o_Position	= getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tLocalPoint;
	tpALifeDynamicObject->m_tNodeID		= getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tNodeID;
}

// events
void CSE_ALifeGraphRegistry::vfRemoveEventFromGraphPoint(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphID)
{
	EVENT_P_IT						I = m_tpGraphObjects[tGraphID].tpEvents.begin();
	EVENT_P_IT						E = m_tpGraphObjects[tGraphID].tpEvents.end();
	for ( ; I != E; I++)
		if ((*I) == tpEvent) {
			m_tpGraphObjects[tGraphID].tpEvents.erase(I);
			break;
		}
}

void CSE_ALifeGraphRegistry::vfAddEventToGraphPoint(CSE_ALifeEvent *tpEvent, _GRAPH_ID tNextGraphPointID)
{
	m_tpGraphObjects[tNextGraphPointID].tpEvents.push_back(tpEvent);
}

void CSE_ALifeGraphRegistry::vfChangeEventGraphPoint(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
{
	vfRemoveEventFromGraphPoint	(tpEvent,tGraphPointID);
	vfAddEventToGraphPoint		(tpEvent,tNextGraphPointID);
}

void CSE_ALifeGraphRegistry::vfAttachItem(CSE_Abstract &CSE_Abstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bAddChild)
{
	if (bAddChild) {
		//.			Msg("ALife : (OFFLINE) Attaching item %s to object %s",tpALifeItem->s_name_replace,CSE_Abstract.s_name_replace);
		CSE_Abstract.children.push_back(tpALifeItem->ID);
		tpALifeItem->ID_Parent = CSE_Abstract.ID;
	}
	//.		else
	//.			Msg("ALife : (ONLINE) Attaching item %s to object %s",tpALifeItem->s_name_replace,CSE_Abstract.s_name_replace);

	vfRemoveObjectFromGraphPoint(tpALifeItem,tGraphID);

	CSE_ALifeTraderAbstract *tpALifeTraderParams = dynamic_cast<CSE_ALifeTraderAbstract*>(&CSE_Abstract);
	VERIFY(tpALifeTraderParams);
	tpALifeTraderParams->m_fCumulativeItemMass += tpALifeItem->m_fMass;
}

void CSE_ALifeGraphRegistry::vfDetachItem(CSE_Abstract &CSE_Abstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bRemoveChild)
{
	if (bRemoveChild) {
		xr_vector<u16>				&tChildren = CSE_Abstract.children;
		xr_vector<u16>::iterator	I = std::find	(tChildren.begin(),tChildren.end(),tpALifeItem->ID);
		VERIFY					(I != tChildren.end());
		tChildren.erase			(I);
		tpALifeItem->ID_Parent	= 0xffff;
	}

	vfAddObjectToGraphPoint(tpALifeItem,tGraphID);

	CSE_ALifeTraderAbstract *tpTraderParams = dynamic_cast<CSE_ALifeTraderAbstract*>(&CSE_Abstract);
	VERIFY(tpTraderParams);
	tpTraderParams->m_fCumulativeItemMass -= tpALifeItem->m_fMass;
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTraderRegistry
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeTraderRegistry::Init()
{
	m_tpTraders.clear			();
}

void CSE_ALifeTraderRegistry::Update(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	CSE_ALifeTrader *tpALifeTrader = dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject);
	if (tpALifeTrader) {
		m_tpTraders.push_back(tpALifeTrader);
		std::sort(m_tpTraders.begin(),m_tpTraders.end(),CCompareTraderRanksPredicate());
	}
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeScheduleRegistry
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeScheduleRegistry::Init()
{
	m_tpScheduledObjects.clear	();
}

void CSE_ALifeScheduleRegistry::Update(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	if (!tpALifeDynamicObject->m_bDirectControl)
		return;

	CSE_ALifeMonsterAbstract		*tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract *>(tpALifeDynamicObject);

	if (tpALifeMonsterAbstract && (tpALifeMonsterAbstract->fHealth > 0))
		m_tpScheduledObjects.push_back	(tpALifeMonsterAbstract);
}	

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeSpawnRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeSpawnRegistry::CSE_ALifeSpawnRegistry()
{
	m_tpSpawnPoints.clear		();
	m_tpArtefactSpawnPositions.clear();
	m_baAliveSpawnObjects.clear ();
}

CSE_ALifeSpawnRegistry::~CSE_ALifeSpawnRegistry()
{
	free_object_vector				(m_tpSpawnPoints);
}

void CSE_ALifeSpawnRegistry::Init()
{
	free_object_vector				(m_tpSpawnPoints);
}

void CSE_ALifeSpawnRegistry::Load(IReader	&tFileStream)
{
	inherited::Load				(tFileStream);
	m_tpSpawnPoints.resize		(m_dwSpawnCount);
	m_baAliveSpawnObjects.assign(m_dwSpawnCount,false);
	ALIFE_ENTITY_P_IT			I = m_tpSpawnPoints.begin();
	ALIFE_ENTITY_P_IT			E = m_tpSpawnPoints.end();
	NET_Packet					tNetPacket;
	IReader						*S = 0;
	u16							ID;
	for (int id=0; I != E; I++, id++) {
		R_ASSERT2				(0!=(S = tFileStream.open_chunk(id)),"Can't find entity chunk in the 'game.spawn'");
		// Spawn
		tNetPacket.B.count		= S->r_u16();
		S->r					(tNetPacket.B.data,tNetPacket.B.count);
		tNetPacket.r_begin		(ID);
		R_ASSERT2				(M_SPAWN == ID,"Invalid packet ID (!= M_SPAWN)!");

		string64				s_name;
		tNetPacket.r_string		(s_name);
		CSE_Abstract			*E = F_entity_Create(s_name);

		R_ASSERT2				(E,"Can't create entity.");
		E->Spawn_Read			(tNetPacket);
		// Update
		tNetPacket.B.count		= S->r_u16();
		S->r					(tNetPacket.B.data,tNetPacket.B.count);
		tNetPacket.r_begin		(ID);
		R_ASSERT2				(M_UPDATE == ID,"Invalid packet ID (!= M_UPDATE)!");
		E->UPDATE_Read			(tNetPacket);

		E->Init					(E->s_name);
		CSE_ALifeObject			*tpALifeObject = dynamic_cast<CSE_ALifeObject*>(E);
		VERIFY					(tpALifeObject);

		R_ASSERT2				((E->s_gameid == GAME_SINGLE) || (E->s_gameid == GAME_ANY),"Invalid game type!");
		R_ASSERT2				((*I = dynamic_cast<CSE_ALifeDynamicObject*>(E)) != 0,"Non-ALife object in the 'game.spawn'");
	}
	{
		R_ASSERT2				(0!=(S = tFileStream.open_chunk(id++)),"Can't find artefact spawn points chunk in the 'game.spawn'");
		m_tpArtefactSpawnPositions.resize(tFileStream.r_u32());
		LEVEL_POINT_IT			I = m_tpArtefactSpawnPositions.begin();
		LEVEL_POINT_IT			E = m_tpArtefactSpawnPositions.end();
		for ( ; I != E; I++)
			tFileStream.r		(&*I,sizeof(*I));
	}
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeAnomalyRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeAnomalyRegistry::CSE_ALifeAnomalyRegistry()
{
	m_tpAnomalies.clear			();
}

CSE_ALifeAnomalyRegistry::~CSE_ALifeAnomalyRegistry()
{
	ANOMALY_P_VECTOR_IT			I = m_tpAnomalies.begin();
	ANOMALY_P_VECTOR_IT			E = m_tpAnomalies.end();
	for ( ; I != E; I++)
		free_object_vector		(*I);
}

void CSE_ALifeAnomalyRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(ANOMALY_CHUNK_DATA);
	tMemoryStream.w_u32			(m_tpAnomalies.size());
	ANOMALY_P_VECTOR_IT			I = m_tpAnomalies.begin();
	ANOMALY_P_VECTOR_IT			E = m_tpAnomalies.end();
	for ( ; I != E; I++)
		save_object_vector		(*I,tMemoryStream);
	tMemoryStream.close_chunk	();
}

void CSE_ALifeAnomalyRegistry::Load(IReader &tFileStream)
{ 
	R_ASSERT2					(tFileStream.find_chunk(ANOMALY_CHUNK_DATA),"Can't find chunk ANOMALY_CHUNK_DATA!");
	m_tpAnomalies.resize		(tFileStream.r_u32());
	ANOMALY_P_VECTOR_IT			I = m_tpAnomalies.begin();
	ANOMALY_P_VECTOR_IT			E = m_tpAnomalies.end();
	for ( ; I != E; I++)
		load_object_vector		(*I,tFileStream);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeOrganizationRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeOrganizationRegistry::CSE_ALifeOrganizationRegistry()
{
	m_tOrganizationRegistry.clear();
	m_tDiscoveryRegistry.clear	();
	
	R_ASSERT2					(pSettings->section_exist("organizations"),"There is no section 'organizations' in the 'system.ltx'!");
	LPCSTR						N,V;
	for (u32 k = 0; pSettings->r_line("organizations",k,&N,&V); k++)
		m_tOrganizationRegistry.insert(std::make_pair(N,xr_new<CSE_ALifeOrganization>(N)));
	
	ORGANIZATION_P_PAIR_IT		I = m_tOrganizationRegistry.begin();
	ORGANIZATION_P_PAIR_IT		E = m_tOrganizationRegistry.end();
	for ( ; I != E; I++) {
		LPCSTR_IT				i = (*I).second->m_tpPossibleDiscoveries.begin();
		LPCSTR_IT				e = (*I).second->m_tpPossibleDiscoveries.end();
		for ( ; i != e; i++) {
			DISCOVERY_P_PAIR_IT j = m_tDiscoveryRegistry.find(*i);
			if (j == m_tDiscoveryRegistry.end())
				m_tDiscoveryRegistry.insert(std::make_pair(*i,xr_new<CSE_ALifeDiscovery>(*i)));
		}
	}
}

CSE_ALifeOrganizationRegistry::~CSE_ALifeOrganizationRegistry()
{
	free_map					(m_tOrganizationRegistry);
	free_map					(m_tDiscoveryRegistry);
}

void CSE_ALifeOrganizationRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(DISCOVERY_CHUNK_DATA);
	save_map					(m_tOrganizationRegistry,tMemoryStream);
	save_map					(m_tDiscoveryRegistry,tMemoryStream);
	tMemoryStream.close_chunk	();
}

void CSE_ALifeOrganizationRegistry::Load(IReader &tFileStream)
{ 
	R_ASSERT2					(tFileStream.find_chunk(DISCOVERY_CHUNK_DATA),"Can't find chunk DISCOVERY_CHUNK_DATA!");
	load_initialized_map		(m_tOrganizationRegistry,tFileStream,cafChooseOrganizationKeyPredicate);
	load_initialized_map		(m_tDiscoveryRegistry,tFileStream,cafChooseDiscoveryKeyPredicate);
}
