////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_brain.cpp
//	Created 	: 06.10.2005
//  Modified 	: 06.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human brain class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_human_brain.h"
#include "alife_human_object_handler.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "ef_storage.h"
#include "ef_primary.h"
#include "object_broker.h"

#ifdef DEBUG
#	include "level.h"
#	include "map_location.h"
#	include "map_manager.h"
#endif

#define MAX_ITEM_FOOD_COUNT			3
#define MAX_ITEM_MEDIKIT_COUNT		3
#define MAX_AMMO_ATTACH_COUNT		10

CALifeHumanBrain::CALifeHumanBrain	(object_type *object)
{
	VERIFY							(object);
	m_object						= object;
	m_object_handler				= xr_new<CALifeHumanObjectHandler>(object);
	m_failed						= true;

	m_dwTotalMoney					= 0;
	m_tpKnownCustomers.clear		();
	m_cpEquipmentPreferences.resize	(5);
	m_cpMainWeaponPreferences.resize(4);

#ifdef XRGAME_EXPORTS
	m_cpEquipmentPreferences.resize	(iFloor(ai().ef_storage().m_pfEquipmentType->ffGetMaxResultValue() + .5f));
	m_cpMainWeaponPreferences.resize(iFloor(ai().ef_storage().m_pfMainWeaponType->ffGetMaxResultValue() + .5f));
	R_ASSERT2						((iFloor(ai().ef_storage().m_pfEquipmentType->ffGetMaxResultValue() + .5f) == 5) && (iFloor(ai().ef_storage().m_pfMainWeaponType->ffGetMaxResultValue() + .5f) == 4),"Recompile Level Editor and xrAI and rebuild file \"game.spawn\"!");
#endif

	for (int i=0, n=m_cpEquipmentPreferences.size(); i<n; ++i)
		m_cpEquipmentPreferences[i]	= u8(::Random.randI(3));

	for (int i=0, n=m_cpMainWeaponPreferences.size(); i<n; ++i)
		m_cpMainWeaponPreferences[i]= u8(::Random.randI(3));

//	m_fSearchSpeed					= pSettings->r_float(object->name(), "search_speed");
//	m_fGoingSuccessProbability		= pSettings->r_float(object->name(), "going_item_detect_probability");
//	m_fSearchSuccessProbability		= pSettings->r_float(object->name(), "search_item_detect_probability");
}

CALifeHumanBrain::~CALifeHumanBrain	()
{
	xr_delete					(m_object_handler);
}

void CALifeHumanBrain::on_surge		()
{
}

void CALifeHumanBrain::on_state_write	(NET_Packet &packet)
{
	save_data					(m_tpKnownCustomers,packet);
	save_data					(m_cpEquipmentPreferences,packet);
	save_data					(m_cpMainWeaponPreferences,packet);
}

void CALifeHumanBrain::on_state_read	(NET_Packet &packet)
{
	if (object().m_wVersion <= 19)
		return;

	if (object().m_wVersion < 108) {
		{
			DWORD_VECTOR		temp;
			load_data			(temp,packet);
		}
		{
			xr_vector<bool>		temp;
			load_data			(temp,packet);
		}
	}

	if (object().m_wVersion <= 35)
		return;

	if (object().m_wVersion < 108) {
		shared_str				temp;
		packet.r_stringZ		(temp);
	}

	load_data					(m_tpKnownCustomers,packet);

	if (object().m_wVersion <= 69)
		return;

	load_data					(m_cpEquipmentPreferences,packet);
	load_data					(m_cpMainWeaponPreferences,packet);
}

void CALifeHumanBrain::on_register		()
{
}

void CALifeHumanBrain::on_unregister	()
{
}

bool CALifeHumanBrain::perform_attack	()
{
	return						(false);
}

ALife::EMeetActionType CALifeHumanBrain::action_type	(CSE_ALifeSchedulable *tpALifeSchedulable, const int &iGroupIndex, const bool &bMutualDetection)
{
	return						(ALife::eMeetActionTypeIgnore);
}

