////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_registries.h"

#pragma todo("Dima to Dima : Be attentive with this speed optimization - it doesn't suit to the OOP paradigm!")
#define FAST_OWNERSHIP

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

	// temporary buffers for trading
	ITEM_P_VECTOR					m_tpItems1;
	ITEM_P_VECTOR					m_tpItems2;
	OBJECT_VECTOR					m_tpBlockedItems1;
	OBJECT_VECTOR					m_tpBlockedItems2;

	enum {
		MAX_STACK_DEPTH				= u32(128),
		SUM_COUNT_THRESHOLD			= u32(30),
	};

	ITEM_P_VECTOR					m_tpTrader1;
	ITEM_P_VECTOR					m_tpTrader2;
	INT_VECTOR						m_tpSums1;
	INT_VECTOR						m_tpSums2;

	SSumStackCell					m_tpStack1[MAX_STACK_DEPTH];
	SSumStackCell					m_tpStack2[MAX_STACK_DEPTH];

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
			void					vfBalanceCreatures			();
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
			bool					bfCheckForInteraction		(CSE_ALifeSchedulable		*tpALifeSchedulable1,		CSE_ALifeSchedulable	*tpALifeSchedulable2,			int				&iCombatGroupIndex,			bool					&bMutualDetection);
			bool					bfCheckObjectDetection		(CSE_ALifeSchedulable		*tpALifeSchedulable1,		CSE_ALifeSchedulable	*tpALifeSchedulable2);

			void					vfPerformAttackAction		(int						iCombatGroupIndex);
			bool					bfCheckIfRetreated			(int						iCombatGroupIndex);
			
			void					vfFillCombatGroup			(CSE_ALifeSchedulable		*tpALifeSchedulable,		int						iGroupIndex);
			void					vfFinishCombat				(ECombatResult				tCombatResult);
	// trading routines
#ifdef DEBUG
			void					vfPrintItems				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ITEM_P_VECTOR			&tpItemVector);
			void					vfPrintItems				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
#endif
			u32						dwfComputeItemCost			(ITEM_P_VECTOR				&tpItemVector);
			void					vfRunFunctionByIndex		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		OBJECT_VECTOR			&tpBlockedItems,				ITEM_P_VECTOR	&tpItems,					int						i,						int						&j);
			void					vfAssignItemParents			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		int						iItemCount);
			void					vfAppendBlockedItems		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		OBJECT_VECTOR			&tpObjectVector1,				OBJECT_VECTOR	&tpObjectVector2,			int						l_iItemCount1);
			void					vfAttachOwnerItems			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ITEM_P_VECTOR			&tpItemVector,					ITEM_P_VECTOR	&tpOwnItems);
			void					vfRestoreItems				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ITEM_P_VECTOR			&tpItemVector);
#ifdef FAST_OWNERSHIP
			void					vfAttachGatheredItems		(CSE_ALifeTraderAbstract	*tpALifeTraderAbstract1,	CSE_ALifeTraderAbstract	*tpALifeTraderAbstract2,		OBJECT_VECTOR	&tpObjectVector);
#else
			void					vfAttachGatheredItems		(CSE_ALifeTraderAbstract	*tpALifeTraderAbstract1,	OBJECT_VECTOR			&tpObjectVector);
#endif
			int						ifComputeBalance			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ITEM_P_VECTOR			&tpItemVector);
			void					vfFillTraderVector			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		int						iItemCount,						ITEM_P_VECTOR	&tpItemVector);
			void					vfGenerateSums				(ITEM_P_VECTOR				&tpTrader,					INT_VECTOR				&tpSums);
			bool					bfGetItemIndexes			(ITEM_P_VECTOR				&tpTrader,					int						iSum1,							INT_VECTOR		&tpIndexes,					SSumStackCell			*tpStack,				int						iStartI,					int				iStackPointer);
			bool					bfCheckForInventoryCapacity	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		ITEM_P_VECTOR			&tpTrader1,						INT_VECTOR		&tpIndexes1,				CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2,	ITEM_P_VECTOR			&tpTrader2,					INT_VECTOR		&tpIndexes2);
			bool					bfCheckForInventoryCapacity	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		ITEM_P_VECTOR			&tpTrader1,						int				iSum1,						int						iMoney1,				CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2,		ITEM_P_VECTOR	&tpTrader2,		int			iSum2,		int iMoney2, int iBalance);
			bool					bfCheckForTrade				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		ITEM_P_VECTOR			&tpTrader1,						INT_VECTOR		&tpSums1,					int						iMoney1,				CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2,		ITEM_P_VECTOR	&tpTrader2,		INT_VECTOR	&tpSums2,	int iMoney2, int iBalance);
			bool					bfCheckIfCanNullTradersBalance(CSE_ALifeHumanAbstract	*tpALifeHumanAbstract1,		CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2,			int				iItemCount1,				int						iItemCount2,			int						iBalance);
			void					vfPerformTrading			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2);
			void					vfPerformCommunication		();
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
	ITEM_P_VECTOR					m_tpTempItemBuffer;
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
			void					vfReleaseObject				(CSE_Abstract				*tpSE_Abstract,				bool					bALifeRequest = true);
	// miscellanious
			void					vfCheckForInteraction		(CSE_ALifeSchedulable		*tpALifeSchedulable);
			void					vfAppendItemVector			(OBJECT_VECTOR				&tObjectVector,				ITEM_P_VECTOR			&tItemList);
			ECombatAction			tfChooseCombatAction		(int						iCombatGroupIndex);
			ERelationType			tfGetRelationType			(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract1,	CSE_ALifeMonsterAbstract*tpALifeMonsterAbstract2);
			CSE_Abstract			*tpfCreateGroupMember		(CSE_ALifeGroupAbstract		*tpALifeGroupAbstract,		CSE_ALifeDynamicObject	*j);
	// trading routines
			void					vfCommunicateWithCustomer	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		CSE_ALifeTrader			*tpALifeTrader);
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