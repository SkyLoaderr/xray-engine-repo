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
#include "ai_alife_space.h"
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
	// since we use multiple inheritance, we have to cast the objects to the underlying class
	// to delete them properly
	D_OBJECT_PAIR_IT			I = m_tObjectRegistry.begin();
	D_OBJECT_PAIR_IT			E = m_tObjectRegistry.end();
	for ( ; I != E; I++)
		xr_delete				(dynamic_cast<CSE_Abstract*>((*I).second));
}

void CSE_ALifeObjectRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(OBJECT_CHUNK_DATA);
	tMemoryStream.w_u32			((u32)m_tObjectRegistry.size());
	D_OBJECT_PAIR_IT I			= m_tObjectRegistry.begin();
	D_OBJECT_PAIR_IT E			= m_tObjectRegistry.end();
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
	m_tObjectRegistry.clear		();
	u32							dwCount = tFileStream.r_u32();
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
		CSE_ALifeDynamicObject	*tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
		R_ASSERT2				(tpALifeDynamicObject,"Non-ALife object in the saved game!");
		tpALifeDynamicObject->Spawn_Read(tNetPacket);

		// Update
		tNetPacket.B.count		= tFileStream.r_u16();
		tFileStream.r			(tNetPacket.B.data,tNetPacket.B.count);
		tNetPacket.r_begin		(u_id);
		R_ASSERT2				(M_UPDATE==u_id,"Invalid packet ID (!= M_UPDATE)");
		tpALifeDynamicObject->UPDATE_Read(tNetPacket);
		Add						(tpALifeDynamicObject);
	}
}

void CSE_ALifeObjectRegistry::Add(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	if (m_tObjectRegistry.find(tpALifeDynamicObject->ID) != m_tObjectRegistry.end()) {
		R_ASSERT2				((*(m_tObjectRegistry.find(tpALifeDynamicObject->ID))).second == tpALifeDynamicObject,"The specified object is already presented in the Object Registry!");
		R_ASSERT2				((*(m_tObjectRegistry.find(tpALifeDynamicObject->ID))).second != tpALifeDynamicObject,"Object with the specified ID is already presented in the Object Registry!");
	}
	
	m_tObjectRegistry.insert	(std::make_pair(tpALifeDynamicObject->ID,tpALifeDynamicObject));
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
	delete_data					(m_tEventRegistry);
}

void CSE_ALifeEventRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(EVENT_CHUNK_DATA);
	tMemoryStream.w				(&m_tEventID,sizeof(m_tEventID));
	save_data					(m_tEventRegistry,tMemoryStream);
	tMemoryStream.close_chunk	();
}

void CSE_ALifeEventRegistry::Load(IReader	&tFileStream)
{
	R_ASSERT2					(tFileStream.find_chunk(EVENT_CHUNK_DATA),"Can't find chunk EVENT_CHUNK_DATA!");
	tFileStream.r				(&m_tEventID,sizeof(m_tEventID));
	load_data					(m_tEventRegistry,tFileStream,tfChooseEventKeyPredicate);
}

void CSE_ALifeEventRegistry::Add(CSE_ALifeEvent	*tpEvent)
{
	if (m_tEventRegistry.find(tpEvent->m_tEventID) != m_tEventRegistry.end()) {
		R_ASSERT2				((*(m_tEventRegistry.find(tpEvent->m_tEventID))).second == tpEvent,"The specified event is already presented in the Event Registry!");
		R_ASSERT2				((*(m_tEventRegistry.find(tpEvent->m_tEventID))).second != tpEvent,"Event with the specified ID is already presented in the Event Registry!");
	}
	m_tEventRegistry.insert		(std::make_pair(tpEvent->m_tEventID = m_tEventID++,tpEvent));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTaskRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeTaskRegistry::CSE_ALifeTaskRegistry()
{
	m_tTaskID					= 0;
	m_tTaskRegistry.clear		();
	m_tTaskCrossMap.clear		();
}

CSE_ALifeTaskRegistry::~CSE_ALifeTaskRegistry()
{
	delete_data					(m_tTaskRegistry);
}

void CSE_ALifeTaskRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(TASK_CHUNK_DATA);
	tMemoryStream.w				(&m_tTaskID,sizeof(m_tTaskID));
	save_data					(m_tTaskRegistry,tMemoryStream);
	tMemoryStream.close_chunk	();
}