/**
			// FSM
			void					vfGoToCustomer			();
			void					vfAccomplishTask		();
			void					vfSearchObject			();
			// FSM miscellanious
			void					vfChooseHumanTask		();
			// miscellanious
			bool					bfCheckIfTaskCompleted	(ALife::_TASK_ID		task_id, ALife::OBJECT_IT &I);
			bool					bfCheckIfTaskCompleted	(ALife::_TASK_ID		task_id);
			bool					bfCheckIfTaskCompleted	(ALife::OBJECT_IT		&I);
			bool					bfCheckIfTaskCompleted	();
			bool					similar_task			(const CALifeTask		*prev_task, const CALifeTask *new_task);
			bool					bfChooseNextRoutePoint	();
			void					vfSetCurrentTask		(ALife::_TASK_ID		&tTaskID);
/**/

void CALifeHumanBrain::update		()
{
#ifdef DEBUG
	if (!Level().MapManager().HasMapLocation("debug_stalker",object().ID)) {
		CMapLocation				*map_location = 
			Level().MapManager().AddMapLocation(
				"debug_stalker",
				object().ID
			);

		map_location->SetHint		(object().name_replace());
	}
#endif

	/**
	VERIFY					(need_update(this));
	VERIFY					(check_inventory_consistency());
	if (!bfActive())
		return;

	R_ASSERT3				(!m_bOnline,"Can't update online object ",name_replace());

	if (m_flags.test(flUseSmartTerrains)) {
		if (!m_brain)
			m_brain			= xr_new<CALifeHumanBrain>(this);

		m_brain->update		();

		if (!m_brain->failed()) {
			m_tTimeID			= ai().alife().time_manager().game_time();
			return;
		}
	}

	m_dwTotalMoney			= u32(-1);
	GameGraph::_GRAPH_ID	start_game_vertex_id = m_tGraphID;
	bool					bOk;
	do {
		switch (m_tTaskState) {
			case eTaskStateChooseTask : {
				vfChooseTask();
				break;
			}
			case eTaskStateGoToCustomer : {
				vfGoToCustomer();
				break;
			}
			case eTaskStateGoToSOS : {
				vfGoToSOS();
				break;
			}
			case eTaskStateSendSOS : {
				vfSendSOS();
				break;
			}
			case eTaskStateAccomplishTask : {
				vfAccomplishTask();
				break;
			}
			case eTaskStateSearchItem : {
				vfSearchObject	();
				break;
			}
			default				: NODEFAULT;
		}
		bOk						= bfChooseNextRoutePoint();
		vfProcessItems			();
		if (start_game_vertex_id != m_tGraphID) {
			alife().check_for_interaction(this);
			start_game_vertex_id	= m_tGraphID;
		}
	}
	while (bOk && bfActive() && (ai().alife().graph().actor()->o_Position.distance_to(o_Position) > ai().alife().online_distance()));

	m_tTimeID					= ai().alife().time_manager().game_time();
	/**/
}

