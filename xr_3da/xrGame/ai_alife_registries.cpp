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
#include "game_graph.h"
#include "level_graph.h"
#include "xrserver_objects_alife_monsters.h"
using namespace ALife;

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
	// to remove them properly
	D_OBJECT_PAIR_IT			I = m_tObjectRegistry.begin();
	D_OBJECT_PAIR_IT			E = m_tObjectRegistry.end();
	for ( ; I != E; ++I)	
	{
		CSE_Abstract*			A	= dynamic_cast<CSE_Abstract*>((*I).second);
		xr_delete				(A);
	}
}

void CSE_ALifeObjectRegistry::Save(IWriter &tMemoryStream)
{
	tMemoryStream.open_chunk	(OBJECT_CHUNK_DATA);
	tMemoryStream.w_u32			((u32)m_tObjectRegistry.size());
	D_OBJECT_PAIR_IT I			= m_tObjectRegistry.begin();
	D_OBJECT_PAIR_IT E			= m_tObjectRegistry.end();
	for ( ; I != E; ++I) {
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
	for (u32 i=0; i<dwCount; ++i) {
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
	
	m_tObjectRegistry.insert	(mk_pair(tpALifeDynamicObject->ID,tpALifeDynamicObject));
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
	m_tEventRegistry.insert		(mk_pair(tpEvent->m_tEventID = m_tEventID++,tpEvent));
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
	for ( ; I != E; ++I)
		Update((*I).second);
}

void CSE_ALifeTaskRegistry::Add	(CSE_ALifeTask	*tpTask)
{
	if (m_tTaskRegistry.find(tpTask->m_tTaskID) != m_tTaskRegistry.end())
		R_ASSERT2				(true,"The specified task is already presented in the Task Registry!");
	m_tTaskRegistry.insert		(mk_pair(tpTask->m_tTaskID = m_tTaskID++,tpTask));
	Update						(tpTask);
}

void CSE_ALifeTaskRegistry::Update(CSE_ALifeTask	*tpTask)
{
	R_ASSERT2					(tpfGetTaskByID(tpTask->m_tTaskID) == tpTask,"Cannot find a specified task in the Task registry!");
	OBJECT_TASK_PAIR_IT			J = m_tTaskCrossMap.find(tpTask->m_tCustomerID);
	if (m_tTaskCrossMap.end() == J) {
		m_tTaskCrossMap.insert	(mk_pair(tpTask->m_tCustomerID,TASK_SET()));
		J						= m_tTaskCrossMap.find(tpTask->m_tCustomerID);
	}
	(*J).second.insert			(tpTask->m_tTaskID);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeGraphRegistry
////////////////////////////////////////////////////////////////////////////
CSE_ALifeGraphRegistry::CSE_ALifeGraphRegistry()
{
	m_tpCurrentLevel			= 0;
}

CSE_ALifeGraphRegistry::~CSE_ALifeGraphRegistry()
{
	xr_delete					(m_tpCurrentLevel);
}

void CSE_ALifeGraphRegistry::Init()
{
	for (int i=0; i<LOCATION_TYPE_COUNT; ++i) {
		{
			for (int j=0; j<LOCATION_COUNT; ++j)
				m_tpTerrain[i][j].clear();
		}
		for (_GRAPH_ID j=0; j<(_GRAPH_ID)ai().game_graph().header().vertex_count(); ++j)
			m_tpTerrain[i][ai().game_graph().vertex(j)->vertex_type()[i]].push_back(j);
	}

	m_tpGraphObjects.resize		(ai().game_graph().header().vertex_count());
	{
		GRAPH_POINT_IT			I = m_tpGraphObjects.begin();
		GRAPH_POINT_IT			E = m_tpGraphObjects.end();
		for ( ; I != E; ++I) {
			(*I).tpObjects.clear();
			(*I).tpEvents.clear	();
		}
	}
	
	xr_delete						(m_tpCurrentLevel);
	
	m_tpCurrentLevel			= 0;
	m_tpActor					= 0;
	m_tNextFirstSwitchObjectID	= _OBJECT_ID(-1);
}

void CSE_ALifeGraphRegistry::Update(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	if (!tpALifeDynamicObject->m_bDirectControl)
		return;
	if (tpALifeDynamicObject->s_flags.is(M_SPAWN_OBJECT_ASPLAYER)) {
		m_tpActor = dynamic_cast<CSE_ALifeCreatureActor*>(tpALifeDynamicObject);
		R_ASSERT2			(m_tpActor,"Invalid flag M_SPAWN_OBJECT_ASPLAYER for non-actor object!");
	}

	if (m_tpActor && !m_tpCurrentLevel) {
		m_tpCurrentLevel			= xr_new<D_OBJECT_P_MAP>();
		m_tCurrentLevelID			= ai().game_graph().vertex(m_tpActor->m_tGraphID)->level_id();
		for (int i=0, n=ai().game_graph().header().vertex_count(); i<n; ++i)
			if (ai().game_graph().vertex(i)->level_id() == m_tCurrentLevelID) {
				D_OBJECT_PAIR_IT	I = m_tpGraphObjects[i].tpObjects.begin();
				D_OBJECT_PAIR_IT	E = m_tpGraphObjects[i].tpObjects.end();
				for ( ; I != E; ++I)
					m_tpCurrentLevel->insert(mk_pair((*I).first,(*I).second));
			}
	}

	CSE_ALifeInventoryItem *l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(tpALifeDynamicObject);
	if (!l_tpALifeInventoryItem || (l_tpALifeInventoryItem && !l_tpALifeInventoryItem->bfAttached()))
		vfAddObjectToGraphPoint(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);
}

void CSE_ALifeGraphRegistry::vfAddObjectToCurrentLevel(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg("[LSS] adding object [%s][%d] to current level",tpALifeDynamicObject->s_name_replace,tpALifeDynamicObject->ID);
	}
#endif
	D_OBJECT_PAIR_IT			I = m_tpCurrentLevel->find(tpALifeDynamicObject->ID);
	R_ASSERT2					(m_tpCurrentLevel->end() == I,"Specified object has been already found in the current level map");
	m_tpCurrentLevel->insert	(mk_pair(tpALifeDynamicObject->ID,tpALifeDynamicObject));
	m_bSwitchChanged			= true;
	if (_OBJECT_ID(-1) == m_tNextFirstSwitchObjectID)
		m_tNextFirstSwitchObjectID = tpALifeDynamicObject->ID;
}

void CSE_ALifeGraphRegistry::vfRemoveObjectFromCurrentLevel(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] removing object [%s][%d] from current level",tpALifeDynamicObject->s_name_replace,tpALifeDynamicObject->ID);
	}
#endif
	D_OBJECT_PAIR_IT			I = m_tpCurrentLevel->find(tpALifeDynamicObject->ID), J = I;
	R_ASSERT2					(m_tpCurrentLevel->end() != I,"Specified object hasn't been already found in the current level map");
	if (m_tNextFirstSwitchObjectID == tpALifeDynamicObject->ID) {
		if (m_tpCurrentLevel->end() == ++J)
			J					=	m_tpCurrentLevel->begin();
		if (m_tpCurrentLevel->end() != J)
			m_tNextFirstSwitchObjectID	= (*J).second->ID;
		else
			m_tNextFirstSwitchObjectID	= _OBJECT_ID(-1);
	}
	m_tpCurrentLevel->erase		(I);
	m_bSwitchChanged			= true;
}

