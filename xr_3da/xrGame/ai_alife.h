////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_objects.h"
#include "ai_alife_registries.h"

class CAI_ALife : public CSheduled {
public:
	typedef CSheduled inherited;

	u32								m_dwObjectsBeingProcessed;
	u64								m_qwMaxProcessTime;
	bool							m_bLoaded;
	
	SSpawnHeader					m_tSpawnHeader;
	SALifeHeader					m_tALifeHeader;

	// auto
	svector<vector<_GRAPH_ID>,256>	m_tpTerrain;			// массив списков: по идетнификатору 
															//	местности получить список точек 
															//  графа
	vector<vector<_OBJECT_ID> >		m_tpLocationOwners;		// массив списков : по точке графа 
															//  получить список её владельцев
	vector<vector<_OBJECT_ID> >		m_tpGraphObjects;		// по точке графа получить все 
															//  динамические объекты
	vector<_OBJECT_ID>				m_tpScheduledObjects;	// массив обновляемых объектов
	
	// static
	vector<SSpawnPoint>				m_tpSpawnPoints;		// массив spawn-point-ов

	// dynamic
	CALifeObjectRegistry			m_tObjectRegistry;		// карта объектов
	CALifeEventRegistry				m_tEventRegistry;		// карта событий
	CALifeTaskRegistry				m_tTaskRegistry;		// карта заданий

	IC void vfRemoveFromGraphPoint(_OBJECT_ID tObjectID, _GRAPH_ID tGraphID)
	{
		for (int i=0; i<(int)m_tpGraphObjects[tGraphID].size(); i++)
			if (m_tpGraphObjects[tGraphID][i] == tObjectID) {
				m_tpGraphObjects[tGraphID].erase(m_tpGraphObjects[m_tObjectRegistry.m_tppMap[tObjectID]->m_tGraphID].begin() + i);
				break;
			}
	}
	
	IC void vfAddToGraphPoint(_OBJECT_ID tObjectID, _GRAPH_ID tNextGraphPointID)
	{
		m_tpGraphObjects[tNextGraphPointID].push_back(tObjectID);
	}

	IC void vfChangeGraphPoint(_OBJECT_ID tObjectID, _GRAPH_ID tGraphPointID, _GRAPH_ID tNextGraphPointID)
	{
		vfRemoveFromGraphPoint	(tObjectID,tGraphPointID);
		vfAddToGraphPoint		(tObjectID,tNextGraphPointID);
	}

	void							vfCheckForDeletedEvents	(_OBJECT_ID tObjectID);
	void							vfCheckForTheBattle		(_OBJECT_ID tObjectID);
	void							vfChooseNextRoutePoint	(_OBJECT_ID tObjectID);
	void							vfProcessNPC			(_OBJECT_ID tObjectID);
	void							vfInitTerrain			();
	void							vfInitLocationOwners	();
	void							vfInitGraph				();
	void							vfInitScheduledObjects	();
	// temporary
	void							vfGenerateSpawnPoints	(u32 dwSpawnCount);
	void							vfSaveSpawnPoints		();
	//
public:
									CAI_ALife				();
	virtual							~CAI_ALife				();
	virtual float					shedule_Scale			()					{return .5f;};
	virtual BOOL					Ready					()					{return TRUE;};
	virtual LPCSTR					cName					()					{return "ALife simulator";}; 
	virtual void					Load					();
	virtual void					Update					(u32 dt);	
			void					Save					();
			void					Generate				();
};