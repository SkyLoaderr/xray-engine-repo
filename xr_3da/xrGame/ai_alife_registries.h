////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_spawn.h"
#include "ai_alife_predicates.h"

using namespace ALife;

class CALifeObjectRegistry : public IPureALifeLSObject {
public:
	_OBJECT_ID						m_tObjectID;			// идентификатор карты событий
	OBJECT_MAP						m_tObjectRegistry;		// список событий игры

	CALifeObjectRegistry()
	{
		m_tObjectID					= 0;
		m_tObjectRegistry.clear		();
	};

	virtual							~CALifeObjectRegistry()
	{
		free_map					(m_tObjectRegistry);
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.open_chunk	(OBJECT_CHUNK_DATA);
		tMemoryStream.write			(&m_tObjectID,sizeof(m_tObjectID));
		tMemoryStream.Wdword		(m_tObjectRegistry.size());
		OBJECT_PAIR_IT it			= m_tObjectRegistry.begin();
		OBJECT_PAIR_IT E			= m_tObjectRegistry.end();
		for ( ; it != E; it++) {
			CALifeTrader *tpALifeTrader = dynamic_cast<CALifeTrader *>((*it).second);
			if (tpALifeTrader)
				tMemoryStream.Wbyte(ALIFE_TRADER_ID);
			else {
				CALifeHuman	*tpALifeHuman = dynamic_cast<CALifeHuman *>((*it).second);
				if (tpALifeHuman)
					tMemoryStream.Wbyte(ALIFE_HUMAN_ID);
				else {
					CALifeHumanGroup	*tpALifeHumanGroup = dynamic_cast<CALifeHumanGroup *>((*it).second);
					if (tpALifeHumanGroup)
						tMemoryStream.Wbyte(ALIFE_HUMAN_GROUP_ID);
					else {
						CALifeMonster	*tpALifeMonster = dynamic_cast<CALifeMonster *>((*it).second);
						if (tpALifeMonster)
							tMemoryStream.Wbyte(ALIFE_MONSTER_ID);
						else {
							CALifeMonsterGroup	*tpALifeHumanGroup = dynamic_cast<CALifeMonsterGroup *>((*it).second);
							if (tpALifeHumanGroup)
								tMemoryStream.Wbyte(ALIFE_MONSTER_GROUP_ID);
							else {
								CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>((*it).second);
								if (tpALifeItem)
									tMemoryStream.Wbyte(ALIFE_ITEM_ID);
								else {
									CALifeAnomalousZone *tpALifeAnomalousZone = dynamic_cast<CALifeAnomalousZone *>((*it).second);
									if (tpALifeAnomalousZone)
										tMemoryStream.Wbyte(ALIFE_ANOMALOUS_ZONE_ID);
									else {
										Msg("!Unsupported ALife monster type!");
										R_ASSERT(false);
									}
								}
							}

						}
					}
				}
			}
			(*it).second->Save(tMemoryStream);
		}
		tMemoryStream.close_chunk	();
	};
	
