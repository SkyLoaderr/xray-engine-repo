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
	map<_OBJECT_ID,CALifeItem *>	m_tppMap;				// ������ ������� ����
	map<_OBJECT_ID,CALifeItem *>::iterator	m_tpIterator;

	virtual							~CALifeObjectRegistry()
	{
		for (m_tpIterator = m_tppMap.begin(); m_tpIterator != m_tppMap.end(); m_tpIterator++)
			_DELETE((*m_tpIterator).second);
		m_tppMap.clear();
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.write(&m_tObjectID,sizeof(m_tObjectID));
		tMemoryStream.Wdword(m_tppMap.size());
		for (m_tpIterator = m_tppMap.begin(); m_tpIterator != m_tppMap.end(); m_tpIterator++) {
			CALifeHuman	*tpALifeHuman = dynamic_cast<CALifeHuman *>((*m_tpIterator).second);
			if (tpALifeHuman)
				tMemoryStream.Wbyte(2);
			else {
				CALifeMonster	*tpALifeMonster = dynamic_cast<CALifeMonster *>((*m_tpIterator).second);
				if (tpALifeMonster)
					tMemoryStream.Wbyte(1);
				else
					tMemoryStream.Wbyte(0);
			}
			(*m_tpIterator).second->Save(tMemoryStream);
		}
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		tFileStream.Read(&m_tObjectID,sizeof(m_tObjectID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			CALifeItem *tpALifeItem = 0;
			switch (tFileStream.Rbyte()) {
				case 0 : {
					tpALifeItem = new CALifeItem;
					break;
				}
				case 1 : {
					tpALifeItem = new CALifeMonster;
					break;
				}
				case 2 : {
					tpALifeItem = new CALifeHuman;
					break;
				}
				default : NODEFAULT;
			};
			tpALifeItem->Load(tFileStream);
			m_tppMap.insert			(make_pair(tpALifeItem->m_tObjectID,tpALifeItem));
		}
	};

	virtual	void					Add	(CALifeItem *tpALifeItem)
	{
		m_tppMap.insert				(make_pair(tpALifeItem->m_tObjectID = m_tObjectID++,tpALifeItem));
	};

};

class CALifeEventRegistry {
public:
	_EVENT_ID						m_tEventID;				// ������������� ����� �������
	map<_EVENT_ID,SEvent>			m_tpMap;				// ������ ������� ����
	map<_EVENT_ID,SEvent>::iterator	m_tpIterator;

	virtual							~CALifeEventRegistry()
	{
		for (m_tpIterator = m_tpMap.begin(); m_tpIterator != m_tpMap.end(); m_tpIterator++) {
			_DELETE((*m_tpIterator).second.tpMonsterGroup1);
			_DELETE((*m_tpIterator).second.tpMonsterGroup2);
		}
		m_tpMap.clear();
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.write			(&m_tEventID,sizeof(m_tEventID));
		tMemoryStream.Wdword		(m_tpMap.size());
		for (m_tpIterator = m_tpMap.begin(); m_tpIterator != m_tpMap.end(); m_tpIterator++) {
			SEvent					&tEvent = (*m_tpIterator).second;
			tMemoryStream.write		(&tEvent.tEventID,		sizeof(tEvent.tEventID		));
			tMemoryStream.write		(&tEvent.tTimeID,		sizeof(tEvent.tTimeID		));
			tMemoryStream.write		(&tEvent.tGraphID,		sizeof(tEvent.tGraphID		));
			tMemoryStream.write		(&tEvent.tBattleResult,	sizeof(tEvent.tBattleResult	));
			tMemoryStream.Wbyte		(tEvent.ucMonster1CountAfter);
			tMemoryStream.Wbyte		(tEvent.ucMonster2CountAfter);
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
			tEvent.ucMonster1CountAfter	= tFileStream.Rbyte();
			tEvent.ucMonster2CountAfter	= tFileStream.Rbyte();
			tEvent.tpMonsterGroup1	= new CALifeMonsterGroup;
			tEvent.tpMonsterGroup2	= new CALifeMonsterGroup;
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
	map<_TASK_ID,STask>				m_tpMap;				// ������ ������� ����
	map<_TASK_ID,STask>::iterator	m_tpIterator;

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.write			(&m_tTaskID,sizeof(m_tTaskID));
		tMemoryStream.Wdword		(m_tpMap.size());
		for (m_tpIterator = m_tpMap.begin(); m_tpIterator != m_tpMap.end(); m_tpIterator++) {
			STask					&tTask = (*m_tpIterator).second;
			tMemoryStream.write		(&tTask.tTaskID,		sizeof(tTask.tTaskID	));
			tMemoryStream.write		(&tTask.tTimeID,		sizeof(tTask.tTimeID	));
			tMemoryStream.write		(&tTask.tGraphID,		sizeof(tTask.tGraphID	));
			tMemoryStream.write		(&tTask.tCustomerID,	sizeof(tTask.tCustomerID));
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
			tFileStream.Read		(&tTask.tTaskType,		sizeof(tTask.tTaskType	));
			m_tpMap.insert			(make_pair(tTask.tTaskID,tTask));
		}
	};
	
	virtual	void					Add	(STask	&tTask)
	{
		m_tpMap.insert				(make_pair(tTask.tTaskID = m_tTaskID++,tTask));
	};
};