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
	public CSheduled,
	public CRandom 
{
private:
	u32								m_dwObjectsBeingProcessed;
	u32								m_dwObjectsBeingSwitched;
	u64								m_qwMaxProcessTime;
	float							m_fOnlineDistance;
	u32								m_dwSwitchDelay;
	
	// buffer for union operations
	TASK_VECTOR						m_tpBufferTaskIDs;
	xrServer						*m_tpServer;

	//temporary buffer
	OBJECT_VECTOR					m_tpChildren;

	// comnmon
	void							vfUpdateDynamicData			(CSE_ALifeDynamicObject *tpALifeDynamicObject);
	void							vfUpdateDynamicData			();
	void							vfCreateNewTask				(CSE_ALifeTrader *tpTrader);
	void							vfAssignGraphPosition		(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract);
	// surge
	void							vfGenerateAnomalousZones	();
	void							vfGenerateArtefacts			();
	void							vfSellArtefacts				(CSE_ALifeTrader &tTrader);
	void							vfUpdateArtefactOrders		(CSE_ALifeTrader &tTrader);
	void							vfGiveMilitariesBribe		(CSE_ALifeTrader &tTrader);
	void							vfBuySupplies				(CSE_ALifeTrader &tTrader);
	void							vfAssignPrices				(CSE_ALifeTrader &tTrader);
	void							vfBallanceCreatures			();
	void							vfUpdateCreatures			();
	// after surge
	CSE_ALifeTrader *					tpfGetNearestSuitableTrader	(CSE_ALifeHumanAbstract			*tpALifeHuman);
	void							vfCommunicateWithTrader		(CSE_ALifeHumanAbstract			*tpALifeHuman, CSE_ALifeTrader *tpTrader);
	void							vfUpdateMonster				(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfUpdateHuman				(CSE_ALifeHumanAbstract	*tpALifeHuman);
	bool							bfProcessItems				(CSE_Abstract	&tServerEntity, _GRAPH_ID tGraphID, float fMaxItemMass, float fProbability);
	bool							bfCheckForItems				(CSE_ALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForDeletedEvents		(CSE_ALifeHumanAbstract	*tpALifeHumanAbstract);
	void							vfCheckForTheBattle			(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfChooseNextRoutePoint		(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
	void							vfProcessNPC				(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
	//
public:
	// members
	bool							m_bLoaded;
	// methods
									CSE_ALifeSimulator					(xrServer *tpServer);
	virtual							~CSE_ALifeSimulator					();
	virtual float					shedule_Scale				();
	virtual BOOL					Ready						();
	virtual LPCSTR					cName						();
	virtual void					Update						(u32 dt);	
	virtual void					Load						(LPCSTR caSaveName = SAVE_NAME);
			void					Save						(LPCSTR caSaveName = SAVE_NAME);
			void					Generate					();
			void					vfCreateObject				(CSE_ALifeDynamicObject *tpALifeObject);
			void					vfSwitchObjectOnline		(CSE_ALifeDynamicObject *tpALifeObject);
			void					vfSwitchObjectOffline		(CSE_ALifeDynamicObject *tpALifeObject);
			void					ProcessOnlineOfflineSwitches(CSE_ALifeDynamicObject *I);
			void					vfReleaseObject				(CSE_ALifeDynamicObject *tpALifeObject);
			void					vfNewGame					();
			void					vfRemoveObject				(CSE_Abstract *tpServerEntity);
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
			void					vfSetOnlineDistance			(float	fNewDistance);
			void					vfSetProcessTime			(int	iMicroSeconds);
			void					vfSetSwitchDelay			(int	iMilliSeconds);
			void					vfSetScheduleMin			(int	iMilliSeconds);
			void					vfSetScheduleMax			(int	iMilliSeconds);
#endif
};