////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace ALife;

class CALifeObjectRegistry {
public:
	_OBJECT_ID						m_tObjectID;			// ������������� ����� �������
	OBJECT_MAP						m_tppMap;				// ������ ������� ����

	CALifeObjectRegistry()
	{
		m_tObjectID					= 0;
	};

	virtual							~CALifeObjectRegistry()
	{
		OBJECT_PAIR_IT it			= m_tppMap.begin();
		OBJECT_PAIR_IT E			= m_tppMap.end();
		for ( ; it != E; it++)
			_DELETE((*it).second);
		m_tppMap.clear();
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.write			(&m_tObjectID,sizeof(m_tObjectID));
		tMemoryStream.Wdword		(m_tppMap.size());
		OBJECT_PAIR_IT it			= m_tppMap.begin();
		OBJECT_PAIR_IT E			= m_tppMap.end();
		for ( ; it != E; it++) {
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
						else
							tMemoryStream.Wbyte(ALIFE_ITEM_ID);
					}
				}
			}
			(*it).second->Save(tMemoryStream);
		}
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read(&m_tObjectID,sizeof(m_tObjectID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			CALifeDynamicObject *tpALifeDynamicObject = 0;
			switch (tFileStream.Rbyte()) {
				case ALIFE_ITEM_ID : {
					tpALifeDynamicObject = new CALifeItem;
					break;
				}
				case ALIFE_MONSTER_ID : {
					tpALifeDynamicObject = new CALifeMonster;
					break;
				}
				case ALIFE_MONSTER_GROUP_ID : {
					tpALifeDynamicObject = new CALifeMonsterGroup;
					break;
				}
				case ALIFE_HUMAN_ID : {
					tpALifeDynamicObject = new CALifeHuman;
					break;
				}
				case ALIFE_HUMAN_GROUP_ID : {
					tpALifeDynamicObject = new CALifeHumanGroup;
					break;
				}
				default : NODEFAULT;
			};
			tpALifeDynamicObject->Load(tFileStream);
			m_tppMap.insert			(make_pair(tpALifeDynamicObject->m_tObjectID,tpALifeDynamicObject));
		}
	};

	virtual	void					Add	(CALifeDynamicObject *tpALifeDynamicObject)
	{
		m_tppMap.insert				(make_pair(tpALifeDynamicObject->m_tObjectID = m_tObjectID++,tpALifeDynamicObject));
	};

};

class CALifeEventRegistry {
public:
	_EVENT_ID						m_tEventID;				// ������������� ����� �������
	EVENT_MAP						m_tpMap;				// ������ ������� ����

	CALifeEventRegistry()
	{
		m_tEventID					= 0;
	};

	virtual							~CALifeEventRegistry()
	{
		EVENT_PAIR_IT it			= m_tpMap.begin();
		EVENT_PAIR_IT E				= m_tpMap.end();
		for ( ; it != E; it++) {
			_DELETE((*it).second.tpMonsterGroup1);
			_DELETE((*it).second.tpMonsterGroup2);
		}
		m_tpMap.clear();
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.write			(&m_tEventID,sizeof(m_tEventID));
		tMemoryStream.Wdword		(m_tpMap.size());
		EVENT_PAIR_IT it			= m_tpMap.begin();
		EVENT_PAIR_IT E				= m_tpMap.end();
		for ( ; it != E; it++) {
			SEvent					&tEvent = (*it).second;
			tMemoryStream.write		(&tEvent.tEventID,		sizeof(tEvent.tEventID		));
			tMemoryStream.write		(&tEvent.tTimeID,		sizeof(tEvent.tTimeID		));
			tMemoryStream.write		(&tEvent.tGraphID,		sizeof(tEvent.tGraphID		));
			tMemoryStream.write		(&tEvent.tBattleResult,	sizeof(tEvent.tBattleResult	));
			tEvent.tpMonsterGroup1->Save(tMemoryStream);
			tEvent.tpMonsterGroup2->Save(tMemoryStream);
		}
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read(&m_tEventID,sizeof(m_tEventID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			SEvent					tEvent;
			tFileStream.Read		(&tEvent.tEventID,		sizeof(tEvent.tEventID		));
			tFileStream.Read		(&tEvent.tTimeID,		sizeof(tEvent.tTimeID		));
			tFileStream.Read		(&tEvent.tGraphID,		sizeof(tEvent.tGraphID		));
			tFileStream.Read		(&tEvent.tBattleResult,	sizeof(tEvent.tBattleResult	));
			tEvent.tpMonsterGroup1	= new CALifeEventGroup;
			tEvent.tpMonsterGroup2	= new CALifeEventGroup;
			tEvent.tpMonsterGroup1->Load(tFileStream);
			tEvent.tpMonsterGroup2->Load(tFileStream);
			m_tpMap.insert			(make_pair(tEvent.tEventID,tEvent));
		}
	};
	
	virtual	void					Add	(SEvent	&tEvent)
	{
		m_tpMap.insert				(make_pair(tEvent.tEventID = m_tEventID++,tEvent));
	};
};

class CALifeTaskRegistry {
public:
	_TASK_ID						m_tTaskID;				// ������������� ����� �������
	TASK_MAP						m_tpMap;				// ������ ������� ����

	CALifeTaskRegistry()
	{
		m_tTaskID					= 0;
	};

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.write			(&m_tTaskID,sizeof(m_tTaskID));
		tMemoryStream.Wdword		(m_tpMap.size());
		TASK_PAIR_IT it				= m_tpMap.begin();
		TASK_PAIR_IT E				= m_tpMap.end();
		for ( ; it != E; it++) {
			STask					&tTask = (*it).second;
			tMemoryStream.write		(&tTask.tTaskID,		sizeof(tTask.tTaskID	));
			tMemoryStream.write		(&tTask.tTimeID,		sizeof(tTask.tTimeID	));
			tMemoryStream.write		(&tTask.tGraphID,		sizeof(tTask.tGraphID	));
			tMemoryStream.write		(&tTask.tCustomerID,	sizeof(tTask.tCustomerID));
			tMemoryStream.write		(&tTask.tClassID,		sizeof(tTask.tClassID	));
			tMemoryStream.write		(&tTask.tTaskType,		sizeof(tTask.tTaskType	));
		}
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read(&m_tTaskID,sizeof(m_tTaskID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			STask					tTask;
			tFileStream.Read		(&tTask.tTaskID,		sizeof(tTask.tTaskID	));
			tFileStream.Read		(&tTask.tTimeID,		sizeof(tTask.tTimeID	));
			tFileStream.Read		(&tTask.tGraphID,		sizeof(tTask.tGraphID	));
			tFileStream.Read		(&tTask.tCustomerID,	sizeof(tTask.tCustomerID));
			tFileStream.Read		(&tTask.tClassID,		sizeof(tTask.tClassID	));
			tFileStream.Read		(&tTask.tTaskType,		sizeof(tTask.tTaskType	));
			m_tpMap.insert			(make_pair(tTask.tTaskID,tTask));
		}
	};
	
	virtual	void					Add	(STask	&tTask)
	{
		m_tpMap.insert				(make_pair(tTask.tTaskID = m_tTaskID++,tTask));
	};
};