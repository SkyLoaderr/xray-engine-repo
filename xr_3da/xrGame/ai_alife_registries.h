////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Objects_ALife_All.h"
#include "ai_alife_predicates.h"
#include "ai_alife_a_star.h"

using namespace ALife;

class CSE_ALifeObjectRegistry : public IPureALifeLSObject {
public:
	OBJECT_MAP						m_tObjectRegistry;		// список событий игры

									CSE_ALifeObjectRegistry()
	{
		m_tObjectRegistry.clear		();
	};

	virtual							~CSE_ALifeObjectRegistry()
	{
		free_map					(m_tObjectRegistry);
	};
	
	virtual	void					Save(IWriter &tMemoryStream)
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
	};
	
	virtual void					Load(IReader	&tFileStream)
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
	};

	IC bool bfCheckIfTaskCompleted(CSE_Abstract &CSE_Abstract, CSE_ALifeHumanAbstract *tpALifeHumanAbstract, OBJECT_IT &I)
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
			};
		}
		return(false);
	};

	IC bool bfCheckIfTaskCompleted(CSE_ALifeHumanAbstract *tpALifeHuman, OBJECT_IT &I)
	{
		return(bfCheckIfTaskCompleted(*tpALifeHuman,tpALifeHuman,I));
	};

	IC bool bfCheckIfTaskCompleted(CSE_ALifeHumanAbstract *tpALifeHuman)
	{
		OBJECT_IT I;
		return(bfCheckIfTaskCompleted(tpALifeHuman,I));
	};
};

class CSE_ALifeEventRegistry : public IPureALifeLSObject {
public:
	_EVENT_ID						m_tEventID;				// идентификатор карты событий
	EVENT_MAP						m_tEventRegistry;		// список событий игры

	CSE_ALifeEventRegistry()
	{
		m_tEventID					= 0;
		m_tEventRegistry.clear		();
	};

	virtual							~CSE_ALifeEventRegistry()
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
		R_ASSERT2					(tFileStream.find_chunk(EVENT_CHUNK_DATA),"Can't find chunk EVENT_CHUNK_DATA!");
		tFileStream.r				(&m_tEventID,sizeof(m_tEventID));
		//load_map					(m_tEventRegistry,tFileStream,tfChooseEventKeyPredicate);
	};
	
	virtual	void					Add	(CSE_ALifeEvent	*tpEvent)
	{
		m_tEventRegistry.insert		(std::make_pair(tpEvent->m_tEventID = m_tEventID++,tpEvent));
	};
};

class CSE_ALifeTaskRegistry : public IPureALifeLSObject {
public:
	_TASK_ID						m_tTaskID;				// идентификатор карты событий
	TASK_MAP						m_tTaskRegistry;		// список событий игры

	CSE_ALifeTaskRegistry()
	{
		m_tTaskID					= 0;
		m_tTaskRegistry.clear		();
	};

	virtual							~CSE_ALifeTaskRegistry()
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
		R_ASSERT2					(tFileStream.find_chunk(TASK_CHUNK_DATA),"Can't find chunk TASK_CHUNK_DATA");
		tFileStream.r				(&m_tTaskID,sizeof(m_tTaskID));
//		load_map					(m_tTaskRegistry,tFileStream,tfChooseTaskKeyPredicate);
	};
	 
	virtual	void					Add	(CSE_ALifeTask	*tpTask)
	{
		m_tTaskRegistry.insert		(std::make_pair(tpTask->m_tTaskID = m_tTaskID++,tpTask));
	};
};

class CSE_ALifeGraphRegistry : public CSE_ALifeAStar {
public:
	typedef CSE_ALifeAStar inherited;
	ALIFE_ENTITY_P_VECTOR_MAP		m_tLevelMap;
	CSE_ALifeDynamicObject				*m_tpActor;
	ALIFE_ENTITY_P_VECTOR			*m_tpCurrentLevel;
	GRAPH_POINT_VECTOR				m_tpGraphObjects;		// по точке графа получить все 
	GRAPH_VECTOR_SVECTOR			m_tpTerrain[LOCATION_TYPE_COUNT];			// массив списков: по идетнификатору 
    														//	местности получить список точек 
															//  графа
									CSE_ALifeGraphRegistry() : CSE_ALifeAStar()
	{
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

	IC void							Update(CSE_ALifeDynamicObject *tpALifeDynamicObject)
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

	IC void vfRemoveObjectFromGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphID)
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
	};
	
	IC void vfAddObjectToGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].tpObjects.push_back(tpALifeDynamicObject);
		tpALifeDynamicObject->m_tGraphID = tNextGraphPointID;
