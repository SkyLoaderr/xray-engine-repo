////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_a_star.h"
#include "ai_alife_space.h"
#include "ai_alife_objects.h"
using namespace ALife;

class CSE_ALifeObjectRegistry : public IPureALifeLSObject {
public:
	OBJECT_MAP						m_tObjectRegistry;			// ������ ������� ����

									CSE_ALifeObjectRegistry		();
	virtual							~CSE_ALifeObjectRegistry	();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual void					Load						(IReader &tFileStream);
			bool					bfCheckIfTaskCompleted		(CSE_Abstract &CSE_Abstract, CSE_ALifeHumanAbstract *tpALifeHumanAbstract, OBJECT_IT &I);
			bool					bfCheckIfTaskCompleted		(CSE_ALifeHumanAbstract *tpALifeHuman, OBJECT_IT &I);
			bool					bfCheckIfTaskCompleted		(CSE_ALifeHumanAbstract *tpALifeHuman);
};

class CSE_ALifeEventRegistry : public IPureALifeLSObject {
public:
	_EVENT_ID						m_tEventID;					// ������������� ����� �������
	EVENT_MAP						m_tEventRegistry;			// ������ ������� ����

									CSE_ALifeEventRegistry		();
	virtual							~CSE_ALifeEventRegistry		();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
	virtual	void					Add							(CSE_ALifeEvent	*tpEvent);
};

class CSE_ALifeTaskRegistry : public IPureALifeLSObject {
public:
	_TASK_ID						m_tTaskID;					// ������������� ����� �������
	TASK_MAP						m_tTaskRegistry;			// ������ ������� ����

									CSE_ALifeTaskRegistry		();
	virtual							~CSE_ALifeTaskRegistry		();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
	virtual	void					Add							(CSE_ALifeTask *tpTask);
};

class CSE_ALifeGraphRegistry : public CSE_ALifeAStar {
public:
	typedef CSE_ALifeAStar inherited;
	ALIFE_ENTITY_P_VECTOR_MAP		m_tLevelMap;
	CSE_ALifeDynamicObject			*m_tpActor;
	ALIFE_ENTITY_P_VECTOR			*m_tpCurrentLevel;
	GRAPH_POINT_VECTOR				m_tpGraphObjects;					// �� ����� ����� �������� ��� 
	GRAPH_VECTOR_SVECTOR			m_tpTerrain[LOCATION_TYPE_COUNT];	// ������ �������: �� �������������� 
    																	//	��������� �������� ������ ����� 
																		//  �����
									CSE_ALifeGraphRegistry		();
			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfRemoveObjectFromGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphID);
			void					vfAddObjectToGraphPoint		(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tNextGraphPointID);
			void					vfChangeObjectGraphPoint	(CSE_ALifeDynamicObject *tpALifeDynamicObject, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID);
			// events
			void					vfRemoveEventFromGraphPoint	(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphID);
			void					vfAddEventToGraphPoint		(CSE_ALifeEvent *tpEvent, _GRAPH_ID tNextGraphPointID);
			void					vfChangeEventGraphPoint		(CSE_ALifeEvent *tpEvent, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID);
			void					vfAttachItem				(CSE_Abstract &CSE_Abstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bAddChild = true);
			void					vfDetachItem				(CSE_Abstract &CSE_Abstract, CSE_ALifeItem *tpALifeItem, _GRAPH_ID tGraphID, bool bRemoveChild = true);
};

class CSE_ALifeTraderRegistry {
public:
	TRADER_P_VECTOR					m_tpTraders;			// ������ ���������

			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
};

class CSE_ALifeScheduleRegistry {
public:
	ALIFE_MONSTER_P_VECTOR			m_tpScheduledObjects;	// ������ ����������� ��������

			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
};

class CSE_ALifeSpawnRegistry : public CSE_ALifeSpawnHeader {
public:
	typedef CSE_ALifeSpawnHeader inherited;
	
	ALIFE_ENTITY_P_VECTOR			m_tpSpawnPoints;
	LEVEL_POINT_VECTOR				m_tpArtefactSpawnPositions;
	xr_vector<bool>					m_baAliveSpawnObjects;

									CSE_ALifeSpawnRegistry		();
	virtual							~CSE_ALifeSpawnRegistry		();
	virtual void					Init						();
	virtual void					Load						(IReader &tFileStream);
};

class CSE_ALifeAnomalyRegistry : public IPureALifeLSObject {
public:
	ANOMALY_P_VECTOR_VECTOR			m_tpAnomalies;

									CSE_ALifeAnomalyRegistry	();
	virtual							~CSE_ALifeAnomalyRegistry	();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
};

class CSE_ALifeOrganizationRegistry : public IPureALifeLSObject {
public:
	ORGANIZATION_P_MAP				m_tOrganizationRegistry;
	DISCOVERY_P_MAP					m_tDiscoveryRegistry;

									CSE_ALifeOrganizationRegistry();
	virtual							~CSE_ALifeOrganizationRegistry();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
};