void CSE_ALifeGraphRegistry::vfAddObjectToGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tNextGraphPointID, bool bUpdateSwitchObjects)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] adding object [%s][%d] to graph point %d",tpALifeDynamicObject->s_name_replace,tpALifeDynamicObject->ID,tNextGraphPointID);
	}
#endif
	if (!dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject)) {
		R_ASSERT2					(!dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject),"Can't add a trader to the graph point!");
		D_OBJECT_PAIR_IT			I = m_tpGraphObjects[tNextGraphPointID].tpObjects.find(tpALifeDynamicObject->ID);
		R_ASSERT3					(m_tpGraphObjects[tNextGraphPointID].tpObjects.end() == I,"Specified object has already found on the given graph point!",tpALifeDynamicObject->s_name_replace);
		m_tpGraphObjects[tNextGraphPointID].tpObjects.insert(mk_pair(tpALifeDynamicObject->ID,tpALifeDynamicObject));
		tpALifeDynamicObject->m_tGraphID = tNextGraphPointID;
	}
	
	if (bUpdateSwitchObjects && m_tpCurrentLevel && (m_tCurrentLevelID == ai().game_graph().vertex(tpALifeDynamicObject->m_tGraphID)->level_id()))
		vfAddObjectToCurrentLevel(tpALifeDynamicObject);
}