//.		Msg("ALife : adding object %s to graph point %d",tpALifeDynamicObject->s_name_replace,tNextGraphPointID);
	};

	IC void vfChangeObjectGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveObjectFromGraphPoint	(tpALifeDynamicObject,tGraphPointID);
		vfAddObjectToGraphPoint			(tpALifeDynamicObject,tNextGraphPointID);
		tpALifeDynamicObject->m_tGraphID	= tNextGraphPointID;
		tpALifeDynamicObject->o_Position	= getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tLocalPoint;
		tpALifeDynamicObject->m_tNodeID		= getAI().m_tpaGraph[tpALifeDynamicObject->m_tGraphID].tNodeID;
	};

	// events
	IC void vfRemoveEventFromGraphPoint(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphID)
	{
		EVENT_P_IT						I = m_tpGraphObjects[tGraphID].tpEvents.begin();
		EVENT_P_IT						E = m_tpGraphObjects[tGraphID].tpEvents.end();
		for ( ; I != E; I++)
			if ((*I) == tpEvent) {
				m_tpGraphObjects[tGraphID].tpEvents.erase(I);
				break;
			}
	};
	
	IC void vfAddEventToGraphPoint(CSE_ALifeEvent *tpEvent, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].tpEvents.push_back(tpEvent);
	};

	IC void vfChangeEventGraphPoint(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveEventFromGraphPoint	(tpEvent,tGraphPointID);
		vfAddEventToGraphPoint		(tpEvent,tNextGraphPointID);
	};

	IC void vfAttachItem(CSE_Abstract &CSE_Abstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bAddChild = true)
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

	IC void vfDetachItem(CSE_Abstract &CSE_Abstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bRemoveChild = true)
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
};

class CSE_ALifeTraderRegistry {
public:
	TRADER_P_VECTOR					m_tpTraders;			// массив торговцев

	void							Init()
	{
		m_tpTraders.clear			();
	};
	
	IC void							Update(CSE_ALifeDynamicObject *tpALifeDynamicObject)
	{
		CSE_ALifeTrader *tpALifeTrader = dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject);
		if (tpALifeTrader) {
			m_tpTraders.push_back(tpALifeTrader);
			std::sort(m_tpTraders.begin(),m_tpTraders.end(),CCompareTraderRanksPredicate());
		}
	};
};

class CSE_ALifeScheduleRegistry {
public:
	ALIFE_MONSTER_P_VECTOR			m_tpScheduledObjects;	// массив обновляемых объектов

	void							Init()
	{
		m_tpScheduledObjects.clear	();
	};

	IC void							Update(CSE_ALifeDynamicObject *tpALifeDynamicObject)
	{
		if (!tpALifeDynamicObject->m_bDirectControl)
			return;
		
		CSE_ALifeMonsterAbstract		*tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract *>(tpALifeDynamicObject);
		
		if (tpALifeMonsterAbstract && (tpALifeMonsterAbstract->fHealth > 0))
			m_tpScheduledObjects.push_back	(tpALifeMonsterAbstract);
	};	
};

class CSE_ALifeSpawnRegistry : public CSE_ALifeSpawnHeader {
public:
	typedef CSE_ALifeSpawnHeader inherited;
	
	ALIFE_ENTITY_P_VECTOR			m_tpSpawnPoints;
	LEVEL_POINT_VECTOR				m_tpArtefactSpawnPositions;

									CSE_ALifeSpawnRegistry()
	{
		m_tpSpawnPoints.clear		();
		m_tpArtefactSpawnPositions.clear();
	}
	
	virtual							~CSE_ALifeSpawnRegistry()
	{
		free_vector					(m_tpSpawnPoints);
	};
	
	virtual void					Init()
	{
		free_vector					(m_tpSpawnPoints);
	}

	virtual void					Load(IReader	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_tpSpawnPoints.resize		(m_dwSpawnCount);
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
			load_base_vector		(m_tpArtefactSpawnPositions,tFileStream);
		}
	};
};

class CSE_ALifeAnomalyRegistry : public IPureALifeLSObject {
public:
	ANOMALY_P_VECTOR				m_tpAnomalies;

									CSE_ALifeAnomalyRegistry()
	{
	};

	virtual							~CSE_ALifeAnomalyRegistry()
	{
	};
	
	virtual	void					Save(IWriter &tMemoryStream)
	{
		tMemoryStream.open_chunk	(ANOMALY_CHUNK_DATA);
		save_object_vector			(m_tpAnomalies,tMemoryStream);
		tMemoryStream.close_chunk	();
	};
	
	virtual	void					Load(IReader	&tFileStream)
	{ 
		R_ASSERT2					(tFileStream.find_chunk(OBJECT_CHUNK_DATA),"Can't find chunk OBJECT_CHUNK_DATA!");
		load_object_vector			(m_tpAnomalies,tFileStream);
	};
};