void CSE_ALifeTaskRegistry::Load(IReader	&tFileStream)
{
	R_ASSERT2					(tFileStream.find_chunk(TASK_CHUNK_DATA),"Can't find chunk TASK_CHUNK_DATA");
	tFileStream.r				(&m_tTaskID,sizeof(m_tTaskID));
	load_data					(m_tTaskRegistry,tFileStream,tfChooseTaskKeyPredicate);
	TASK_PAIR_IT				I = m_tTaskRegistry.begin();
	TASK_PAIR_IT				E = m_tTaskRegistry.end();
	for ( ; I != E; I++)
		Update((*I).second);
}

void CSE_ALifeTaskRegistry::Add	(CSE_ALifeTask	*tpTask)
{
	if (m_tTaskRegistry.find(tpTask->m_tTaskID) != m_tTaskRegistry.end())
		R_ASSERT2				(true,"The specified task is already presented in the Task Registry!");
	m_tTaskRegistry.insert		(std::make_pair(tpTask->m_tTaskID = m_tTaskID++,tpTask));
	Update						(tpTask);
}

void CSE_ALifeTaskRegistry::Update(CSE_ALifeTask	*tpTask)
{
	R_ASSERT2					(tpTask == tpfGetTaskByID(tpTask->m_tTaskID),"Cannot find a specified task in the Task registry!");
	OBJECT_TASK_PAIR_IT			J = m_tTaskCrossMap.find(tpTask->m_tCustomerID);
	if (J == m_tTaskCrossMap.end()) {
		m_tTaskCrossMap.insert	(std::make_pair(tpTask->m_tCustomerID,TASK_SET()));
		J						= m_tTaskCrossMap.find(tpTask->m_tCustomerID);
	}
	(*J).second.insert			(tpTask->m_tTaskID);
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
		{
			for (int j=0; j<LOCATION_COUNT; j++)
				m_tpTerrain[i][j].clear();
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
	if (tpALifeDynamicObject->s_flags.is(M_SPAWN_OBJECT_ASPLAYER)) {
		m_tpActor = dynamic_cast<CSE_ALifeCreatureActor*>(tpALifeDynamicObject);
		R_ASSERT2			(m_tpActor,"Invalid flag M_SPAWN_OBJECT_ASPLAYER for non-actor object!");
	}

	u8 dwLevelID = getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tLevelID;

	D_OBJECT_P_MAP_PAIR_IT I = m_tLevelMap.find(dwLevelID);
	if (I == m_tLevelMap.end()) {
		D_OBJECT_P_MAP	*tpTemp = xr_new<D_OBJECT_P_MAP>();
		tpTemp->insert		(std::make_pair(tpALifeDynamicObject->ID,tpALifeDynamicObject));
		m_tLevelMap.insert	(std::make_pair(dwLevelID,tpTemp));
	}
	else
		(*I).second->insert	(std::make_pair(tpALifeDynamicObject->ID,tpALifeDynamicObject));

	if (m_tpActor && !m_tpCurrentLevel) {
		I = m_tLevelMap.find(getAI().m_tpaGraph[m_tpActor->m_tGraphID].tLevelID);
		R_ASSERT2(I != m_tLevelMap.end(),"Can't find corresponding to actor level!");
		m_tpCurrentLevel = (*I).second;
		m_tCurrentLevelID = (*I).first;
	}

	CSE_ALifeItem *tpALifeItem = dynamic_cast<CSE_ALifeItem *>(tpALifeDynamicObject);
	if ((!tpALifeItem && !dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject)) || (tpALifeItem && !tpALifeItem->bfAttached()))
		vfAddObjectToGraphPoint(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);
}

void CSE_ALifeGraphRegistry::vfRemoveObjectFromCurrentLevel(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	D_OBJECT_P_PAIR_IT		I = m_tpCurrentLevel->find(tpALifeDynamicObject->ID), J = I;
	R_ASSERT2				(I != m_tpCurrentLevel->end(),"Object not found in the level map!");
	if (m_tNextFirstSwitchObjectID == tpALifeDynamicObject->ID) {
		if (++J == m_tpCurrentLevel->end())
			J = m_tpCurrentLevel->begin();
		if (J != m_tpCurrentLevel->end())
			m_tNextFirstSwitchObjectID	= (*J).second->ID;
		else
			m_tNextFirstSwitchObjectID	= _OBJECT_ID(0xffff);
	}
	m_tpCurrentLevel->erase(I);
}

