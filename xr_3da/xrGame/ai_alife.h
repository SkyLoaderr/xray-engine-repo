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

#define ALIFE_SUPPORT_CONSOLE_COMMANDS

class CAI_ALife : 
	public CALifeHeader, 
	public CALifeGameTime,
	public CALifeSpawnRegistry, 
	public CALifeObjectRegistry, 
	public CALifeEventRegistry, 
	public CALifeTaskRegistry, 
	public CALifeGraphRegistry,
	public CALifeTraderRegistry,
	public CALifeScheduleRegistry,
	public CSheduled,
	public CRandom 
{
private:
	u32								m_dwObjectsBeingProcessed;
	u32								m_dwObjectsBeingSwitched;
	u64								m_qwMaxProcessTime;
	float							m_fOnlineDistance;
	
	// buffer for union operations
	TASK_VECTOR						m_tpBufferTaskIDs;
	xrServer						*m_tpServer;
	CAI_DDD							*m_tpAI_DDD;
	// comnmon
	void							vfUpdateDynamicData			(CALifeDynamicObject *tpALifeDynamicObject);
	void							vfUpdateDynamicData			();
	void							vfCreateNewTask				(CALifeTrader *tpTrader);
	// surge
	void							vfGenerateAnomalousZones	();
	void							vfGenerateArtefacts			();
	void							vfSellArtefacts				(CALifeTrader &tTrader);
	void							vfUpdateArtefactOrders		(CALifeTrader &tTrader);
	void							vfGiveMilitariesBribe		(CALifeTrader &tTrader);
	void							vfBuySupplies				(CALifeTrader &tTrader);
	void							vfAssignPrices				(CALifeTrader &tTrader);
	void							vfBallanceCreatures			();
	void							vfUpdateCreatures			();
	// after surge
	CALifeTrader *					tpfGetNearestSuitableTrader	(CALifeHuman *tpALifeHuman);
	void							vfCommunicateWithTrader		(CALifeHuman *tpALifeHuman, CALifeTrader *tpTrader);
	void							vfUpdateMonster				(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfUpdateHumanGroup			(CALifeHumanGroup		*tpALifeHumanGroup);
	void							vfUpdateHuman				(CALifeHuman			*tpALifeHuman);
	bool							bfProcessItems				(CALifeHumanParams		&tHumanParams, _GRAPH_ID tGraphID, float fMaxItemMass, float fProbability);
	bool							bfCheckForItems				(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForDeletedEvents		(CALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForTheBattle			(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfChooseNextRoutePoint		(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfProcessNPC				(CALifeMonsterAbstract	*tpALifeMonsterAbstract);
	//
public:
	// members
	bool							m_bLoaded;
	// methods
									CAI_ALife					(xrServer *tpServer);
	virtual							~CAI_ALife					();
	virtual float					shedule_Scale				();
	virtual BOOL					Ready						();
	virtual LPCSTR					cName						();
	virtual void					Update						(u32 dt);	
	virtual void					Load						();
			void					Save						();
			void					Generate					();
			void					vfCreateObject				(CALifeDynamicObject *tpALifeObject);
			void					vfSwitchObjectOnline		(CALifeDynamicObject *tpALifeObject);
			void					vfSwitchObjectOffline		(CALifeDynamicObject *tpALifeObject);
			void					ProcessOnlineOfflineSwitches(CALifeDynamicObject *I);
			void					vfReleaseObject				(CALifeDynamicObject *tpALifeObject);
			void					vfNewGame					();
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
			void					vfListObjects				();
			void					vfListEvents				();
			void					vfListTasks					();
			void					vfListTerrain				();
			void					vfListSpawnPoints			();
			void					vfObjectInfo				(_OBJECT_ID	&tObjectID);
			void					vfEventInfo					(_EVENT_ID &tEventID);
			void					vfTaskInfo					(_TASK_ID &tTaskID);
			void					vfSpawnPointInfo			(_SPAWN_ID &tSpawnID);
			void					vfGraphVertexInfo			(_GRAPH_ID &tGraphID);
#endif
};