////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace ALife;

class CAI_ALife;

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
		OBJECT_PAIR_IT it			= m_tObjectRegistry.begin();
		OBJECT_PAIR_IT E			= m_tObjectRegistry.end();
		for ( ; it != E; it++)
			xr_delete((*it).second);
		m_tObjectRegistry.clear();
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
					tpALifeDynamicObject = new CALifeItem();
					break;
				}
				case ALIFE_MONSTER_ID : {
					tpALifeDynamicObject = new CALifeMonster();
					break;
				}
				case ALIFE_MONSTER_GROUP_ID : {
					tpALifeDynamicObject = new CALifeMonsterGroup();
					break;
				}
				case ALIFE_HUMAN_ID : {
					tpALifeDynamicObject = new CALifeHuman();
					break;
				}
				case ALIFE_HUMAN_GROUP_ID : {
					tpALifeDynamicObject = new CALifeHumanGroup();
					break;
				}
				case ALIFE_TRADER_ID : {
					tpALifeDynamicObject = new CALifeTrader();
					break;
				}
				case ALIFE_ANOMALOUS_ZONE_ID : {
					tpALifeDynamicObject = new CALifeDynamicAnomalousZone();
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
		EVENT_PAIR_IT it			= m_tEventRegistry.begin();
		EVENT_PAIR_IT E				= m_tEventRegistry.end();
		for ( ; it != E; it++) {
			xr_delete((*it).second->m_tpMonsterGroup1);
			xr_delete((*it).second->m_tpMonsterGroup2);
		}
		m_tEventRegistry.clear();
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.open_chunk	(EVENT_CHUNK_DATA);
		tMemoryStream.write			(&m_tEventID,sizeof(m_tEventID));
		tMemoryStream.Wdword		(m_tEventRegistry.size());
		EVENT_PAIR_IT it			= m_tEventRegistry.begin();
		EVENT_PAIR_IT E				= m_tEventRegistry.end();
		for ( ; it != E; it++) {
			CALifeEvent				*tpEvent = (*it).second;
			tMemoryStream.write		(&(tpEvent->m_tEventID),		sizeof(tpEvent->m_tEventID		));
			tMemoryStream.write		(&(tpEvent->m_tTimeID),			sizeof(tpEvent->m_tTimeID		));
			tMemoryStream.write		(&(tpEvent->m_tGraphID),		sizeof(tpEvent->m_tGraphID		));
			tMemoryStream.write		(&(tpEvent->m_tBattleResult),	sizeof(tpEvent->m_tBattleResult	));
			tpEvent->m_tpMonsterGroup1 = new CALifeEventGroup();
			tpEvent->m_tpMonsterGroup2 = new CALifeEventGroup();
			tpEvent->m_tpMonsterGroup1->Save(tMemoryStream);
			tpEvent->m_tpMonsterGroup2->Save(tMemoryStream);
		}
		tMemoryStream.close_chunk	();
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(EVENT_CHUNK_DATA));
		tFileStream.Read(&m_tEventID,sizeof(m_tEventID));
		m_tEventRegistry.clear();
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			CALifeEvent				*tpEvent = new CALifeEvent;
			tFileStream.Read		(&tpEvent->m_tEventID,		sizeof(tpEvent->m_tEventID		));
			tFileStream.Read		(&tpEvent->m_tTimeID,		sizeof(tpEvent->m_tTimeID		));
			tFileStream.Read		(&tpEvent->m_tGraphID,		sizeof(tpEvent->m_tGraphID		));
			tFileStream.Read		(&tpEvent->m_tBattleResult,	sizeof(tpEvent->m_tBattleResult	));
			tpEvent->m_tpMonsterGroup1	= new CALifeEventGroup();
			tpEvent->m_tpMonsterGroup2	= new CALifeEventGroup();
			tpEvent->m_tpMonsterGroup1->Load(tFileStream);
			tpEvent->m_tpMonsterGroup2->Load(tFileStream);
			m_tEventRegistry.insert	(make_pair(tpEvent->m_tEventID,tpEvent));
		}
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
		m_tTaskRegistry.clear				();
	};

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.open_chunk	(TASK_CHUNK_DATA);
		tMemoryStream.write			(&m_tTaskID,sizeof(m_tTaskID));
		tMemoryStream.Wdword		(m_tTaskRegistry.size());
		TASK_PAIR_IT it				= m_tTaskRegistry.begin();
		TASK_PAIR_IT E				= m_tTaskRegistry.end();
		for ( ; it != E; it++)
			(*it).second->Save(tMemoryStream);
		tMemoryStream.close_chunk	();
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(TASK_CHUNK_DATA));
		tFileStream.Read			(&m_tTaskID,sizeof(m_tTaskID));
		m_tTaskRegistry.clear		();
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			CALifeTask				*tpTask = new CALifeTask();
			tpTask->Load			(tFileStream);
			m_tTaskRegistry.insert	(make_pair(tpTask->m_tTaskID,tpTask));
		}
	};
	
	virtual	void					Add	(CALifeTask	*tpTask)
	{
		m_tTaskRegistry.insert		(make_pair(tpTask->m_tTaskID = m_tTaskID++,tpTask));
	};
};

class CALifeSpawnRegistry : public CALifeSpawnHeader {
public:
	typedef CALifeSpawnHeader inherited;
	
	SPAWN_P_VECTOR					m_tpSpawnPoints;
	
	virtual void					Load(CStream	&tFileStream)
	{
		inherited::Load(tFileStream);
		R_ASSERT(tFileStream.FindChunk(SPAWN_POINT_CHUNK_DATA));
		load_vector(m_tpSpawnPoints,tFileStream);
	};
};