void CSE_ALifeGraphRegistry::vfRemoveObjectFromGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphID)
{
	if (dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject))
		return;

	D_OBJECT_P_IT				I = m_tpGraphObjects[tGraphID].tpObjects.begin();
	D_OBJECT_P_IT				E = m_tpGraphObjects[tGraphID].tpObjects.end();
	bool							bOk = false;
	for ( ; I != E; I++)
		if ((*I) == tpALifeDynamicObject) {
			m_tpGraphObjects[tGraphID].tpObjects.erase(I);
			bOk = true;
			break;
		}
	R_ASSERT3						(bOk,"Specified object (%s) not found on the given graph point!",tpALifeDynamicObject->s_name_replace);
#ifdef DEBUG_LOG
	Msg("ALife : removing object %s from graph point %d",tpALifeDynamicObject->s_name_replace,tGraphID);
#endif
}

void CSE_ALifeGraphRegistry::vfAddObjectToGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tNextGraphPointID)
{
	m_tpGraphObjects[tNextGraphPointID].tpObjects.push_back(tpALifeDynamicObject);
	tpALifeDynamicObject->m_tGraphID = tNextGraphPointID;
#ifdef DEBUG_LOG
	Msg("ALife : adding object %s to graph point %d",tpALifeDynamicObject->s_name_replace,tNextGraphPointID);
#endif
}

void CSE_ALifeGraphRegistry::vfChangeObjectGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
{
	vfRemoveObjectFromGraphPoint		(tpALifeDynamicObject,tGraphPointID);
	vfAddObjectToGraphPoint				(tpALifeDynamicObject,tNextGraphPointID);
	tpALifeDynamicObject->m_tGraphID	= tNextGraphPointID;
	tpALifeDynamicObject->o_Position	= getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tLocalPoint;
	tpALifeDynamicObject->m_tNodeID		= getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tNodeID;

	if ((getAI().m_tpaGraph[tGraphPointID].tLevelID == m_tCurrentLevelID) && (getAI().m_tpaGraph[tNextGraphPointID].tLevelID != m_tCurrentLevelID))
		vfRemoveObjectFromCurrentLevel	(tpALifeDynamicObject);
	else
		if ((getAI().m_tpaGraph[tGraphPointID].tLevelID != m_tCurrentLevelID) && (getAI().m_tpaGraph[tNextGraphPointID].tLevelID == m_tCurrentLevelID))
			m_tpCurrentLevel->insert	(std::make_pair(tpALifeDynamicObject->ID,tpALifeDynamicObject));
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

void CSE_ALifeGraphRegistry::vfAttachItem(CSE_Abstract &tAbstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bALifeRequest)
{
	if (bALifeRequest)
		vfRemoveObjectFromGraphPoint(tpALifeItem,tGraphID);

	CSE_ALifeTraderAbstract		*l_tpALifeTraderAbstract = dynamic_cast<CSE_ALifeTraderAbstract*>(&tAbstract);
	R_ASSERT2					(!bALifeRequest || l_tpALifeTraderAbstract,"Cannot attach an item to a non-trader object");

	if (l_tpALifeTraderAbstract) {
		l_tpALifeTraderAbstract->children.push_back(tpALifeItem->ID);
		tpALifeItem->ID_Parent	= l_tpALifeTraderAbstract->ID;
		l_tpALifeTraderAbstract->m_fCumulativeItemMass		+= tpALifeItem->m_fMass;
		l_tpALifeTraderAbstract->m_iCumulativeItemVolume	+= tpALifeItem->m_iVolume;
	}
	else {
		tAbstract.children.push_back(tpALifeItem->ID);
		tpALifeItem->ID_Parent	= tAbstract.ID;
	}
}

void CSE_ALifeGraphRegistry::vfDetachItem(CSE_Abstract &tAbstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bALifeRequest)
{
	vfAddObjectToGraphPoint		(tpALifeItem,tGraphID);

	CSE_ALifeTraderAbstract		*l_tpALifeTraderAbstract = dynamic_cast<CSE_ALifeTraderAbstract*>(&tAbstract);
	R_ASSERT2					(!bALifeRequest || l_tpALifeTraderAbstract,"Cannot detach an item from non-trader object");

	if (l_tpALifeTraderAbstract) {
		OBJECT_IT				I = std::find	(l_tpALifeTraderAbstract->children.begin(),l_tpALifeTraderAbstract->children.end(),tpALifeItem->ID);
		R_ASSERT2				(I != l_tpALifeTraderAbstract->children.end(),"Can't detach an item which is not on my own");
		l_tpALifeTraderAbstract->children.erase(I);
		tpALifeItem->ID_Parent	= 0xffff;
		l_tpALifeTraderAbstract->m_fCumulativeItemMass		-= tpALifeItem->m_fMass;
		l_tpALifeTraderAbstract->m_iCumulativeItemVolume	-= tpALifeItem->m_iVolume;
	}
	else {
		OBJECT_IT				I = std::find	(tAbstract.children.begin(),tAbstract.children.end(),tpALifeItem->ID);
		R_ASSERT2				(I != tAbstract.children.end(),"Can't detach an item which is not on my own");
		tAbstract.children.erase(I);
		tpALifeItem->ID_Parent	= 0xffff;
	}
}

void CSE_ALifeGraphRegistry::vfAssignGraphPosition(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	tpALifeMonsterAbstract->m_tNextGraphID					= tpALifeMonsterAbstract->m_tPrevGraphID = tpALifeMonsterAbstract->m_tGraphID;
	tpALifeMonsterAbstract->m_fDistanceToPoint				= tpALifeMonsterAbstract->m_fDistance;
	_GRAPH_ID												tGraphID = tpALifeMonsterAbstract->m_tNextGraphID;
	u16														wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
	CSE_ALifeGraph::SGraphEdge								*tpaEdges = (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);
	for (int i=0; i<wNeighbourCount; i++)
		if (tpaEdges[i].fPathDistance > tpALifeMonsterAbstract->m_fDistance) {
			tpALifeMonsterAbstract->m_fDistanceFromPoint	= tpaEdges[i].fPathDistance - tpALifeMonsterAbstract->m_fDistance;
			break;
		}
}


////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTraderRegistry
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeTraderRegistry::Init()
{
	m_tpTraders.clear			();
	m_tpCrossTraders.clear		();
}

void CSE_ALifeTraderRegistry::Update(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	CSE_ALifeTrader *tpALifeTrader = dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject);
	if (tpALifeTrader) {
		m_tpTraders.push_back(tpALifeTrader);
		std::sort(m_tpTraders.begin(),m_tpTraders.end(),CCompareTraderRanksPredicate());
	}
}

