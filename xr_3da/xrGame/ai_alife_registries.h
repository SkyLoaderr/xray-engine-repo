////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_server_objects.h"
#include "ai_alife_predicates.h"
#include "ai_alife_a_star.h"

using namespace ALife;

class CALifeObjectRegistry : public IPureALifeLSObject {
public:
	OBJECT_MAP						m_tObjectRegistry;		// список событий игры

	CALifeObjectRegistry()
	{
		m_tObjectRegistry.clear		();
	};

	virtual							~CALifeObjectRegistry()
	{
		free_map					(m_tObjectRegistry);
	};
	
	virtual	void					Save(IWriter &tMemoryStream)
	{
		tMemoryStream.open_chunk	(OBJECT_CHUNK_DATA);
		tMemoryStream.w_u32			(m_tObjectRegistry.size());
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
	};
	
	void CALifeObjectRegistry::Load(IReader	&tFileStream)
	{ 
		R_ASSERT(tFileStream.find_chunk(OBJECT_CHUNK_DATA));
		m_tObjectRegistry.clear();
		u32 dwCount = tFileStream.r_u32();
		for (u32 i=0; i<dwCount; i++) {
			NET_Packet				tNetPacket;
			u16						u_id;
			// Spawn
			tNetPacket.B.count		= tFileStream.r_u16();
			tFileStream.r			(tNetPacket.B.data,tNetPacket.B.count);
			tNetPacket.r_begin		(u_id);
			R_ASSERT				(M_SPAWN==u_id);

			string64				s_name;
			tNetPacket.r_string		(s_name);
			// create entity
			xrServerEntity			*tpServerEntity = F_entity_Create	(s_name);
			R_ASSERT2				(tpServerEntity,"Can't create entity.");
			CALifeDynamicObject		*tpALifeDynamicObject = dynamic_cast<CALifeDynamicObject*>(tpServerEntity);
			R_ASSERT				(tpALifeDynamicObject);
			tpALifeDynamicObject->Spawn_Read(tNetPacket);

			// Update
			tNetPacket.B.count		= tFileStream.r_u16();
			tFileStream.r			(tNetPacket.B.data,tNetPacket.B.count);
			tNetPacket.r_begin		(u_id);
			R_ASSERT				(M_UPDATE==u_id);
			tpALifeDynamicObject->UPDATE_Read(tNetPacket);
			tpALifeDynamicObject->Init(tpALifeDynamicObject->s_name);

			m_tObjectRegistry.insert(make_pair(tpALifeDynamicObject->m_tObjectID,tpALifeDynamicObject));
		}
	};

	IC bool bfCheckIfTaskCompleted(xrServerEntity &tServerEntity, CALifeHumanAbstract *tpALifeHumanAbstract, OBJECT_IT &I)
	{
		if (tpALifeHumanAbstract->m_dwCurTask >= tpALifeHumanAbstract->m_tpTasks.size())
			return(false);
		I = tServerEntity.children.begin();
		OBJECT_IT	E = tServerEntity.children.end();
		CALifePersonalTask	&tPersonalTask = *(tpALifeHumanAbstract->m_tpTasks[tpALifeHumanAbstract->m_dwCurTask]);
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
					if (m_tObjectRegistry[*I]->m_tObjectID == tPersonalTask.m_tObjectID)
						return(true);
					break;
				}
			};
		}
		return(false);
	};

	IC bool bfCheckIfTaskCompleted(CALifeHuman *tpALifeHuman, OBJECT_IT &I)
	{
		return(bfCheckIfTaskCompleted(*tpALifeHuman,tpALifeHuman,I));
	};

	IC bool bfCheckIfTaskCompleted(CALifeHuman *tpALifeHuman)
	{
		OBJECT_IT I;
		return(bfCheckIfTaskCompleted(tpALifeHuman,I));
	};
};

class CALifeEventRegistry : public IPureALifeLSObject {
public:
	_EVENT_ID						m_tEventID;				// идентификатор карты событий
	EVENT_MAP						m_tEventRegistry;		// список событий игры

	CALifeEventRegistry()
	{
		m_tEventID					= 0;
		m_tEventRegistry.clear		();
	};