void CSE_ALifeGraphRegistry::vfRemoveObjectFromGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphID, bool bUpdateSwitchObjects)
{
	if (dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject))
		return;

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] removing object [%s][%d] from graph point %d",tpALifeDynamicObject->s_name_replace,tpALifeDynamicObject->ID,tGraphID);
	}
#endif
	D_OBJECT_PAIR_IT			I = m_tpGraphObjects[tGraphID].tpObjects.find(tpALifeDynamicObject->ID), J;
	R_ASSERT3					(m_tpGraphObjects[tGraphID].tpObjects.end() != I,"Specified object not found on the given graph point!",tpALifeDynamicObject->s_name_replace);
	m_tpGraphObjects[tGraphID].tpObjects.erase(I);
	
	if (bUpdateSwitchObjects && m_tpCurrentLevel && (m_tCurrentLevelID == ai().game_graph().vertex(tpALifeDynamicObject->m_tGraphID)->level_id()))
		vfRemoveObjectFromCurrentLevel(tpALifeDynamicObject);
}

void CSE_ALifeGraphRegistry::vfChangeObjectGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
{
	vfRemoveObjectFromGraphPoint		(tpALifeDynamicObject,tGraphPointID);
	vfAddObjectToGraphPoint				(tpALifeDynamicObject,tNextGraphPointID);
	tpALifeDynamicObject->m_tGraphID	= tNextGraphPointID;
	tpALifeDynamicObject->o_Position	= ai().game_graph().vertex(tpALifeDynamicObject->m_tGraphID)->level_point();
	tpALifeDynamicObject->m_tNodeID		= ai().game_graph().vertex(tpALifeDynamicObject->m_tGraphID)->level_vertex_id();
}

// events
void CSE_ALifeGraphRegistry::vfRemoveEventFromGraphPoint(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphID)
{
	EVENT_PAIR_IT				I = m_tpGraphObjects[tGraphID].tpEvents.find(tpEvent->m_tEventID);
	R_ASSERT2					(m_tpGraphObjects[tGraphID].tpEvents.end() != I,"Specified object not found on the given graph point!");
	m_tpGraphObjects[tGraphID].tpEvents.erase(I);
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] removing event [%d] from graph point %d",tpEvent->m_tEventID,tGraphID);
	}
#endif
}

void CSE_ALifeGraphRegistry::vfAddEventToGraphPoint(CSE_ALifeEvent *tpEvent, _GRAPH_ID tNextGraphPointID)
{
	EVENT_PAIR_IT				I = m_tpGraphObjects[tNextGraphPointID].tpEvents.find(tpEvent->m_tEventID);
	R_ASSERT2					(m_tpGraphObjects[tNextGraphPointID].tpEvents.end() == I,"Specified object has been already found on the given graph point!");
	m_tpGraphObjects[tNextGraphPointID].tpEvents.insert(mk_pair(tpEvent->m_tEventID,tpEvent));
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] adding event [%d] from graph point %d",tpEvent->m_tEventID,tNextGraphPointID);
	}
#endif
}

void CSE_ALifeGraphRegistry::vfChangeEventGraphPoint(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
{
	vfRemoveEventFromGraphPoint	(tpEvent,tGraphPointID);
	vfAddEventToGraphPoint		(tpEvent,tNextGraphPointID);
}

void CSE_ALifeGraphRegistry::vfAttachItem(CSE_Abstract &tAbstract, CSE_ALifeInventoryItem *tpALifeInventoryItem, _GRAPH_ID tGraphID, bool bALifeRequest)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] Attaching item [%s][%d] to [%s][%d]",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID,tAbstract.s_name_replace,tAbstract.ID);
	}
