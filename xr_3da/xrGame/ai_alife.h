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

	// buffer for union operations
	TASK_VECTOR						m_tpBufferTaskIDs;
	
	// static
	SPAWN_VECTOR					m_tpSpawnPoints;		// ������ spawn-point-��

	// auto
	GRAPH_VECTOR_SVECTOR			m_tpTerrain;			// ������ �������: �� �������������� 
															//	��������� �������� ������ ����� 
															//  �����
	OBJECT_VECTOR_VECTOR			m_tpLocationOwners;		// ������ ������� : �� ����� ����� 
															//  �������� ������ � ����������
	GRAPH_POINT_VECTOR				m_tpGraphObjects;		// �� ����� ����� �������� ��� 
															//  ������������ �������
	ALIFE_MONSTER_P_VECTOR			m_tpScheduledObjects;	// ������ ����������� ��������
	HUMAN_P_VECTOR					m_tpTraders;			// ������ ���������
	
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

	IC void vfCreateNewDynamicObject(SPAWN_IT I)
	{
		CALifeDynamicObject	*tpALifeDynamicObject;
		if (pSettings->LineExists((*I).caModel, "scheduled") && pSettings->ReadBOOL((*I).caModel, "scheduled"))
			if (pSettings->LineExists((*I).caModel, "human") && pSettings->ReadBOOL((*I).caModel, "human"))
				if (((*I).wCount > 1) && pSettings->LineExists((*I).caModel, "single") && pSettings->ReadBOOL((*I).caModel, "single"))
					tpALifeDynamicObject	= new CALifeHumanGroup;
				else
					tpALifeDynamicObject	= new CALifeHuman;
			else
				if (((*I).wCount > 1) && pSettings->LineExists((*I).caModel, "single") && pSettings->ReadBOOL((*I).caModel, "single"))
					tpALifeDynamicObject	= new CALifeMonsterGroup;
				else
					tpALifeDynamicObject	= new CALifeMonster;
		else
			tpALifeDynamicObject		= new CALifeItem;

		tpALifeDynamicObject->Init(_SPAWN_ID(I - m_tpSpawnPoints.begin()),m_tpSpawnPoints);
		m_tObjectRegistry.Add(tpALifeDynamicObject);
	};

	IC void vfCreateNewTask(CALifeHuman *tpTrader)
	{
		OBJECT_PAIR_IT	I = m_tObjectRegistry.m_tppMap.begin();
		OBJECT_PAIR_IT	E = m_tObjectRegistry.m_tppMap.end();
		for ( ; I != E; I++) {
			CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>((*I).second);
			if (tpALifeItem) {
				STask						tTask;
				tTask.tCustomerID			= tpTrader->m_tObjectID;
				tTask.tGraphID				= tpALifeItem->m_tGraphID;
				tTask.tTimeID				= Level().timeServer();
				tTask.tTaskType				= eTaskTypeSearchForArtefact;
				m_tTaskRegistry.Add			(tTask);
				tpTrader->m_tpTaskIDs.push_back(tTask.tTaskID);
				break;
			}
		}
	}

	void							vfCommunicateWithTrader	(CALifeHuman *tpALifeHuman, CALifeHuman *tpTrader);
	CALifeHuman *					tpfGetNearestSuitableTrader(CALifeHuman *tpALifeHuman);
	void							vfUpdateMonster			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfUpdateTrader			(CALifeHuman			*tpALifeHuman);
	void							vfUpdateHumanGroup		(CALifeHumanGroup		*tpALifeHumanGroup);
	void							vfUpdateHuman			(CALifeHuman			*tpALifeHuman);
	void							vfProcessItems			(CALifeHumanParams		&tHumanParams, _GRAPH_ID tGraphID, float fMaxItemMass);
	void							vfCheckForItems			(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForDeletedEvents	(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForTheBattle		(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfChooseNextRoutePoint	(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfProcessNPC			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfInitTerrain			();
	void							vfInitLocationOwners	();
	void							vfInitGraph				();
	void							vfInitScheduledObjects	();
	void							vfLoadSpawnPoints		(CStream *tpStream);
	// temporary
	void							vfGenerateSpawnPoints	(const u32 dwTotalCount, FLOAT_VECTOR &fpFactors);
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
  		
class CCompareTraderRanksPredicate {
	CAI_ALife	*m_tpALife;
public:
	CCompareTraderRanksPredicate	(CAI_ALife &tALife)
	{
		m_tpALife = &tALife;
	};

	bool operator()(const CALifeHuman *tpALifeHuman1, const CALifeHuman *tpALifeHuman2) const
	{
		return(tpALifeHuman1->m_tHumanParams.m_tRank < tpALifeHuman2->m_tHumanParams.m_tRank);
	};
};