	void CALifeObjectRegistry::Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(OBJECT_CHUNK_DATA));
		tFileStream.Read(&m_tObjectID,sizeof(m_tObjectID));
		m_tObjectRegistry.clear();
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			CALifeDynamicObject *tpALifeDynamicObject = 0;
			switch (tFileStream.Rbyte()) {
				case ALIFE_ITEM_ID : {
					tpALifeDynamicObject = xr_new<CALifeItem> ();
					break;
				}
				case ALIFE_MONSTER_ID : {
					tpALifeDynamicObject = xr_new<CALifeMonster> ();
					break;
				}
				case ALIFE_MONSTER_GROUP_ID : {
					tpALifeDynamicObject = xr_new<CALifeMonsterGroup> ();
					break;
				}
				case ALIFE_HUMAN_ID : {
					tpALifeDynamicObject = xr_new<CALifeHuman> ();
					break;
				}
				case ALIFE_HUMAN_GROUP_ID : {
					tpALifeDynamicObject = xr_new<CALifeHumanGroup> ();
					break;
				}
				case ALIFE_TRADER_ID : {
					tpALifeDynamicObject = xr_new<CALifeTrader> ();
					break;
				}
				case ALIFE_ANOMALOUS_ZONE_ID : {
					tpALifeDynamicObject = xr_new<CALifeDynamicAnomalousZone> ();
					break;
				}
				default : NODEFAULT;
			};
			tpALifeDynamicObject->Load	(tFileStream);
			m_tObjectRegistry.insert	(make_pair(tpALifeDynamicObject->m_tObjectID,tpALifeDynamicObject));
		}
	};

	virtual	void					Add	(CALifeDynamicObject *tpALifeDynamicObject)
	{
		m_tObjectRegistry.insert				(make_pair(tpALifeDynamicObject->m_tObjectID = m_tObjectID++,tpALifeDynamicObject));
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
		save_map					(m_tEventRegistry,tMemoryStream);
		tMemoryStream.close_chunk	();
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(EVENT_CHUNK_DATA));
		tFileStream.Read(&m_tEventID,sizeof(m_tEventID));
		load_map					(m_tEventRegistry,tFileStream,tfChooseEventKeyPredicate);
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
		save_map					(m_tTaskRegistry,tMemoryStream);
		tMemoryStream.close_chunk	();
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(TASK_CHUNK_DATA));
		tFileStream.Read			(&m_tTaskID,sizeof(m_tTaskID));
		load_map					(m_tTaskRegistry,tFileStream,tfChooseTaskKeyPredicate);
	};
	
	virtual	void					Add	(CALifeTask	*tpTask)
	{
		m_tTaskRegistry.insert		(make_pair(tpTask->m_tTaskID = m_tTaskID++,tpTask));
	};
};

class CALifeGraphRegistry {
public:
	GRAPH_POINT_VECTOR				m_tpGraphObjects;		// по точке графа получить все 
															//  динамические объекты
	void							Init()
	{
		m_tpGraphObjects.resize		(Level().AI.GraphHeader().dwVertexCount);
		{
			GRAPH_POINT_IT				I = m_tpGraphObjects.begin();
			GRAPH_POINT_IT				E = m_tpGraphObjects.end();
			for ( ; I != E; I++) {
				(*I).tpObjects.clear();
				(*I).tpEvents.clear();
			}
		}
	};

	IC void vfRemoveObjectFromGraphPoint(CALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphID)
	{
		DYNAMIC_OBJECT_P_IT				I = m_tpGraphObjects[tGraphID].tpObjects.begin();
		DYNAMIC_OBJECT_P_IT				E = m_tpGraphObjects[tGraphID].tpObjects.end();
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
		tpALifeItem->m_bAttached = true;
		DYNAMIC_OBJECT_P_IT		I = m_tpGraphObjects[tGraphID].tpObjects.begin();
		DYNAMIC_OBJECT_P_IT		E = m_tpGraphObjects[tGraphID].tpObjects.end();
		for ( ; I != E; I++)
			if (*I == tpALifeItem) {
				m_tpGraphObjects[tGraphID].tpObjects.erase(I);
				break;
			}
		tHumanParams.m_fCumulativeItemMass += tpALifeItem->m_fMass;
	}

	IC void vfDetachItem(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID)
	{
		tpALifeItem->m_bAttached = true;
		m_tpGraphObjects[tGraphID].tpObjects.push_back(tpALifeItem);
		tHumanParams.m_fCumulativeItemMass -= tpALifeItem->m_fMass;
	}