CSE_ALifeTrader *CSE_ALifeTraderRegistry::tpfGetNearestSuitableTrader(CSE_ALifeHumanAbstract *tpALifeHumanAbstract)
{
	float			fBestDistance = MAX_NODE_ESTIMATION_COST;
	CSE_ALifeTrader *tpBestTrader = 0;
	TRADER_P_IT		I = m_tpTraders.begin();
	TRADER_P_IT		E = m_tpTraders.end();
	Fvector			&tGlobalPoint = getAI().m_tpaGraph[tpALifeHumanAbstract->m_tGraphID].tGlobalPoint;
	for ( ; I != E; I++) {
//		if ((*I)->m_tRank != tpALifeHumanAbstract->m_tRank)
//			break;
		float		fCurDistance = getAI().m_tpaGraph[(*I)->m_tGraphID].tGlobalPoint.distance_to(tGlobalPoint);
		if (fCurDistance < fBestDistance) {
			fBestDistance	= fCurDistance;
			tpBestTrader	= *I;
		}
	}
	return			(tpBestTrader);
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
	vfAddObjectToScheduled		(tpALifeDynamicObject);
}

void CSE_ALifeScheduleRegistry::vfAddObjectToScheduled(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	R_ASSERT2					(!tpALifeDynamicObject->m_bOnline,"Can't add to scheduled objects online object!");
	
	CSE_ALifeSchedulable		*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeSchedulable*>(tpALifeDynamicObject);
	if (!l_tpALifeSchedulable)
		return;

	CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeDynamicObject);
	if (l_tpALifeMonsterAbstract && (l_tpALifeMonsterAbstract->fHealth <= 0))
		return;

	CSE_ALifeAnomalousZone		*l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeDynamicObject);
	if (l_tpALifeAnomalousZone && (l_tpALifeAnomalousZone->m_maxPower < EPS_L))
		return;

	m_tpScheduledObjects.insert	(std::make_pair(l_tpALifeSchedulable->ID,l_tpALifeSchedulable));

	if (m_tNextFirstProcessObjectID == _OBJECT_ID(-1))
		m_tNextFirstProcessObjectID = l_tpALifeSchedulable->ID;
}

