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
private:
	u64								m_qwMaxProcessTime;
	float							m_fOnlineDistance;
	u32								m_dwSwitchDelay;
	xrServer						*m_tpServer;
	bool							m_bActorEnabled;
	string256						m_caSaveName;

	
	// buffer for union operations
	TASK_VECTOR						m_tpBufferTaskIDs;

	// temporary buffer for purchesed by the particular trader artefacts
	ITEM_COUNT_MAP					m_tpTraderItems;
	ORGANIZATION_ORDER_MAP			m_tpSoldArtefacts;

	// comnmon
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
	// after surge
			CSE_ALifeTrader *		tpfGetNearestSuitableTrader	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfCommunicateWithTrader		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,	CSE_ALifeTrader *tpTrader);
			void					vfUpdateMonster				(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
			void					vfUpdateHuman				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			bool					bfProcessItems				(CSE_Abstract				&CSE_Abstract,			_GRAPH_ID		tGraphID, float fMaxItemMass, float fProbability);
			bool					bfCheckForItems				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfCheckForDeletedEvents		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
			void					vfCheckForTheBattle			(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
			void					vfChooseNextRoutePoint		(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
			void					vfProcessNPC				(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
			void					vfReleaseObject				(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfCreateObject				(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfSwitchObjectOnline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfSwitchObjectOffline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					ProcessOnlineOfflineSwitches(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfFurlObjectOffline			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
public:
	// members
	bool							m_bLoaded;
	LPSTR							*m_cppServerOptions;
	// methods
									CSE_ALifeSimulator			(xrServer					*tpServer);
	virtual							~CSE_ALifeSimulator			();
	virtual float					shedule_Scale				();
	virtual void					shedule_Update				(u32 dt);	
	virtual BOOL					shedule_Ready				();
	virtual LPCSTR					cName						();
	virtual void					Load						(LPCSTR						caSaveName);
			void					Save						(LPCSTR						caSaveName);
			void					Save						();
			void					Generate					();
			void					vfNewGame					(LPCSTR						caSaveName);
			void					vfRemoveObject				(CSE_Abstract				*tpSE_Abstract);
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
			void					vfListObjects				();
			void					vfListEvents				();
			void					vfListTasks					();
			void					vfListTerrain				();
			void					vfListSpawnPoints			();
			void					vfObjectInfo				(_OBJECT_ID					&tObjectID);
			void					vfEventInfo					(_EVENT_ID					&tEventID);
			void					vfTaskInfo					(_TASK_ID					&tTaskID);
			void					vfSpawnPointInfo			(_SPAWN_ID					&tSpawnID);
			void					vfGraphVertexInfo			(_GRAPH_ID					&tGraphID);
			void					vfSetOnlineDistance			(float						fNewDistance);
			void					vfSetProcessTime			(int						iMicroSeconds);
			void					vfSetSwitchDelay			(int						iMilliSeconds);
			void					vfSetScheduleMin			(int						iMilliSeconds);
			void					vfSetScheduleMax			(int						iMilliSeconds);
#endif
};