	virtual							~CALifeEventRegistry()
	{
		free_map					(m_tEventRegistry);
	};
	
	virtual	void					Save(IWriter &tMemoryStream)
	{
		tMemoryStream.open_chunk	(EVENT_CHUNK_DATA);
		tMemoryStream.w				(&m_tEventID,sizeof(m_tEventID));
		//save_map					(m_tEventRegistry,tMemoryStream);
		tMemoryStream.close_chunk	();
	};

	virtual	void					Load(IReader	&tFileStream)
	{
		R_ASSERT(tFileStream.find_chunk(EVENT_CHUNK_DATA));
		tFileStream.r	(&m_tEventID,sizeof(m_tEventID));
		//load_map					(m_tEventRegistry,tFileStream,tfChooseEventKeyPredicate);
	};
	
	virtual	void					Add	(CALifeEvent	*tpEvent)
	{
		m_tEventRegistry.insert		(make_pair(tpEvent->m_tEventID = m_tEventID++,tpEvent));
	};
};

class CALifeTaskRegistry : public IPureALifeLSObject {
public:
	_TASK_ID						m_tTaskID;				// идентификатор карты событий
	TASK_MAP						m_tTaskRegistry;		// список событий игры

	CALifeTaskRegistry()
	{
		m_tTaskID					= 0;
		m_tTaskRegistry.clear		();
	};

	virtual							~CALifeTaskRegistry()
	{
		free_map					(m_tTaskRegistry);
	};
	
	virtual	void					Save(IWriter &tMemoryStream)
	{
		tMemoryStream.open_chunk	(TASK_CHUNK_DATA);
		tMemoryStream.w				(&m_tTaskID,sizeof(m_tTaskID));
		//save_map					(m_tTaskRegistry,tMemoryStream);
		tMemoryStream.close_chunk	();
	};
	
	virtual	void					Load(IReader	&tFileStream)
	{
		R_ASSERT(tFileStream.find_chunk(TASK_CHUNK_DATA));
		tFileStream.r				(&m_tTaskID,sizeof(m_tTaskID));
//		load_map					(m_tTaskRegistry,tFileStream,tfChooseTaskKeyPredicate);
	};
	 
	virtual	void					Add	(CALifeTask	*tpTask)
	{
		m_tTaskRegistry.insert		(make_pair(tpTask->m_tTaskID = m_tTaskID++,tpTask));
	};
};

class CALifeGraphRegistry : public CALifeAStar {
public:
	typedef CALifeAStar inherited;
	ALIFE_ENTITY_P_VECTOR_MAP		m_tLevelMap;
	CALifeDynamicObject				*m_tpActor;
	ALIFE_ENTITY_P_VECTOR			*m_tpCurrentLevel;
	GRAPH_POINT_VECTOR				m_tpGraphObjects;		// по точке графа получить все 
	GRAPH_VECTOR_SVECTOR			m_tpTerrain[LOCATION_TYPE_COUNT];			// массив списков: по идетнификатору 
    														//	местности получить список точек 
															//  графа
	game_sv_GameState				*m_tpGame;

									CALifeGraphRegistry(game_sv_GameState *tpGame) : CALifeAStar()
	{
		VERIFY(tpGame);
		m_tpGame = tpGame;
	}
	