	IC void							Update(CALifeDynamicObject *tpALifeDynamicObject)
	{
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem) {
			if (!tpALifeItem->m_bAttached)
				m_tpGraphObjects[tpALifeItem->m_tGraphID].tpObjects.push_back(tpALifeItem);
			return;
		}
		if (!dynamic_cast<CALifeTrader *>(tpALifeDynamicObject))
			m_tpGraphObjects[tpALifeDynamicObject->m_tGraphID].tpObjects.push_back(tpALifeDynamicObject);
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

	IC CALifeTrader *				tpfGetNearestSuitableTrader(CALifeHuman *tpALifeHuman)
	{
		float			fBestDistance = MAX_NODE_ESTIMATION_COST;
		CALifeTrader *	tpBestTrader = 0;
		TRADER_P_IT		I = m_tpTraders.begin();
		TRADER_P_IT		E = m_tpTraders.end();
		Fvector			&tGlobalPoint = Level().AI.m_tpaGraph[tpALifeHuman->m_tGraphID].tGlobalPoint;
		for ( ; I != E; I++) {
			if ((*I)->m_tRank != tpALifeHuman->m_tRank)
				break;
			float fCurDistance = Level().AI.m_tpaGraph[(*I)->m_tGraphID].tGlobalPoint.distance_to(tGlobalPoint);
			if (fCurDistance < fBestDistance) {
				fBestDistance = fCurDistance;
				tpBestTrader = *I;
			}
		}
		return(tpBestTrader);
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
		if (tpALifeMonsterAbstract)
			m_tpScheduledObjects.push_back	(tpALifeMonsterAbstract);
	};	
};

class CALifeSpawnRegistry : public CALifeSpawnHeader {
public:
	typedef CALifeSpawnHeader inherited;
	
	ALIFE_ENTITY_P_VECTOR			m_tpALifeEntitites;
	ALIFE_ENTITY_P_VECTOR_MAP		m_tLevelEntities;
	
	virtual							~CALifeSpawnRegistry()
	{
		free_vector					(m_tpALifeEntitites);
	};
	
	virtual void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_tpALifeEntitites.resize	(m_dwSpawnCount);
		ALIFE_ENTITY_P_IT			I = m_tpALifeEntitites.begin();
		ALIFE_ENTITY_P_IT			E = m_tpALifeEntitites.end();
		NET_Packet					P;
		CStream						*S = 0;
		u16							ID;
		for (int id=0; I != E; I++, id++) {
			R_ASSERT				(0!=(S = tFileStream.OpenChunk(id)));
			// Spawn
			P.B.count				= S->Rword();
			S->Read					(P.B.data,P.B.count);
			P.r_begin				(ID);
			R_ASSERT				(M_SPAWN == ID);
			
			string64				s_name;
			P.r_string				(s_name);
			xrServerEntity			*E = F_entity_Create(s_name);

			R_ASSERT2				(E,"Can't create entity.");
			E->Spawn_Read			(P);
			// Update
			P.B.count				= S->Rword();
			S->Read					(P.B.data,P.B.count);
			P.r_begin				(ID);
			R_ASSERT				(M_UPDATE == ID);
			E->UPDATE_Read			(P);
			
			R_ASSERT				(E->s_gameid == GAME_SINGLE);
			R_ASSERT				((*I = dynamic_cast<xrALifeEntity*>(E)) != 0);
		}
		m_tLevelEntities.clear		();
		I							= m_tpALifeEntitites.begin();
		for ( ; I != E; I++) {
			u8						tLevelID = Level().AI.m_tpaGraph[(*I)->m_tGraphID].tLevelID;
			ALIFE_ENTITY_P_VECTOR_PAIR_IT	K = m_tLevelEntities.find(tLevelID);
			if (K == m_tLevelEntities.end()) {
				ALIFE_ENTITY_P_VECTOR	*T = xr_new<ALIFE_ENTITY_P_VECTOR>();
				T->push_back		(*I);
				m_tLevelEntities.insert(make_pair(tLevelID,T));
			}
			else
				(*K).second->push_back(*I);
		}
	};
};