#endif
	if (bALifeRequest)
		vfRemoveObjectFromGraphPoint	(dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeInventoryItem),tGraphID);
	else
		vfRemoveObjectFromCurrentLevel	(dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeInventoryItem));

	CSE_ALifeTraderAbstract		*l_tpALifeTraderAbstract = dynamic_cast<CSE_ALifeTraderAbstract*>(&tAbstract);
	R_ASSERT2					(!bALifeRequest || l_tpALifeTraderAbstract,"Cannot attach an item to a non-trader object");

	if (l_tpALifeTraderAbstract)
		l_tpALifeTraderAbstract->vfAttachItem(tpALifeInventoryItem,bALifeRequest);
}

void CSE_ALifeGraphRegistry::vfDetachItem(CSE_Abstract &tAbstract, CSE_ALifeInventoryItem *tpALifeInventoryItem, _GRAPH_ID tGraphID, bool bALifeRequest)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] Detaching item [%s][%d] from [%s][%d]",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID,tAbstract.s_name_replace,tAbstract.ID);
	}
#endif
	if (bALifeRequest)
		vfAddObjectToGraphPoint		(dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeInventoryItem),tGraphID);
	else
		vfAddObjectToCurrentLevel	(dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeInventoryItem));

	CSE_ALifeTraderAbstract		*l_tpALifeTraderAbstract = dynamic_cast<CSE_ALifeTraderAbstract*>(&tAbstract);
	R_ASSERT2					(!bALifeRequest || l_tpALifeTraderAbstract,"Cannot detach an item from non-trader object");

	if (l_tpALifeTraderAbstract)
		l_tpALifeTraderAbstract->vfDetachItem(tpALifeInventoryItem,0,bALifeRequest);
	else
		R_ASSERT2				(std::find(tAbstract.children.begin(),tAbstract.children.end(),tpALifeInventoryItem->ID) != tAbstract.children.end(),"Can't detach an item which is not on my own");
}

void CSE_ALifeGraphRegistry::vfAssignGraphPosition(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	tpALifeMonsterAbstract->m_tNextGraphID					= tpALifeMonsterAbstract->m_tPrevGraphID = tpALifeMonsterAbstract->m_tGraphID;
	tpALifeMonsterAbstract->m_fDistanceToPoint				= tpALifeMonsterAbstract->m_fDistance;
	CGameGraph::const_iterator								i,e;
	ai().game_graph().begin									(tpALifeMonsterAbstract->m_tNextGraphID,i,e);
	for ( ; i != e; ++i)
		if ((*i).distance() > tpALifeMonsterAbstract->m_fDistance) {
			tpALifeMonsterAbstract->m_fDistanceFromPoint	= (*i).distance() - tpALifeMonsterAbstract->m_fDistance;
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
	float			fBestDistance = dInfinity;
	CSE_ALifeTrader *tpBestTrader = 0;
	TRADER_P_IT		I = m_tpTraders.begin();
	TRADER_P_IT		E = m_tpTraders.end();
	const Fvector	&tGlobalPoint = ai().game_graph().vertex(tpALifeHumanAbstract->m_tGraphID)->game_point();
	for ( ; I != E; ++I) {
//		if ((*I)->m_tRank != tpALifeHumanAbstract->m_tRank)
//			break;
		float		fCurDistance = ai().game_graph().vertex((*I)->m_tGraphID)->game_point().distance_to(tGlobalPoint);
		if (fCurDistance < fBestDistance) {
			fBestDistance	= fCurDistance;
			tpBestTrader	= *I;
		}
	}
	R_ASSERT2		(tpBestTrader,"There is no traders in the game");
	return			(tpBestTrader);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeScheduleRegistry
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeScheduleRegistry::Init()
{
	m_tpScheduledObjects.clear	();
	m_tNextFirstProcessObjectID	= _OBJECT_ID(-1);
}

void CSE_ALifeScheduleRegistry::Update(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	if (!tpALifeDynamicObject->m_bDirectControl)
		return;
	vfAddObjectToScheduled		(tpALifeDynamicObject);
}

void CSE_ALifeScheduleRegistry::vfAddObjectToScheduled(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bUpdateSchedulableObjects)
{
	R_ASSERT2					(!tpALifeDynamicObject->m_bOnline,"Can't add to scheduled objects online object!");
	
	CSE_ALifeSchedulable		*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeSchedulable*>(tpALifeDynamicObject);
	if (!l_tpALifeSchedulable)
		return;

	CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeDynamicObject);
	if (l_tpALifeMonsterAbstract && (l_tpALifeMonsterAbstract->fHealth <= 0))
		return;

	CSE_ALifeAnomalousZone		*l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeDynamicObject);
#pragma todo("Dima to Dima : Change this line if CSE_ALifeAnomalousZone::Update() is not empty, this is just a performance trick")
	//	if (l_tpALifeAnomalousZone && (l_tpALifeAnomalousZone->m_maxPower < EPS_L))
	if (l_tpALifeAnomalousZone)
		return;

	if (!bUpdateSchedulableObjects)
		return;
	SCHEDULE_P_PAIR_IT			I = m_tpScheduledObjects.find(tpALifeDynamicObject->ID);
	R_ASSERT2					(m_tpScheduledObjects.end() == I,"Specified object has been already found in the scheduled objects registry");
	m_tpScheduledObjects.insert	(mk_pair(l_tpALifeSchedulable->ID,l_tpALifeSchedulable));
	m_bUpdateChanged			= true;

	if (_OBJECT_ID(-1) == m_tNextFirstProcessObjectID)
		m_tNextFirstProcessObjectID = l_tpALifeSchedulable->ID;
}