	void							Init()
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
	};

	IC void							Update(CALifeDynamicObject *tpALifeDynamicObject)
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
			m_tLevelMap.insert(make_pair(dwLevelID,tpTemp));
		}
		else
			(*I).second->push_back(tpALifeDynamicObject);
		
		if (m_tpActor && !m_tpCurrentLevel) {
			I = m_tLevelMap.find(getAI().m_tpaGraph[m_tpActor->m_tGraphID].tLevelID);
			R_ASSERT(I != m_tLevelMap.end());
			m_tpCurrentLevel = (*I).second;
		}
		
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem) {
			if (!tpALifeItem->bfAttached())
				m_tpGraphObjects[tpALifeItem->m_tGraphID].tpObjects.push_back(tpALifeItem);
			return;
		}
	
		if (!dynamic_cast<CALifeTrader *>(tpALifeDynamicObject))
			m_tpGraphObjects[tpALifeDynamicObject->m_tGraphID].tpObjects.push_back(tpALifeDynamicObject);
	}

	IC void vfRemoveObjectFromGraphPoint(CALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphID)
	{
		ALIFE_ENTITY_P_IT				I = m_tpGraphObjects[tGraphID].tpObjects.begin();
		ALIFE_ENTITY_P_IT				E = m_tpGraphObjects[tGraphID].tpObjects.end();
		for ( ; I != E; I++)
			if ((*I) == tpALifeDynamicObject) {
				m_tpGraphObjects[tGraphID].tpObjects.erase(I);
				break;
			}
	};
	
	IC void vfAddObjectToGraphPoint(CALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].tpObjects.push_back(tpALifeDynamicObject);
	};

	IC void vfChangeObjectGraphPoint(CALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveObjectFromGraphPoint	(tpALifeDynamicObject,tGraphPointID);
		vfAddObjectToGraphPoint			(tpALifeDynamicObject,tNextGraphPointID);
		tpALifeDynamicObject->m_tGraphID	= tNextGraphPointID;
		tpALifeDynamicObject->o_Position	= getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tLocalPoint;
		tpALifeDynamicObject->m_tNodeID		= getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tNodeID;
	};

	// events
	IC void vfRemoveEventFromGraphPoint(CALifeEvent *tpEvent, _GRAPH_ID tGraphID)
	{
		EVENT_P_IT						I = m_tpGraphObjects[tGraphID].tpEvents.begin();
		EVENT_P_IT						E = m_tpGraphObjects[tGraphID].tpEvents.end();
		for ( ; I != E; I++)
			if ((*I) == tpEvent) {
				m_tpGraphObjects[tGraphID].tpEvents.erase(I);
				break;
			}
	};
	
	IC void vfAddEventToGraphPoint(CALifeEvent *tpEvent, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].tpEvents.push_back(tpEvent);
	};

	IC void vfChangeEventGraphPoint(CALifeEvent *tpEvent, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveEventFromGraphPoint	(tpEvent,tGraphPointID);
		vfAddEventToGraphPoint		(tpEvent,tNextGraphPointID);
	};

	IC void vfAttachItem(xrServerEntity &tServerEntity, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bGenEvent = true)
	{
		if (bGenEvent) {
			NET_Packet		P;
			m_tpGame->u_EventGen(P,GE_OWNERSHIP_TAKE,tServerEntity.ID);
			P.w_u16			(u16(tpALifeItem->ID));
			m_tpGame->u_EventSend(P);
		}
		
		CALifeTraderParams *tpALifeTraderParams = dynamic_cast<CALifeTraderParams*>(&tServerEntity);
		VERIFY(tpALifeTraderParams);
		ALIFE_ENTITY_P_IT		I = m_tpGraphObjects[tGraphID].tpObjects.begin();
		ALIFE_ENTITY_P_IT		E = m_tpGraphObjects[tGraphID].tpObjects.end();
		for ( ; I != E; I++)
			if (*I == tpALifeItem) {
				m_tpGraphObjects[tGraphID].tpObjects.erase(I);
				break;
			}
		tpALifeTraderParams->m_fCumulativeItemMass += tpALifeItem->m_fMass;
//		tServerEntity.children.push_back(tpALifeItem->ID);
//		vector<u16>::iterator i = tServerEntity.children.begin();
//		vector<u16>::iterator e = tServerEntity.children.end();
//		bool bOk = true;
//		for ( ; i != e; i++)
//			if (*i == tpALifeItem->ID) {
//				bOk = false;
//				break;
//			}
//		VERIFY(bOk);
//		tpALifeItem->ID_Parent = tServerEntity.ID;
	}

	IC void vfDetachItem(xrServerEntity &tServerEntity, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bGenEvent = true)
	{
		if (bGenEvent) {
			NET_Packet		P;
			m_tpGame->u_EventGen(P,GE_OWNERSHIP_REJECT,tServerEntity.ID);
			P.w_u16			(u16(tpALifeItem->ID));
			m_tpGame->u_EventSend(P);
		}
		CALifeTraderParams *tpTraderParams = dynamic_cast<CALifeTraderParams*>(&tServerEntity);
		VERIFY(tpTraderParams);
		m_tpGraphObjects[tGraphID].tpObjects.push_back(tpALifeItem);
		tpTraderParams->m_fCumulativeItemMass -= tpALifeItem->m_fMass;
//		tpALifeItem->ID_Parent = 0xffff;
//		
//		vector<u16>::iterator i = tServerEntity.children.begin();
//		vector<u16>::iterator e = tServerEntity.children.end();
//		bool bOk = false;
//		for ( ; i != e; i++)
//			if (*i == tpALifeItem->ID) {
//				tServerEntity.children.erase(i);
//				bOk = true;
//				break;
//			}
//		VERIFY(bOk);
	}
};