void CSE_ALifeScheduleRegistry::vfRemoveObjectFromScheduled(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	R_ASSERT2					(tpALifeDynamicObject->m_bOnline || (dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeDynamicObject) && (dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeDynamicObject)->fHealth <= 0)	|| (dynamic_cast<CSE_ALifeAbstractGroup*>(tpALifeDynamicObject) && dynamic_cast<CSE_ALifeAbstractGroup*>(tpALifeDynamicObject)->m_tpMembers.empty())),"Can't remove from scheduled objects offline object!");
	
	CSE_ALifeSchedulable		*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeMonsterAbstract *>(tpALifeDynamicObject);
	if (!l_tpALifeSchedulable)
		return;

	CSE_ALifeAnomalousZone		*l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeDynamicObject);
	if (l_tpALifeAnomalousZone && (l_tpALifeAnomalousZone->m_maxPower < EPS_L))
		return;

	CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract *>(tpALifeDynamicObject);
	if (tpALifeMonsterAbstract && (tpALifeMonsterAbstract->fHealth <= 0) && (m_tpScheduledObjects.find(tpALifeDynamicObject->ID) == m_tpScheduledObjects.end()))
		return;

	SCHEDULE_P_PAIR_IT			I = m_tpScheduledObjects.find(l_tpALifeSchedulable->ID), J = I;
	R_ASSERT2					(I != m_tpScheduledObjects.end(),"Object not found on the level map!");
	if (m_tNextFirstProcessObjectID == l_tpALifeSchedulable->ID) {
		if (++J == m_tpScheduledObjects.end())
			J = m_tpScheduledObjects.begin();
		if (J != m_tpScheduledObjects.end())
			m_tNextFirstProcessObjectID	= (*J).second->ID;
		else
			m_tNextFirstProcessObjectID	= _OBJECT_ID(-1);
	}
	m_tpScheduledObjects.erase(I);
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
	// since we use multiple inheritance, we have to cast the objects to the underlying class
	// to delete them properly
	D_OBJECT_P_IT			I = m_tpSpawnPoints.begin();
	D_OBJECT_P_IT			E = m_tpSpawnPoints.end();
	for ( ; I != E; I++)
		xr_delete				(dynamic_cast<CSE_Abstract*>(*I));
}

void CSE_ALifeSpawnRegistry::Init()
{
	// since we use multiple inheritance, we have to cast the objects to the underlying class
	// to delete them properly
	D_OBJECT_P_IT			I = m_tpSpawnPoints.begin();
	D_OBJECT_P_IT			E = m_tpSpawnPoints.end();
	for ( ; I != E; I++)
		xr_delete				(dynamic_cast<CSE_Abstract*>(*I));
}