void CSE_ALifeScheduleRegistry::vfRemoveObjectFromScheduled(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bUpdateSchedulableObjects)
{
//	R_ASSERT2					(tpALifeDynamicObject->m_bOnline || (dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeDynamicObject) && (dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeDynamicObject)->fHealth <= 0)	|| (dynamic_cast<CSE_ALifeGroupAbstract*>(tpALifeDynamicObject) && dynamic_cast<CSE_ALifeGroupAbstract*>(tpALifeDynamicObject)->m_tpMembers.empty())),"Can't remove from scheduled objects offline object!");
	
	CSE_ALifeSchedulable		*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeMonsterAbstract *>(tpALifeDynamicObject);
	if (!l_tpALifeSchedulable)
		return;

	CSE_ALifeAnomalousZone		*l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeDynamicObject);
	if (l_tpALifeAnomalousZone && (l_tpALifeAnomalousZone->m_maxPower < EPS_L))
		return;

	CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract *>(tpALifeDynamicObject);
	if (tpALifeMonsterAbstract && (tpALifeMonsterAbstract->fHealth <= 0) && (m_tpScheduledObjects.find(tpALifeDynamicObject->ID) == m_tpScheduledObjects.end()))
		return;

	if (!bUpdateSchedulableObjects)
		return;
	SCHEDULE_P_PAIR_IT			I = m_tpScheduledObjects.find(l_tpALifeSchedulable->ID), J = I;
	R_ASSERT2					(m_tpScheduledObjects.end() != I,"Specified object hasn't been found in the scheduled objects registry!");
	
	if (m_tNextFirstProcessObjectID == l_tpALifeSchedulable->ID) {
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg							("[LSS] Changing next schedulable object (%d)",m_tNextFirstProcessObjectID);
		}
#endif
		VERIFY					(!m_tpScheduledObjects.empty());
		if (m_tpScheduledObjects.end() == ++J)
			J = m_tpScheduledObjects.begin();
		if ((m_tpScheduledObjects.end() != J) && ((*J).second->ID != m_tNextFirstProcessObjectID))
			m_tNextFirstProcessObjectID	= (*J).second->ID;
		else
			m_tNextFirstProcessObjectID	= _OBJECT_ID(-1);
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg							("[LSS] Changing next schedulable object (%d)",m_tNextFirstProcessObjectID);
		}
