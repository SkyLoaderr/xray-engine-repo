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
//#include "ai_space.h"

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
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.open_chunk	(OBJECT_CHUNK_DATA);
		tMemoryStream.Wdword		(m_tObjectRegistry.size());
		OBJECT_PAIR_IT I			= m_tObjectRegistry.begin();
		OBJECT_PAIR_IT E			= m_tObjectRegistry.end();
		for ( ; I != E; I++) {
			NET_Packet				tNetPacket;
			// Spawn
			(*I).second->Spawn_Write(tNetPacket,TRUE);
			tMemoryStream.Wword		(u16(tNetPacket.B.count));
			tMemoryStream.write		(tNetPacket.B.data,tNetPacket.B.count);

			// Update
			tNetPacket.w_begin		(M_UPDATE);
			(*I).second->UPDATE_Write(tNetPacket);

			tMemoryStream.Wword		(u16(tNetPacket.B.count));
			tMemoryStream.write		(tNetPacket.B.data,tNetPacket.B.count);
		}
		tMemoryStream.close_chunk	();
	};
	
	void CALifeObjectRegistry::Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(OBJECT_CHUNK_DATA));
		m_tObjectRegistry.clear();
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			NET_Packet				tNetPacket;
			u16						u_id;
			// Spawn
			tNetPacket.B.count		= tFileStream.Rword();
			tFileStream.Read		(tNetPacket.B.data,tNetPacket.B.count);
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
			tNetPacket.B.count		= tFileStream.Rword();
			tFileStream.Read		(tNetPacket.B.data,tNetPacket.B.count);
			tNetPacket.r_begin		(u_id);
			R_ASSERT				(M_UPDATE==u_id);
			tpALifeDynamicObject->UPDATE_Read(tNetPacket);

			m_tObjectRegistry.insert(make_pair(tpALifeDynamicObject->m_tObjectID,tpALifeDynamicObject));
		}
	};

	IC bool bfCheckIfTaskCompleted(CALifeHumanParams &tHumanParams, CALifeHumanAbstract *tpALifeHumanAbstract, OBJECT_IT &I)
	{
		if (tpALifeHumanAbstract->m_dwCurTask >= tpALifeHumanAbstract->m_tpTasks.size())
			return(false);
		I = tHumanParams.m_tpItemIDs.begin();
		OBJECT_IT	E = tHumanParams.m_tpItemIDs.end();
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
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.open_chunk	(EVENT_CHUNK_DATA);
		tMemoryStream.write			(&m_tEventID,sizeof(m_tEventID));
		//save_map					(m_tEventRegistry,tMemoryStream);
		tMemoryStream.close_chunk	();
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(EVENT_CHUNK_DATA));
		tFileStream.Read(&m_tEventID,sizeof(m_tEventID));
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
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.open_chunk	(TASK_CHUNK_DATA);
		tMemoryStream.write			(&m_tTaskID,sizeof(m_tTaskID));
		//save_map					(m_tTaskRegistry,tMemoryStream);
		tMemoryStream.close_chunk	();
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(TASK_CHUNK_DATA));
		tFileStream.Read			(&m_tTaskID,sizeof(m_tTaskID));
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

	IC void vfAttachItem(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID)
	{
		tHumanParams.m_tpItemIDs.push_back(tpALifeItem->m_tObjectID);
		tpALifeItem->ID_Parent = tHumanParams.ID;
		ALIFE_ENTITY_P_IT		I = m_tpGraphObjects[tGraphID].tpObjects.begin();
		ALIFE_ENTITY_P_IT		E = m_tpGraphObjects[tGraphID].tpObjects.end();
		for ( ; I != E; I++)
			if (*I == tpALifeItem) {
				m_tpGraphObjects[tGraphID].tpObjects.erase(I);
				break;
			}
		tHumanParams.m_fCumulativeItemMass += tpALifeItem->m_fMass;
	}

	IC void vfDetachItem(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID)
	{
		tpALifeItem->ID = 65535;
		m_tpGraphObjects[tGraphID].tpObjects.push_back(tpALifeItem);
		tHumanParams.m_fCumulativeItemMass -= tpALifeItem->m_fMass;
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
		CALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CALifeMonsterAbstract *>(tpALifeDynamicObject);
		if (tpALifeMonsterAbstract && tpALifeMonsterAbstract->m_bDirectControl)
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
	
	virtual void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_tpSpawnPoints.resize	(m_dwSpawnCount);
		ALIFE_ENTITY_P_IT			I = m_tpSpawnPoints.begin();
		ALIFE_ENTITY_P_IT			E = m_tpSpawnPoints.end();
		NET_Packet					tNetPacket;
		CStream						*S = 0;
		u16							ID;
		for (int id=0; I != E; I++, id++) {
			R_ASSERT				(0!=(S = tFileStream.OpenChunk(id)));
			// Spawn
			tNetPacket.B.count		= S->Rword();
			S->Read					(tNetPacket.B.data,tNetPacket.B.count);
			tNetPacket.r_begin		(ID);
			R_ASSERT				(M_SPAWN == ID);
			
			string64				s_name;
			tNetPacket.r_string		(s_name);
			xrServerEntity			*E = F_entity_Create(s_name);

			R_ASSERT2				(E,"Can't create entity.");
			E->Spawn_Read			(tNetPacket);
			// Update
			tNetPacket.B.count		= S->Rword();
			S->Read					(tNetPacket.B.data,tNetPacket.B.count);
			tNetPacket.r_begin		(ID);
			R_ASSERT				(M_UPDATE == ID);
			E->UPDATE_Read			(tNetPacket);
			
			E->Init					(E->s_name);

			R_ASSERT				((E->s_gameid == GAME_SINGLE) || (E->s_gameid == GAME_ANY));
			R_ASSERT				((*I = dynamic_cast<CALifeDynamicObject*>(E)) != 0);
		}
	};
};