void CSE_ALifeSpawnRegistry::Load(IReader	&tFileStream)
{
	inherited::Load				(tFileStream);
	m_tpSpawnPoints.resize		(m_dwSpawnCount);
	m_baAliveSpawnObjects.assign(m_dwSpawnCount,false);
	m_tArtefactAnomalyMap.clear	();
	D_OBJECT_P_IT			I = m_tpSpawnPoints.begin();
	D_OBJECT_P_IT			E = m_tpSpawnPoints.end();
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

		CSE_ALifeObject			*tpALifeObject = dynamic_cast<CSE_ALifeObject*>(E);
		VERIFY					(tpALifeObject);

		R_ASSERT2				((E->s_gameid == GAME_SINGLE) || (E->s_gameid == GAME_ANY),"Invalid game type!");
		R_ASSERT2				((*I = dynamic_cast<CSE_ALifeDynamicObject*>(E)) != 0,"Non-ALife object in the 'game.spawn'");
		R_ASSERT3				((*I)->m_tNodeID && ((*I)->m_tNodeID != u32(-1)),"Invalid node for object ",(*I)->s_name_replace);
		
		// building map of sets : get all the zone types which can generate given artefact
		CSE_ALifeAnomalousZone	*l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(E);
		if (l_tpALifeAnomalousZone) {
			EAnomalousZoneType	l_tAnomalousZoneType = l_tpALifeAnomalousZone->m_tAnomalyType;
			for (u16 i=0, n = l_tpALifeAnomalousZone->m_wItemCount; i<n; i++) {
				ITEM_SET_PAIR_IT	I = m_tArtefactAnomalyMap.find(l_tpALifeAnomalousZone->m_cppArtefactSections[i]);
				if (I != m_tArtefactAnomalyMap.end())
					(*I).second.insert(l_tAnomalousZoneType);
				else {
					m_tArtefactAnomalyMap.insert(std::make_pair(l_tpALifeAnomalousZone->m_cppArtefactSections[i],U32_SET()));
					I = m_tArtefactAnomalyMap.find(l_tpALifeAnomalousZone->m_cppArtefactSections[i]);
					if ((*I).second.find(l_tAnomalousZoneType) == (*I).second.end())
						(*I).second.insert(l_tAnomalousZoneType);
				}
			}
		}
	}
	R_ASSERT2					(0!=(S = tFileStream.open_chunk(id++)),"Can't find artefact spawn points chunk in the 'game.spawn'");
	load_data					(m_tpArtefactSpawnPositions,tFileStream);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeAnomalyRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeAnomalyRegistry::CSE_ALifeAnomalyRegistry()
{
	m_tpAnomalies.clear			();
	m_tpCrossAnomalies.resize	(eAnomalousZoneTypeDummy);
}

CSE_ALifeAnomalyRegistry::~CSE_ALifeAnomalyRegistry()
{
	delete_data					(m_tpAnomalies);
}

void CSE_ALifeAnomalyRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(ANOMALY_CHUNK_DATA);
	save_data					(m_tpAnomalies,tMemoryStream);
	tMemoryStream.close_chunk	();
}

void CSE_ALifeAnomalyRegistry::Load(IReader &tFileStream)
{ 
	R_ASSERT2					(tFileStream.find_chunk(ANOMALY_CHUNK_DATA),"Can't find chunk ANOMALY_CHUNK_DATA!");
	load_data					(m_tpAnomalies,tFileStream);
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
		LPSTR_IT				i = (*I).second->m_tpPossibleDiscoveries.begin();
		LPSTR_IT				e = (*I).second->m_tpPossibleDiscoveries.end();
		for ( ; i != e; i++) {
			DISCOVERY_P_PAIR_IT j = m_tDiscoveryRegistry.find(*i);
			if (j == m_tDiscoveryRegistry.end())
				m_tDiscoveryRegistry.insert(std::make_pair(*i,xr_new<CSE_ALifeDiscovery>(*i)));
		}
	}
	LPCSTR						S = pSettings->r_string("alife","preknown_artefacts");
	for (u32 i=0, n=_GetItemCount(S); i<n; i++) {
		string64				S1;
		_GetItem				(S,i,S1);
		LPSTR					S2 = (LPSTR)xr_malloc((strlen(S1) + 1)*sizeof(char));
		strcpy					(S2,S1);
		m_tArtefactRegistry.insert(std::make_pair(S2,false));
	}
}

CSE_ALifeOrganizationRegistry::~CSE_ALifeOrganizationRegistry()
{
	delete_data					(m_tOrganizationRegistry);
	delete_data					(m_tDiscoveryRegistry);
	delete_data					(m_tArtefactRegistry);
}

void CSE_ALifeOrganizationRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(DISCOVERY_CHUNK_DATA);
	save_data					(m_tOrganizationRegistry,tMemoryStream,false);
	save_data					(m_tDiscoveryRegistry,tMemoryStream,false);
	save_data					(m_tArtefactRegistry,tMemoryStream);
	tMemoryStream.close_chunk	();
}

void CSE_ALifeOrganizationRegistry::Load(IReader &tFileStream)
{ 
	R_ASSERT2					(tFileStream.find_chunk(DISCOVERY_CHUNK_DATA),"Can't find chunk DISCOVERY_CHUNK_DATA!");
	load_data					(m_tOrganizationRegistry,tFileStream,false);
	load_data					(m_tDiscoveryRegistry,tFileStream,false);
	load_data					(m_tArtefactRegistry,tFileStream);
}