#endif
	}
	
	m_tpScheduledObjects.erase	(I);
	m_bUpdateChanged			= true;
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
	// to remove them properly
	D_OBJECT_P_IT			I = m_tpSpawnPoints.begin();
	D_OBJECT_P_IT			E = m_tpSpawnPoints.end();
	for ( ; I != E; ++I) {
		CSE_Abstract*			A	= dynamic_cast<CSE_Abstract*>(*I);
		xr_delete				(A);
	}
}

void CSE_ALifeSpawnRegistry::Init()
{
	// since we use multiple inheritance, we have to cast the objects to the underlying class
	// to remove them properly
	D_OBJECT_P_IT			I = m_tpSpawnPoints.begin();
	D_OBJECT_P_IT			E = m_tpSpawnPoints.end();
	for ( ; I != E; ++I) {
		CSE_Abstract*			A	= dynamic_cast<CSE_Abstract*>(*I);
		xr_delete				(A);
	}
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
	for (int id=0; I != E; ++I, ++id) {
		R_ASSERT2				(0!=(S = tFileStream.open_chunk(id)),"Can't find entity chunk in the 'game.spawn'");
		// Spawn
		tNetPacket.B.count		= S->r_u16();
		S->r					(tNetPacket.B.data,tNetPacket.B.count);
		tNetPacket.r_begin		(ID);
		R_ASSERT2				(M_SPAWN == ID,"Invalid packet ID (!= M_SPAWN)!");

		string64				s_name;
		tNetPacket.r_string		(s_name);
		Msg						("Loading spawn point %s",s_name);
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

		R_ASSERT2				((GAME_SINGLE == E->s_gameid) || (GAME_ANY == E->s_gameid),"Invalid game type!");
		R_ASSERT2				(0 != (*I = dynamic_cast<CSE_ALifeDynamicObject*>(E)),"Non-ALife object in the 'game.spawn'");
		R_ASSERT3				(!((*I)->used_ai_locations()) || ((*I)->m_tNodeID != u32(-1)),"Invalid vertex for object ",(*I)->s_name_replace);
		
		// building map of sets : get all the zone types which can generate given artefact
		CSE_ALifeAnomalousZone	*l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(E);
		if (l_tpALifeAnomalousZone) {
			EAnomalousZoneType	l_tAnomalousZoneType = l_tpALifeAnomalousZone->m_tAnomalyType;
			for (u16 i=0, n = l_tpALifeAnomalousZone->m_wItemCount; i<n; ++i) {
				ITEM_SET_PAIR_IT	I = m_tArtefactAnomalyMap.find(l_tpALifeAnomalousZone->m_cppArtefactSections[i]);
				if (m_tArtefactAnomalyMap.end() != I)
					(*I).second.insert(l_tAnomalousZoneType);
				else {
					m_tArtefactAnomalyMap.insert(mk_pair(l_tpALifeAnomalousZone->m_cppArtefactSections[i],U32_SET()));
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
	for (u32 k = 0; pSettings->r_line("organizations",k,&N,&V); ++k)
		m_tOrganizationRegistry.insert(mk_pair(N,xr_new<CSE_ALifeOrganization>(N)));

	ORGANIZATION_P_PAIR_IT		I = m_tOrganizationRegistry.begin();
	ORGANIZATION_P_PAIR_IT		E = m_tOrganizationRegistry.end();
	for ( ; I != E; ++I) {
		LPSTR_IT				i = (*I).second->m_tpPossibleDiscoveries.begin();
		LPSTR_IT				e = (*I).second->m_tpPossibleDiscoveries.end();
		for ( ; i != e; ++i) {
			DISCOVERY_P_PAIR_IT j = m_tDiscoveryRegistry.find(*i);
			if (m_tDiscoveryRegistry.end() == j)
				m_tDiscoveryRegistry.insert(mk_pair(*i,xr_new<CSE_ALifeDiscovery>(*i)));
		}
	}
	LPCSTR						S = pSettings->r_string("alife","preknown_artefacts");
	for (u32 i=0, n=_GetItemCount(S); i<n; ++i) {
		string64				S1;
		_GetItem				(S,i,S1);
		LPSTR					S2 = (LPSTR)xr_malloc((xr_strlen(S1) + 1)*sizeof(char));
		strcpy					(S2,S1);
		m_tArtefactRegistry.insert(mk_pair(S2,false));
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
