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
private:
	u64								m_qwMaxProcessTime;
	float							m_fOnlineDistance;
	u32								m_dwSwitchDelay;
	xrServer						*m_tpServer;
	bool							m_bActorEnabled;
	string256						m_caSaveName;
	bool							m_bFirstUpdate;
	
	// temporary buffer for purchased by the particular trader artefacts
	ITEM_COUNT_MAP					m_tpTraderItems;
	ORGANIZATION_ORDER_MAP			m_tpSoldArtefacts;

	// common
			void					vfUpdateDynamicData			(bool bReserveID = true);
			void					vfUpdateDynamicData			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfCreateNewTask				(CSE_ALifeTrader			*tpTrader);
			void					vfAssignGraphPosition		(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
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

	// human misc
			IC		bool					bfCheckIfTaskCompleted		(CSE_ALifeHumanAbstract		*tpALifeHuman,			OBJECT_IT &I)
			{
				return(bfCheckIfTaskCompleted(*tpALifeHuman,tpALifeHuman,I));
			}

			IC		bool					bfCheckIfTaskCompleted		(CSE_ALifeHumanAbstract		*tpALifeHuman)
			{
				OBJECT_IT I;
				return(bfCheckIfTaskCompleted(tpALifeHuman,I));
			}

			IC		void					vfSetCurrentTask			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract, _TASK_ID &tTaskID)
			{
				tpALifeHumanAbstract->m_dwCurTaskID = tpfGetTaskByID(tTaskID)->m_tTaskID;
			}
			
			bool					bfCheckIfTaskCompleted		(CSE_Abstract				&CSE_Abstract,			CSE_ALifeHumanAbstract *tpALifeHumanAbstract,	OBJECT_IT &I);
			void					vfChooseHumanTask			(CSE_ALifeHumanAbstract		*tpALifeHuman);
			CSE_ALifeTrader *		tpfGetNearestSuitableTrader	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfCommunicateWithCustomer	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract, CSE_ALifeTraderAbstract *tpTraderAbstract);
			bool					bfProcessItems				(CSE_Abstract				&CSE_Abstract,			_GRAPH_ID		tGraphID, float fMaxItemMass,	float fProbability);
			bool					bfCheckForItems				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfCheckForDeletedEvents		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			bool					bfChooseNextRoutePoint		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
	// human fsm
			void					vfUpdateHuman				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfChooseTask				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfHealthCare				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfBuySupplies				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfGoToCustomer				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfBringToCustomer			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfGoToSOS					(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfSendSOS					(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfAccomplishTask			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfSearchObject				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
	// monster misc
			void					vfUpdateMonster				(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
			void					vfChooseNextRoutePoint		(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
	// common misc
			void					vfProcessNPC				(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
			void					vfCheckForTheBattle			(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
	// switch online/offline routines
			void					vfValidatePosition			(CSE_ALifeDynamicObject		*I);
			void					vfRemoveOnlineObject		(CSE_ALifeDynamicObject		*tpALifeDynamicObject, bool bAddToScheduled = true);
			void					vfCreateOnlineObject		(CSE_ALifeDynamicObject		*tpALifeDynamicObject, bool bRemoveFromScheduled = true);
			void					vfSwitchObjectOnline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfSwitchObjectOffline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					ProcessOnlineOfflineSwitches(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfFurlObjectOffline			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
public:
	// members
	bool							m_bLoaded;
	LPSTR							*m_cppServerOptions;
	
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
			void					vfReleaseObject				(CSE_Abstract				*tpSE_Abstract, bool bForceDelete = true);
	
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