////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_registries.h"

class CSE_ALifeSimulator : 
	public CSE_ALifeHeader, 
	public CSE_ALifeGameTime,
	public CSE_ALifeSpawnRegistry, 
	public CSE_ALifeObjectRegistry, 
	public CSE_ALifeEventRegistry, 
	public CSE_ALifeTaskRegistry, 
	public CSE_ALifeGraphRegistry,
	public CSE_ALifeTraderRegistry,
	public CSE_ALifeScheduleRegistry,
	public CSE_ALifeAnomalyRegistry,
	public CSE_ALifeOrganizationRegistry,
	public ISheduled,
	public CRandom 
{
	u64								m_qwMaxProcessTime;
	xrServer						*m_tpServer;
	bool							m_bActorEnabled;
	string256						m_caSaveName;
	bool							m_bFirstUpdate;
	u32								m_dwMaxCombatIterationCount;
	u64								m_qwCycleCounter;
	
	// temporary buffer for purchased by the particular trader artefacts
	ITEM_COUNT_MAP					m_tpTraderItems;
	ORGANIZATION_ORDER_MAP			m_tpSoldArtefacts;
	
	// temporary buffers for combats
	SCHEDULE_P_VECTOR				m_tpaCombatGroups[2];
	CSE_ALifeSchedulable			*m_tpaCombatObjects[2];
	D_OBJECT_MAP					m_tpGraphPointObjects;

	// common
			void					vfUpdateDynamicData			(bool						bReserveID = true);
			void					vfUpdateDynamicData			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfCreateNewTask				(CSE_ALifeTrader			*tpTrader);
			void					vfCreateObjectFromSpawnPoint(CSE_ALifeDynamicObject		*&tpALifDynamicObject,		CSE_ALifeDynamicObject *tpALifeDynamicObject,			_SPAWN_ID tSpawnID);
			void					vfPrintTime					(LPCSTR						S,							_TIME_ID				tTimeID);
			void					vfAssignDeathPosition		(CSE_ALifeCreatureAbstract	*tpALifeCreatureAbstract,	_GRAPH_ID				tGraphID,						CSE_ALifeSchedulable *tpALifeSchedulable = 0);
			void					vfAssignArtefactPosition	(CSE_ALifeAnomalousZone		*tpALifeAnomalousZone,		CSE_ALifeDynamicObject	*tpALifeDynamicObject);
	// surge
			void					vfPerformSurge				();
			void					vfGenerateAnomalousZones	();
			void					vfGenerateAnomalyMap		();
			void					vfSellArtefacts				(CSE_ALifeTrader			&tTrader);
			void					vfUpdateArtefactOrders		(CSE_ALifeTrader			&tTrader);
			void					vfBuySupplies				(CSE_ALifeTrader			&tTrader);
			void					vfGiveMilitariesBribe		(CSE_ALifeTrader			&tTrader);
			void					vfUpdateOrganizations		();
			void					vfKillCreatures				();
			void					vfBallanceCreatures			();
			void					vfUpdateTasks				();
			void					vfAssignStalkerCustomers	();
	// switch online/offline routines
			void					vfValidatePosition			(CSE_ALifeDynamicObject		*I);
			void					vfRemoveOnlineObject		(CSE_ALifeDynamicObject		*tpALifeDynamicObject,		bool					bAddToRegistries = true);
			void					vfCreateOnlineObject		(CSE_ALifeDynamicObject		*tpALifeDynamicObject,		bool					bRemoveFromRegistries = true);
			void					vfSwitchObjectOnline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfSwitchObjectOffline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					ProcessOnlineOfflineSwitches(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfFurlObjectOffline			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfProcessAllTheSwitches		();
			void					vfProcessUpdates			();
	// interaction routines
			void					vfInitAI_ALifeMembers		();
			void					vfCheckForInteraction		(CSE_ALifeSchedulable		*tpALifeSchedulable,		_GRAPH_ID				tGraphID);
			bool					bfCheckForInteraction		(CSE_ALifeSchedulable		*tpALifeSchedulable1,		CSE_ALifeSchedulable	*tpALifeSchedulable2,			int &iCombatGroupIndex, bool &bMutualDetection);
			bool					bfCheckObjectDetection		(CSE_ALifeSchedulable		*tpALifeSchedulable1,		CSE_ALifeSchedulable	*tpALifeSchedulable2);

			void					vfPerformAttackAction		(int						iCombatGroupIndex);
			bool					bfCheckIfRetreated			(int						iCombatGroupIndex);
			void					vfPerformCommunication		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2);
			
			void					vfFillCombatGroup			(CSE_ALifeSchedulable		*tpALifeSchedulable,		int						iGroupIndex);
			void					vfFinishCombat				(ECombatResult				tCombatResult);
public:
	// members
	bool							m_bLoaded;
	LPSTR							*m_cppServerOptions;
	float							m_fSwitchDistance;
	float							m_fSwitchFactor;
	float							m_fOnlineDistance;
	float							m_fOfflineDistance;
	u32								m_dwInventorySlotCount;
	WEAPON_P_VECTOR					m_tpWeaponVector;	
	BOOL_VECTOR						m_baMarks;
	ECombatType						m_tCombatType;
	
	// temporary buffers for combats
	ITEM_P_VECTOR					m_tpItemVector;
	// constructors/destructors
									CSE_ALifeSimulator			(xrServer					*tpServer);
	virtual							~CSE_ALifeSimulator			();
	// scheduler
	virtual float					shedule_Scale				();
	virtual void					shedule_Update				(u32 dt);	
	virtual BOOL					shedule_Ready				();
	virtual LPCSTR					cName						();
	// game interface
	virtual void					Load						(LPCSTR						caSaveName);
			void					Save						(LPCSTR						caSaveName);
			void					Save						();
			void					vfNewGame					(LPCSTR						caSaveName);
			void					vfReleaseObject				(CSE_Abstract				*tpSE_Abstract,				bool						bALifeRequest = true);
	// miscellanious
			void					vfCommunicateWithCustomer	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		CSE_ALifeTraderAbstract		*tpTraderAbstract);
			void					vfCheckForInteraction		(CSE_ALifeSchedulable		*tpALifeSchedulable);
			void					vfAppendItemVector			(OBJECT_VECTOR &tObjectVector, ITEM_P_VECTOR &tItemList);
			ECombatAction			tfChooseCombatAction		(int						iCombatGroupIndex);
			ERelationType			tfGetRelationType			(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract1,	CSE_ALifeMonsterAbstract*tpALifeMonsterAbstract2);
	// console commands support
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
			void					vfListObjects				();
			void					vfListEvents				();
			void					vfListTasks					();
			void					vfListTerrain				();
			void					vfListSpawnPoints			();
			void					vfObjectInfo				(_OBJECT_ID					tObjectID);
			void					vfEventInfo					(_EVENT_ID					tEventID);
			void					vfTaskInfo					(_TASK_ID					tTaskID);
			void					vfSpawnPointInfo			(_SPAWN_ID					tSpawnID);
			void					vfGraphVertexInfo			(_GRAPH_ID					tGraphID);
			void					vfSetSwitchDistance			(float						fNewDistance);
			void					vfSetProcessTime			(int						iMicroSeconds);
			void					vfSetSwitchFactor			(float						fSwitchFactor);
			void					vfSetSwitchDelay			(int						iMilliSeconds);
			void					vfSetScheduleMin			(int						iMilliSeconds);
			void					vfSetScheduleMax			(int						iMilliSeconds);
#endif
};