class CALifeTraderRegistry {
public:
	TRADER_P_VECTOR					m_tpTraders;			// массив торговцев

	void							Init()
	{
		m_tpTraders.clear			();
	};
	
	IC void							Update(CALifeDynamicObject *tpALifeDynamicObject)
	{
		CALifeTrader *tpALifeTrader = dynamic_cast<CALifeTrader *>(tpALifeDynamicObject);
		if (tpALifeTrader) {
			m_tpTraders.push_back(tpALifeTrader);
			sort(m_tpTraders.begin(),m_tpTraders.end(),CCompareTraderRanksPredicate());
		}
	};
};

class CALifeScheduleRegistry {
public:
	ALIFE_MONSTER_P_VECTOR			m_tpScheduledObjects;	// массив обновляемых объектов

	void							Init()
	{
		m_tpScheduledObjects.clear	();
	};

	IC void							Update(CALifeDynamicObject *tpALifeDynamicObject)
	{
		if (!tpALifeDynamicObject->m_bDirectControl)
			return;
		CALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CALifeMonsterAbstract *>(tpALifeDynamicObject);
		if (tpALifeMonsterAbstract && (tpALifeMonsterAbstract->fHealth > 0))
			m_tpScheduledObjects.push_back	(tpALifeMonsterAbstract);
	};	
};

class CALifeSpawnRegistry : public CALifeSpawnHeader {
public:
	typedef CALifeSpawnHeader inherited;
	
	ALIFE_ENTITY_P_VECTOR			m_tpSpawnPoints;
	
	virtual							~CALifeSpawnRegistry()
	{
		free_vector					(m_tpSpawnPoints);
	};
	
	virtual void					Load(IReader	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_tpSpawnPoints.resize	(m_dwSpawnCount);
		ALIFE_ENTITY_P_IT			I = m_tpSpawnPoints.begin();
		ALIFE_ENTITY_P_IT			E = m_tpSpawnPoints.end();
		NET_Packet					tNetPacket;
		IReader						*S = 0;
		u16							ID;
		for (int id=0; I != E; I++, id++) {
			R_ASSERT				(0!=(S = tFileStream.open_chunk(id)));
			// Spawn
			tNetPacket.B.count		= S->r_u16();
			S->r					(tNetPacket.B.data,tNetPacket.B.count);
			tNetPacket.r_begin		(ID);
			R_ASSERT				(M_SPAWN == ID);
			
			string64				s_name;
			tNetPacket.r_string		(s_name);
			xrServerEntity			*E = F_entity_Create(s_name);

			R_ASSERT2				(E,"Can't create entity.");
			E->Spawn_Read			(tNetPacket);
			// Update
			tNetPacket.B.count		= S->r_u16();
			S->r					(tNetPacket.B.data,tNetPacket.B.count);
			tNetPacket.r_begin		(ID);
			R_ASSERT				(M_UPDATE == ID);
			E->UPDATE_Read			(tNetPacket);
			
			E->Init					(E->s_name);

			R_ASSERT				((E->s_gameid == GAME_SINGLE) || (E->s_gameid == GAME_ANY));
			R_ASSERT				((*I = dynamic_cast<CALifeDynamicObject*>(E)) != 0);
		}
	};
};