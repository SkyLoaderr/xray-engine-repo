////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_space.h"
#include "ai_alife_objects.h"
#include "ai_alife_registries.h"

#define ALIFE_SUPPORT_CONSOLE_COMMANDS

class CAI_ALife : public CSheduled {
public:
	typedef CSheduled inherited;

	u32								m_dwObjectsBeingProcessed;
	u64								m_qwMaxProcessTime;
	bool							m_bLoaded;
	
	SSpawnHeader					m_tSpawnHeader;
	SALifeHeader					m_tALifeHeader;

	// auto
	GRAPH_VECTOR_SVECTOR			m_tpTerrain;			// ������ �������: �� �������������� 
															//	��������� �������� ������ ����� 
															//  �����
	OBJECT_VECTOR_VECTOR			m_tpLocationOwners;		// ������ ������� : �� ����� ����� 
															//  �������� ������ � ����������
	GRAPH_POINT_VECTOR				m_tpGraphObjects;		// �� ����� ����� �������� ��� 
															//  ������������ �������
	ALIFE_MONSTER_P_VECTOR			m_tpScheduledObjects;	// ������ ����������� ��������
	
	// static
	SPAWN_VECTOR					m_tpSpawnPoints;		// ������ spawn-point-��

	// dynamic
	CALifeObjectRegistry			m_tObjectRegistry;		// ����� ��������
	CALifeEventRegistry				m_tEventRegistry;		// ����� �������
	CALifeTaskRegistry				m_tTaskRegistry;		// ����� �������

	// objects
	IC void vfRemoveObjectFromGraphPoint(_OBJECT_ID tObjectID, _GRAPH_ID tGraphID)
	{
		for (int i=0; i<(int)m_tpGraphObjects[tGraphID].tpObjectIDs.size(); i++)
			if (m_tpGraphObjects[tGraphID].tpObjectIDs[i] == tObjectID) {
				m_tpGraphObjects[tGraphID].tpObjectIDs.erase(m_tpGraphObjects[tGraphID].tpObjectIDs.begin() + i);
				break;
			}
	}
	
	IC void vfAddObjectToGraphPoint(_OBJECT_ID tObjectID, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].tpObjectIDs.push_back(tObjectID);
	}

	IC void vfChangeObjectGraphPoint(_OBJECT_ID tObjectID, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveObjectFromGraphPoint	(tObjectID,tGraphPointID);
		vfAddObjectToGraphPoint		(tObjectID,tNextGraphPointID);
	}

	// events
	IC void vfRemoveEventFromGraphPoint(_EVENT_ID tEventID, _GRAPH_ID tGraphID)
	{
		for (int i=0; i<(int)m_tpGraphObjects[tGraphID].tpEventIDs.size(); i++)
			if (m_tpGraphObjects[tGraphID].tpEventIDs[i] == tEventID) {
				m_tpGraphObjects[tGraphID].tpEventIDs.erase(m_tpGraphObjects[tGraphID].tpEventIDs.begin() + i);
				break;
			}
	}
	
	IC void vfAddEventToGraphPoint(_EVENT_ID tEventID, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].tpEventIDs.push_back(tEventID);
	}

	IC void vfChangeEventGraphPoint(_EVENT_ID tEventID, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveEventFromGraphPoint	(tEventID,tGraphPointID);
		vfAddEventToGraphPoint		(tEventID,tNextGraphPointID);
	}

	void							vfCheckForItems			(CALifeHuman	*tpALifeHuman);
	void							vfCheckForDeletedEvents	(CALifeHuman	*tpALifeHuman);
	void							vfCheckForTheBattle		(CALifeMonster	*tpALifeMonster);
	void							vfChooseNextRoutePoint	(CALifeMonster	*tpALifeMonster);
	void							vfProcessNPC			(CALifeMonster	*tpALifeMonster);
	void							vfInitTerrain			();
	void							vfInitLocationOwners	();
	void							vfInitGraph				();
	void							vfInitScheduledObjects	();
	void							vfLoadSpawnPoints		(CStream *tpStream);
	// temporary
	void							vfGenerateSpawnPoints	(u32 dwSpawnCount);
	void							vfSaveSpawnPoints		();
	//
public:
									CAI_ALife				();
	virtual							~CAI_ALife				();
	virtual float					shedule_Scale			()					{return .5f;};
	virtual BOOL					Ready					()					{return TRUE;};
	virtual LPCSTR					cName					()					{return "ALife Simulator";}; 
	virtual void					Load					();
	virtual void					Update					(u32 dt);	
			void					Save					();
			void					Generate				();
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
			void					vfListObjects			();
			void					vfListEvents			();
			void					vfListTasks				();
			void					vfListLocations			();
			void					vfListTerrain			();
			void					vfListSpawnPoints		();
			void					vfObjectInfo			(_OBJECT_ID	&tObjectID);
			void					vfEventInfo				(_EVENT_ID &tEventID);
			void					vfTaskInfo				(_TASK_ID &tTaskID);
			void					vfSpawnPointInfo		(_SPAWN_ID &tSpawnID);
#endif
};