////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_registries.h"

//#define DEBUG_LOG

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
	u32								m_dwSwitchDelay;
	xrServer						*m_tpServer;
	bool							m_bActorEnabled;
	string256						m_caSaveName;
	bool							m_bFirstUpdate;
	u32								m_dwMaxCombatIterationCount;
	
	// temporary buffer for purchased by the particular trader artefacts
	ITEM_COUNT_MAP					m_tpTraderItems;
	ORGANIZATION_ORDER_MAP			m_tpSoldArtefacts;
	
	// temporary buffers for combats
	MONSTER_P_VECTOR				m_tpaCombatGroups[2];
	CSE_ALifeMonsterAbstract		*m_tpaCombatMonsters[2];

	// common
			void					vfUpdateDynamicData			(bool						bReserveID = true);
			void					vfUpdateDynamicData			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfCreateNewTask				(CSE_ALifeTrader			*tpTrader);
			void					vfCreateObjectFromSpawnPoint(CSE_ALifeDynamicObject		*&tpALifDynamicObject, CSE_ALifeDynamicObject *j, _SPAWN_ID tSpawnID);
			void					vfSetupScheduledObjects		();
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
			void					vfRemoveOnlineObject		(CSE_ALifeDynamicObject		*tpALifeDynamicObject, bool bAddToScheduled = true);
			void					vfCreateOnlineObject		(CSE_ALifeDynamicObject		*tpALifeDynamicObject, bool bRemoveFromScheduled = true);
			void					vfSwitchObjectOnline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfSwitchObjectOffline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					ProcessOnlineOfflineSwitches(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfFurlObjectOffline			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
	// interaction routines
			void					vfCheckForTheInteraction	(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract,	_GRAPH_ID					tGraphID);
			void					vfFillCombatGroup			(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract,	MONSTER_P_VECTOR			&tpGroupVector);
			bool					bfCheckForCombat			(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract1,	CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract2, int &iCombatGroupIndex);
			ECombatAction			tfChooseCombatAction		(int						iCombatGroupIndex);
			void					vfPerformAttackAction		(int						iCombatGroupIndex);
			bool					bfCheckIfRetreated			(int						iCombatGroupIndex);
			void					vfPerformCommunication		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		CSE_ALifeHumanAbstract		*tpALifeHumanAbstract2);
			void					vfFinishCombat				(ECombatResult				tCombatResult);
			void					vfAssignDeathPosition		(CSE_ALifeCreatureAbstract	*tpALifeCreatureAbstract,	_GRAPH_ID tGraphID);
			void					vfInitAI_ALifeMembers		();
public:
	// members
	bool							m_bLoaded;
	LPSTR							*m_cppServerOptions;
	float							m_fOnlineDistance;
	u32								m_dwInventorySlotCount;
	WEAPON_P_VECTOR					m_tpWeaponVector;	
	
	// temporary buffers for combats
	ITEM_P_LIST						m_tpItemList;
	
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
			void					vfReleaseObject				(CSE_Abstract				*tpSE_Abstract,				bool						bForceDelete = true);
	// miscellanious
			void					vfCommunicateWithCustomer	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		CSE_ALifeTraderAbstract		*tpTraderAbstract);
			void					vfCheckForTheInteraction	(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
			void					vfAppendItemList			(OBJECT_VECTOR &tObjectVector, ITEM_P_LIST &tItemList);
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
			void					vfSetOnlineDistance			(float						fNewDistance);
			void					vfSetProcessTime			(int						iMicroSeconds);
			void					vfSetSwitchDelay			(int						iMilliSeconds);
			void					vfSetScheduleMin			(int						iMilliSeconds);
			void					vfSetScheduleMax			(int						iMilliSeconds);
#endif
};