/**
void CALifeHumanBrain::vfChooseTask()
{
		if (!bfEnoughEquipmentToGo()) {
			if (bfDistanceToTraderIsDanger() || !bfEnoughMoneyToEquip()) {
				m_tTaskState = eTaskStateSendSOS;
				return;
			}
			else {
				m_tTaskState = eTaskStateGoToCustomer;
				return;
			}
		}
		else {
			m_tDestGraphPointIndex	= GameGraph::_GRAPH_ID(-1);
			vfChooseHumanTask		();
			CALifeTask			*l_tpTask = ai().alife().tasks().task(m_dwCurTaskID);
			switch (l_tpTask->m_tTaskType) {
				case eTaskTypeSearchForItemCG :
				case eTaskTypeSearchForItemOG : {
					m_tDestGraphPointIndex = l_tpTask->m_tGraphID;
					break;
				}
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemOL : {
#pragma todo("Dima to Dima : add graph point type item search")
					//m_baVisitedVertices.assign(tpALife->m_tpTerrain[0][l_tpTask->m_tLocationID].size(),false);
					//m_tDestGraphPointIndex = tpALife->m_tpTerrain[l_tpTask->m_tLocationID][m_dwCurTaskLocation = 0];
					//m_baVisitedVertices[m_dwCurTaskLocation] = true;
					break;
				}
				default				: NODEFAULT;
			};
			m_tTaskState			= eTaskStateAccomplishTask;
			m_tpPath.clear			();
		}
	}
}

void CALifeHumanBrain::vfGoToCustomer()
{
	// go to customer to get something to accomplish task
	if (m_tpPath.empty()) {
		ai().graph_engine().search	(ai().game_graph(),m_tGraphID,ai().alife().objects().object(ai().alife().tasks().task(m_dwCurTaskID)->m_tCustomerID)->m_tGraphID,&m_tpPath,GraphEngineSpace::CBaseParameters());
		m_dwCurNode					= 0;
		m_tNextGraphID				= m_tGraphID;
		m_fCurSpeed					= m_fGoingSpeed;
	}
	if ((m_dwCurNode >= (m_tpPath.size() - 1)) && (m_tGraphID == m_tNextGraphID)) {
		m_tpPath.clear();
		m_dwCurNode = 0;
		if (int(m_dwCurTaskID) > 0) {
			CALifeTask				*l_tpTask = ai().alife().tasks().task(m_dwCurTaskID);
			CSE_ALifeTrader			*l_tpTrader = smart_cast<CSE_ALifeTrader*>(ai().alife().objects().object(l_tpTask->m_tCustomerID));
			if (l_tpTrader)
				alife().communicate_with_customer(this,l_tpTrader);
		}
		m_tTaskState = eTaskStateChooseTask;
	}
}

void CALifeHumanBrain::vfAccomplishTask()
{
	// build path and wait until we go to the end of it
	m_fCurSpeed		= m_fGoingSpeed;
	m_detect_probability	= m_fGoingSuccessProbability; 
	if (m_tpPath.empty()) {
		ai().graph_engine().search(ai().game_graph(),m_tGraphID,m_tDestGraphPointIndex,&m_tpPath,GraphEngineSpace::CBaseParameters());
		m_dwCurNode				= 0;
		m_tNextGraphID			= m_tGraphID;
	}
	if ((m_dwCurNode + 1 >= (m_tpPath.size())) && (m_tGraphID == m_tNextGraphID)) {
		if (bfCheckIfTaskCompleted()) {
			m_tpPath.clear		();
			m_tTaskState		= eTaskStateGoToCustomer;
		}
		else {
			CALifeTask		*l_tpALifeTask = ai().alife().tasks().task(m_dwCurTaskID);
			switch (l_tpALifeTask->m_tTaskType) {
				case eTaskTypeSearchForItemCG :
				case eTaskTypeSearchForItemOG : {
					if ((m_dwCurNode + 1>= (m_tpPath.size())) && (m_tGraphID == m_tNextGraphID)) {
						++(l_tpALifeTask->m_dwTryCount);
						m_tTaskState = eTaskStateChooseTask;
					}
					break;
				}
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemOL : {
//					++(tpALife->task(m_dwCurTaskID)->m_dwTryCount);
//					m_tTaskState		= eTaskStateChooseTask;
//					for (++m_dwCurTaskLocation; (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) && (m_baVisitedVertices[m_dwCurTaskLocation]); ++m_dwCurTaskLocation);
//					if (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) {
//						m_baVisitedVertices[m_dwCurTaskLocation] = true;
//						tpAlife->ffFindMinimalPath(m_tGraphID,tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID][m_dwCurTaskLocation],m_tpPath);
//						m_dwCurNode = 0;
//					}
//					else
						m_tTaskState	= eTaskStateChooseTask;
					break;
				}
				default :				NODEFAULT;
			}
		}
	}
	else
		if (m_tNextGraphID == ai().alife().tasks().task(m_dwCurTaskID)->m_tGraphID)
			m_tTaskState	= eTaskStateSearchItem;
}

void CALifeHumanBrain::vfSearchObject()
{
	m_fCurSpeed					= m_fSearchSpeed;
	m_detect_probability		= m_fSearchSuccessProbability; 
	CALifeTask	*l_tpALifeTask = ai().alife().tasks().task(m_dwCurTaskID);
	switch (l_tpALifeTask->m_tTaskType) {
		case eTaskTypeSearchForItemCG :
		case eTaskTypeSearchForItemOG : {
			if ((m_tNextGraphID != l_tpALifeTask->m_tGraphID) || (m_tGraphID == ai().alife().tasks().task(m_dwCurTaskID)->m_tGraphID))
				m_tTaskState	= eTaskStateAccomplishTask;
			break;
		}
		case eTaskTypeSearchForItemCL :
		case eTaskTypeSearchForItemOL : {
			m_tTaskState		= eTaskStateChooseTask;
			break;
		}
		default :				NODEFAULT;
	}
}

bool CSE_ALifeHumanAbstract::bfCheckIfTaskCompleted(ALife::_TASK_ID task_id, OBJECT_IT &I)
{
	if (!ai().alife().tasks().task(task_id,true))
		return		(false);
	I				= children.begin();
	OBJECT_IT E = children.end();
	const CALifeTask	&tTask = *ai().alife().tasks().task(task_id);
	for ( ; I != E; ++I) {
		switch (tTask.m_tTaskType) {
			case eTaskTypeSearchForItemCL :
			case eTaskTypeSearchForItemCG : {
				if (!xr_strcmp(ai().alife().objects().object(*I)->s_name,tTask.m_caSection))
					return(true);
				break;
			}
			case eTaskTypeSearchForItemOL :
			case eTaskTypeSearchForItemOG : {
				if (ai().alife().objects().object(*I)->ID == tTask.m_tObjectID)
					return(true);
				break;
			}
		}
	}
	return			(false);
}

bool CSE_ALifeHumanAbstract::bfCheckIfTaskCompleted(OBJECT_IT &I)
{
	return					(bfCheckIfTaskCompleted(m_dwCurTaskID,I));
}

bool CSE_ALifeHumanAbstract::bfCheckIfTaskCompleted(ALife::_TASK_ID task_id)
{
	OBJECT_IT			I;
	return				(bfCheckIfTaskCompleted(task_id,I));
}

bool CSE_ALifeHumanAbstract::bfCheckIfTaskCompleted()
{
	OBJECT_IT			I;
	return				(bfCheckIfTaskCompleted(I));
}

void CSE_ALifeHumanAbstract::vfSetCurrentTask(_TASK_ID &tTaskID)
{
	m_dwCurTaskID				= ai().alife().tasks().task(tTaskID)->m_tTaskID;
}

bool CSE_ALifeHumanAbstract::bfChooseNextRoutePoint()
{
	if (!move_offline())
		return					(false);

	bool						bContinue = false;
	if (m_tNextGraphID != m_tGraphID) {
		_TIME_ID				tCurTime = ai().alife().time_manager().game_time();
		m_fDistanceFromPoint	+= float(tCurTime - m_tTimeID)/1000.f/ai().alife().time_manager().normal_time_factor()*m_fCurSpeed;
		if (m_fDistanceToPoint - m_fDistanceFromPoint < EPS_L) {
			bContinue			= true;
			if ((m_fDistanceFromPoint - m_fDistanceToPoint > EPS_L) && (m_fCurSpeed > EPS_L))
				m_tTimeID		= tCurTime - _TIME_ID(iFloor((m_fDistanceFromPoint - m_fDistanceToPoint)*1000.f/m_fCurSpeed));
			m_fDistanceToPoint	= m_fDistanceFromPoint	= 0.0f;
			m_tPrevGraphID		= m_tGraphID;
			alife().graph().change(this,m_tGraphID,m_tNextGraphID);
		}
	}
	else {
		if (++m_dwCurNode < m_tpPath.size()) {
			m_tNextGraphID		= GameGraph::_GRAPH_ID(m_tpPath[m_dwCurNode]);
			m_fDistanceToPoint	= ai().game_graph().distance(m_tGraphID,m_tNextGraphID);
			bContinue			= true;
		}
		else
			m_fCurSpeed	= 0.f;
	}
	return			(bContinue);
}

bool CSE_ALifeHumanAbstract::similar_task				(const CALifeTask *prev_task, const CALifeTask *new_task)
{
	if (!prev_task)
		return			(false);

	if (prev_task->m_tTaskType != new_task->m_tTaskType)
		return			(false);

	switch (prev_task->m_tTaskType) {
		case ALife::eTaskTypeSearchForItemOG :
		case ALife::eTaskTypeSearchForItemCG : {
			return		(prev_task->m_tGraphID == new_task->m_tGraphID);
											   }
		case ALife::eTaskTypeSearchForItemOL :
		case ALife::eTaskTypeSearchForItemCL : {
			return		(prev_task->m_tLocationID == new_task->m_tLocationID);
											   }
	}
	return				(false);
}

void CSE_ALifeHumanAbstract::vfChooseHumanTask()
{
	CALifeTask			*previous_task = ai().alife().tasks().task(m_dwCurTaskID,true);
	
	OBJECT_IT			I = m_tpKnownCustomers.begin();
	OBJECT_IT			E = m_tpKnownCustomers.end();
	for ( ; I != E; ++I) {
		OBJECT_TASK_MAP::const_iterator	J = ai().alife().tasks().cross().find(*I);
		R_ASSERT2		(ai().alife().tasks().cross().end() != J,"Can't find a specified customer in the Task registry!\nPossibly, there is no traders at all or there is no anomalous zones.");
		
		u32				l_dwMinTryCount = u32(-1);
		_TASK_ID		l_tBestTaskID = _TASK_ID(-1);
		TASK_SET::const_iterator	i = (*J).second.begin();
		TASK_SET::const_iterator	e = (*J).second.end();
		for ( ; i != e; ++i) {
			CALifeTask		*l_tpTask = ai().alife().tasks().task(*i);

			if (similar_task(previous_task,l_tpTask) && !bfCheckIfTaskCompleted(*i))
				continue;

			if (!l_tpTask->m_dwTryCount) {
				l_tBestTaskID = l_tpTask->m_tTaskID;
				break;
			}
			else
				if (l_tpTask->m_dwTryCount < l_dwMinTryCount) {
					l_dwMinTryCount = l_tpTask->m_dwTryCount;
					l_tBestTaskID = l_tpTask->m_tTaskID;
				}
		}
		
		if (_TASK_ID(-1) != l_tBestTaskID) {
			vfSetCurrentTask	(l_tBestTaskID);
			break;
		}
	}
}

bool CSE_ALifeHumanAbstract::bfPerformAttack()
{
	if (!m_tpCurrentBestWeapon)
		return(false);

	switch (m_tpCurrentBestWeapon->m_dwSlot) {
		case 0 :
			return						(true);
		case 3 : {
			bool						l_bOk = false;
			OBJECT_IT			I = children.begin();
			OBJECT_IT			E = children.end();
			for ( ; I != E; ++I)
				if (*I == m_tpCurrentBestWeapon->ID) {
					l_bOk				= true;
					CSE_ALifeItem		*l_tpALifeItem = smart_cast<CSE_ALifeItem*>(ai().alife().objects().object(*I));
					alife().release		(l_tpALifeItem,true);
					break;
				}
			R_ASSERT2					(l_bOk,"Cannot find specified weapon in the inventory");
			return						(false);
		}
		default : {
			R_ASSERT2					(m_tpCurrentBestWeapon->m_dwAmmoAvailable,"No ammo for the selected weapon!");
			if (!m_trader_flags.test(eTraderFlagInfiniteAmmo))
				--(m_tpCurrentBestWeapon->m_dwAmmoAvailable);
			if (m_tpCurrentBestWeapon->m_dwAmmoAvailable)
				return					(true);

			for (int i=0, n=children.size() ; i<n; ++i) {
				CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = smart_cast<CSE_ALifeItemAmmo*>(ai().alife().objects().object(children[i]));
				if (l_tpALifeItemAmmo && strstr(m_tpCurrentBestWeapon->m_caAmmoSections,*l_tpALifeItemAmmo->s_name) && l_tpALifeItemAmmo->a_elapsed) {
					alife().release		(l_tpALifeItemAmmo,true);
					--i;
					--n;
				}
			}
			m_tpCurrentBestWeapon		= 0;
			return						(false);
		}
	}
}

EMeetActionType	CSE_ALifeHumanAbstract::tfGetActionType(CSE_ALifeSchedulable *tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
	if (eCombatTypeMonsterMonster == ai().alife().combat_type()) {
		CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable);
		R_ASSERT2					(l_tpALifeMonsterAbstract,"Inconsistent meet action type");
		return						(eRelationTypeFriend == ai().alife().relation_type(this,smart_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable)) ? eMeetActionTypeInteract : ((bMutualDetection || (eCombatActionAttack == alife().choose_combat_action(iGroupIndex))) ? eMeetActionTypeAttack : eMeetActionTypeIgnore));
	}
	else
		return(eMeetActionTypeAttack);
}

void CSE_ALifeHumanAbstract::vfChooseGroup(CSE_ALifeGroupAbstract *tpALifeGroupAbstract)
{
	{
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		for ( ; I != E; ++I) {
			CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ai().alife().objects().object(*I));
			R_ASSERT2				(l_tpALifeHumanAbstract,"Invalid group member");
			l_tpALifeHumanAbstract->vfAttachItems(eTakeTypeMin);
		}
	}

	{
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		for ( ; I != E; ++I) {
			CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ai().alife().objects().object(*I));
			R_ASSERT2				(l_tpALifeHumanAbstract,"Invalid group member");
			l_tpALifeHumanAbstract->vfAttachItems(eTakeTypeRest);
		}
	}
}
/**/