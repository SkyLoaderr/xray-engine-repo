////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_surge_manager.cpp
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator surge manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_surge_manager.h"
#include "alife_trader_registry.h"
#include "alife_object_registry.h"
#include "alife_spawn_registry.h"
#include "alife_anomaly_registry.h"
#include "alife_known_anomaly.h"
#include "alife_organization.h"
#include "alife_organization_registry.h"
#include "alife_time_manager.h"
#include "alife_graph_registry.h"
#include "ef_storage.h"
#include "graph_engine.h"
#include "alife_task_registry.h"
#include "alife_schedule_registry.h"
#include "alife_simulator_header.h"
#include "xrserver.h"
#include "ai_space.h"

using namespace ALife;

CALifeSurgeManager::~CALifeSurgeManager	()
{
}

void CALifeSurgeManager::surge		()
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg								("[LSS] Surge is started...");
	}
#endif
	
	random().seed						(u32(CPU::GetCycleCount() & 0xffffffff));
	m_alive_spawn_objects.assign		(spawns().header().count(),false);
	
	// update all the objects
	D_OBJECT_P_MAP::const_iterator		I = objects().objects().begin();
	D_OBJECT_P_MAP::const_iterator		E = objects().objects().end();
	for ( ; I != E; ++I)
		(*I).second->on_surge			();
	
	generate_anomalies					();
	generate_anomaly_map				();
//	kill_creatures						();
	ballance_creatures					();
	VERIFY								(graph().actor());
	{
		CALifeTraderRegistry::TRADER_REGISTRY::const_iterator	I = traders().traders().begin();
		CALifeTraderRegistry::TRADER_REGISTRY::const_iterator	E = traders().traders().end();
		for ( ; I != E; ++I) {
			sell_artefacts				(**I);
			buy_supplies				(**I);
			give_military_bribe			(**I);
		}
	}
	organizations().update				();
	{
		CALifeTraderRegistry::TRADER_REGISTRY::const_iterator	I = traders().traders().begin();
		CALifeTraderRegistry::TRADER_REGISTRY::const_iterator	E = traders().traders().end();
		for ( ; I != E; ++I)
			organizations().update_artefact_orders(**I);
	}
	update_tasks						();
	assign_stalker_customers			();

	time_manager().last_surge_time		(time_manager().game_time());

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg								("[LSS] Surge is successfully completed");
	}
#endif
}

void CALifeSurgeManager::generate_anomalies()
{
	// deactivating all the anomalous zones
	D_OBJECT_P_MAP::const_iterator	B = objects().objects().begin(), I = B, J;
	D_OBJECT_P_MAP::const_iterator	E = objects().objects().end();
	for ( ; I != E; ++I) {
		CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = smart_cast<CSE_ALifeAnomalousZone*>((*I).second);
		if (l_tpALifeAnomalousZone)
			l_tpALifeAnomalousZone->m_maxPower = 0.f;
	}
	// for each spawn group activate a zone if any
	for (I = B; I != E; ) {
		CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = smart_cast<CSE_ALifeAnomalousZone*>((*I).second);
		if (!l_tpALifeAnomalousZone) {
			++I;
			continue;
		}

		// counting zones with the same group ID
		// !
		// spawn points are sorted according to their spawn group number in ascending order
		// this condition _must_ guarantee xrAI since it sorts spawn points during 'game.spawn' generation
		// we assume that zones in the spawn groups are generated in a row 
		// therefore they _must_ have their IDs in a row, since we starts their ID generation with a fullfilled
		// ID structure (look id_generator.h for details)
		// here we use this information by iterating on vector m_tpSpawnPoints and map objects().objects() 
		// (because this map is ordered by ID in ascending order)
		// if this condition is _not_ guaranteed we have to rewrite this piece of code
		// !
		float							fSum = 0;
		D_OBJECT_P_VECTOR::const_iterator	i = spawns().spawns().begin() + l_tpALifeAnomalousZone->m_tSpawnID, j = i, e = spawns().spawns().end(), b = spawns().spawns().begin();
		u32								l_dwGroupID = (*i)->m_dwSpawnGroup;
		for ( ; j != e; ++j)
			if ((*j)->m_dwSpawnGroup != l_dwGroupID)
				break;
			else
				fSum += (*j)->m_fProbability;
		R_ASSERT2				(fSum < 1 + EPS_L,"Group probability more than 1!");

		// computing probability of the anomalous zone activation
		float					fProbability = randF(1.f);
		fSum					= 0.f;
		J						= I;
		D_OBJECT_P_VECTOR::const_iterator	m = j;
		for ( j = i; (j != e) && ((*j)->m_dwSpawnGroup == l_dwGroupID); ++j, ++I) {
			fSum += (*j)->m_fProbability;
			if (fSum > fProbability)
				break;
		}

		// if random choosed a number due to which there is no active zones in the current group ID
		// then continue loop
		if (fSum <= fProbability)
			continue;

		// otherwise assign random anomaly power to the zone
		l_tpALifeAnomalousZone	= smart_cast<CSE_ALifeAnomalousZone*>((*I).second);
		R_ASSERT2				(l_tpALifeAnomalousZone,"Anomalous zones are grouped with incompatible objects!");
		CSE_ALifeAnomalousZone	*l_tpSpawnAnomalousZone = smart_cast<CSE_ALifeAnomalousZone*>(*j);
		R_ASSERT2				(l_tpSpawnAnomalousZone,"Anomalous zones are grouped with incompatible objects!");

#pragma todo("Dima to Dima : Correct anomalous zones power")
		l_tpALifeAnomalousZone->m_maxPower = l_tpALifeAnomalousZone->m_fStartPower = randF(l_tpALifeAnomalousZone->m_min_start_power,l_tpALifeAnomalousZone->m_max_start_power);
		u32						jj = iFloor(l_tpALifeAnomalousZone->m_maxPower/l_tpALifeAnomalousZone->m_power_artefact_factor);

		// proceed random artefacts generation for the active zone
		for (u32 ii=0; ii<jj; ++ii) {
			fProbability		= randF(1.f);
			fSum				= 0;
			for (u16 p=0; p<l_tpSpawnAnomalousZone->m_wItemCount; ++p) {
				fSum			+= l_tpSpawnAnomalousZone->m_faWeights[p];
				if (fSum > fProbability)
					break;
			}
			if (p < l_tpSpawnAnomalousZone->m_wItemCount) {
				CSE_Abstract	*l_tpSE_Abstract = F_entity_Create(l_tpSpawnAnomalousZone->m_cppArtefactSections[p]);
				R_ASSERT3		(l_tpSE_Abstract,"Can't spawn artefact ",l_tpSpawnAnomalousZone->m_cppArtefactSections[p]);
				CSE_ALifeDynamicObject	*i = smart_cast<CSE_ALifeDynamicObject*>(l_tpSE_Abstract);
				R_ASSERT2		(i,"Non-ALife object in the 'game.spawn'");

				i->ID			= server().PerformIDgen(0xffff);
				i->m_tSpawnID	= _SPAWN_ID(j - b);
				spawns().assign_artefact_position(l_tpSpawnAnomalousZone,i);
				i->m_bALifeControl = true;

				CSE_ALifeItemArtefact *l_tpALifeItemArtefact = smart_cast<CSE_ALifeItemArtefact*>(i);
				R_ASSERT2		(l_tpALifeItemArtefact,"Anomalous zone can't generate non-artefact objects since they don't have an 'anomaly property'!");

				l_tpALifeItemArtefact->m_fAnomalyValue = l_tpALifeAnomalousZone->m_maxPower*(1.f - i->o_Position.distance_to(l_tpSpawnAnomalousZone->o_Position)/l_tpSpawnAnomalousZone->m_fRadius);

				strcpy					(l_tpALifeItemArtefact->s_name_replace,l_tpALifeItemArtefact->s_name);
				if (l_tpALifeItemArtefact->ID < 1000)
					strcat				(l_tpALifeItemArtefact->s_name_replace,"0");
				if (l_tpALifeItemArtefact->ID < 100)
					strcat				(l_tpALifeItemArtefact->s_name_replace,"0");
				if (l_tpALifeItemArtefact->ID < 10)
					strcat				(l_tpALifeItemArtefact->s_name_replace,"0");
				string16				S1;
				strcat					(l_tpALifeItemArtefact->s_name_replace,itoa(l_tpALifeItemArtefact->ID,S1,10));
				register_object			(i,true);
			}
		}
		++I;
	}
}

void CALifeSurgeManager::generate_anomaly_map	()
{
	anomalies().clear						();
	D_OBJECT_P_MAP::const_iterator			I = objects().objects().begin();
	D_OBJECT_P_MAP::const_iterator			E = objects().objects().end();
	for ( ; I != E; ++I) {
		CSE_ALifeAnomalousZone				*anomaly = smart_cast<CSE_ALifeAnomalousZone*>((*I).second);
		if (!anomaly || !randI(20))
			continue;

		CALifeKnownAnomaly					*known_anomaly	= xr_new<CALifeKnownAnomaly>();
		known_anomaly->set_type				(randI(10) ? anomaly->m_tAnomalyType : EAnomalousZoneType(randI(eAnomalousZoneTypeDummy)));
		known_anomaly->set_power			(randF(anomaly->m_maxPower*.5f,anomaly->m_maxPower*1.5f));
		known_anomaly->set_distance			(randF(anomaly->m_fDistance*.5f,anomaly->m_fDistance*1.5f));
		known_anomaly->set_game_vertex_id	(anomaly->m_tGraphID);
		anomalies().add						(known_anomaly);
	}
}

void CALifeSurgeManager::ballance_creatures()
{
//	Msg									("BALLANCING OBJECTS");
#pragma todo("Dima to Dima : Respawn the objects in the spawn groups only")
	// filling array of the survived creatures
	{
		D_OBJECT_P_MAP::const_iterator	I = objects().objects().begin();
		D_OBJECT_P_MAP::const_iterator	E = objects().objects().end();
		for ( ; I != E; ++I) {
//			Msg							("object %s, spawn group %d",(*I).second->s_name_replace,(*I).second->m_tSpawnID);
			CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = smart_cast<CSE_ALifeCreatureAbstract*>((*I).second);
			CSE_ALifeGroupAbstract		*l_tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>((*I).second);
			if (l_tpALifeCreatureAbstract) {
				if (l_tpALifeGroupAbstract) {
					if (l_tpALifeGroupAbstract->m_wCount) {
						m_alive_spawn_objects[(*I).second->m_tSpawnID] = true;
#pragma todo("Dima to Dima : Add monster population increase here")
						//l_tpALifeGroupAbstract->m_wCount *= l_tpALifeGroupAbstract->m_wCount < 50 ? 1.5 : 0.8;
					}
				}
				else {
					if (l_tpALifeCreatureAbstract->fHealth > 0.f)
						m_alive_spawn_objects[(*I).second->m_tSpawnID] = true;
					else {
						CSE_ALifeCreatureAbstract	*creature = smart_cast<CSE_ALifeCreatureAbstract*>(spawns().spawns()[(*I).second->m_tSpawnID]);
						VERIFY						(creature);
						if (creature->g_Health() <= 0.f)
							m_alive_spawn_objects[(*I).second->m_tSpawnID] = true;
					}
				}
			}
			else
				m_alive_spawn_objects[(*I).second->m_tSpawnID] = true;
		}
	}
	// balancing creatures by spawn groups
	// i.e. if there is no object being spawned by the particular spawn group
	// then we have to spawn an object from this spawn group
	{
		D_OBJECT_P_VECTOR::const_iterator	B = spawns().spawns().begin(), I = B, J;
		D_OBJECT_P_VECTOR::const_iterator	E = spawns().spawns().end();
		for ( ; I != E; ) {
			u32						l_dwSpawnGroup = (*I)->m_dwSpawnGroup;
			bool					bOk = false;
			J						= I;
			for ( ; (I != E) && (l_dwSpawnGroup == (*I)->m_dwSpawnGroup); ++I)
				if (m_alive_spawn_objects[I - B]) {
					bOk = true;
					++I;
					break;
				}
			if (!bOk) {
				// there is no object being spawned from this spawn group -> spawn it!
				float				l_fProbability = randF(0,1.f), l_fSum = 0.f;
				D_OBJECT_P_VECTOR::const_iterator	j = J;
				D_OBJECT_P_VECTOR::const_iterator	e = I;
				for ( ; (j != e); ++j) {
					l_fSum			+= (*j)->m_fProbability;
					if (l_fSum > l_fProbability)
						break;
				}
				if (l_fSum > l_fProbability) {
					CSE_ALifeAnomalousZone		*l_tpALifeAnomalousZone		= smart_cast<CSE_ALifeAnomalousZone*>(*j);
					if (l_tpALifeAnomalousZone)
						continue;

#pragma todo("Dima to Dima : Decide where to spawn an actor!")
//					CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract	= smart_cast<CSE_ALifeCreatureAbstract*>(*j);
//					if (l_tpALifeCreatureAbstract)
//						continue;

					CSE_ALifeDynamicObject		*l_tpALifeDynamicObject;
					create(l_tpALifeDynamicObject,*j,_SPAWN_ID(j - B));
				}
			}
		}
	}

	// initialize array
	m_alive_spawn_objects.assign(m_alive_spawn_objects.size(),false);
}

void CALifeSurgeManager::kill_creatures()
{
	D_OBJECT_P_MAP::const_iterator	I = objects().objects().begin();
	D_OBJECT_P_MAP::const_iterator	E = objects().objects().end();
	for ( ; I != E; ++I) {
		CSE_ALifeCreatureAbstract *l_tpALifeCreatureAbstract = smart_cast<CSE_ALifeCreatureAbstract*>((*I).second);
		if (l_tpALifeCreatureAbstract && (l_tpALifeCreatureAbstract->m_bDirectControl) && (l_tpALifeCreatureAbstract->fHealth > 0.f)) {
			CSE_ALifeGroupAbstract *l_tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>((*I).second);
			ai().ef_storage().m_tpCurrentALifeObject = (*I).second;
			if (l_tpALifeGroupAbstract) {
				_GRAPH_ID			l_tGraphID = l_tpALifeCreatureAbstract->m_tGraphID;
				for (u32 i=0, N = (u32)l_tpALifeGroupAbstract->m_tpMembers.size(); i<N; ++i) {
					CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = smart_cast<CSE_ALifeCreatureAbstract*>(objects().object(l_tpALifeGroupAbstract->m_tpMembers[i]));
					R_ASSERT2					(l_tpALifeCreatureAbstract,"Group class differs from the member class!");
					ai().ef_storage().m_tpCurrentALifeObject = l_tpALifeCreatureAbstract;
					if (randF(100) > ai().ef_storage().m_pfSurgeDeathProbability->ffGetValue()) {
						l_tpALifeCreatureAbstract->m_bDirectControl	= true;
						l_tpALifeGroupAbstract->m_tpMembers.erase	(l_tpALifeGroupAbstract->m_tpMembers.begin() + i);
						register_object			(l_tpALifeCreatureAbstract);
						assign_death_position	(l_tpALifeCreatureAbstract,l_tGraphID);
						--i;
						--N;
					}
				}
			}
			else
				if (randI(100) < ai().ef_storage().m_pfSurgeDeathProbability->ffGetValue())
					l_tpALifeCreatureAbstract->fHealth = 0.f;
		}
	}
}

void CALifeSurgeManager::give_military_bribe(CSE_ALifeTrader &tTrader)
{
	tTrader.m_dwMoney = u32(float(tTrader.m_dwMoney)*.9f);
}

void CALifeSurgeManager::sell_artefacts(CSE_ALifeTrader &tTrader)
{
	// filling temporary map with the purchased artefacts
	m_tpTraderItems.clear();
	{
		xr_vector<u16>::iterator	i = tTrader.children.begin();
		xr_vector<u16>::iterator	e = tTrader.children.end();
		for ( ; i != e; ++i) {
			// checking if the purchased item is an artefact
			CSE_ALifeItemArtefact *l_tpALifeItemArtefact = smart_cast<CSE_ALifeItemArtefact*>(objects().object(*i));
			if (!l_tpALifeItemArtefact)
				continue;

			// adding item to the temporary artefact map
			ITEM_COUNT_PAIR_IT		k = m_tpTraderItems.find(l_tpALifeItemArtefact->s_name);
			if (m_tpTraderItems.end() == k)
				m_tpTraderItems.insert(mk_pair(l_tpALifeItemArtefact->s_name,1));
			else
				++((*k).second);
		}
	}

	// iterating on all the trader artefacts
	ITEM_COUNT_PAIR_IT				i = m_tpTraderItems.begin();
	ITEM_COUNT_PAIR_IT				e = m_tpTraderItems.end();
	for ( ; i != e; ++i) {
		m_tpSoldArtefacts.clear	();
		// iterating on all the organizations
		{
			ORGANIZATION_P_MAP::const_iterator	I = organizations().organizations().begin();
			ORGANIZATION_P_MAP::const_iterator	E = organizations().organizations().end();
			for ( ; I != E; ++I) {
				// checking if our rank is enough for the organization
				// and the organization in the appropriate state
				if (((*I).second->m_id == tTrader.m_tOrgID) && (eResearchStateJoin == (*I).second->research_state())) {
					// iterating on all the organization artefact orders
					ARTEFACT_ORDER_IT		ii = (*I).second->m_ordered_artefacts.begin();
					ARTEFACT_ORDER_IT		ee = (*I).second->m_ordered_artefacts.end();
					for ( ; ii != ee; ++ii)
						if (!xr_strcmp((*ii).m_section,(*i).first)) {
							SOrganizationOrder							l_tOrganizationOrder;
							l_tOrganizationOrder.m_tpALifeOrganization	= (*I).second;
							l_tOrganizationOrder.m_dwCount				= (*ii).m_count;
							m_tpSoldArtefacts.insert					(mk_pair((*ii).m_price,l_tOrganizationOrder));
							break;
						}
				}
			}
		}
		// iterating on all the orders in descending order in order to get the maximum profit
		{
			ORGANIZATION_ORDER_PAIR_IT	I = m_tpSoldArtefacts.begin();
			ORGANIZATION_ORDER_PAIR_IT	E = m_tpSoldArtefacts.end();
			for ( ; I != E; ++I) {
				// checking if organization has already bought these artefacts
				ITEM_COUNT_PAIR_IT		j = (*I).second.m_tpALifeOrganization->m_purchased_artefacts.find((*i).first);
				if ((*I).second.m_tpALifeOrganization->m_purchased_artefacts.end() == j)
					(*I).second.m_tpALifeOrganization->m_purchased_artefacts.insert(mk_pair((*i).first,(*I).second.m_dwCount));
				else
					(*j).second			+= (*I).second.m_dwCount;
				// adding money to trader
				tTrader.m_dwMoney		+= (*I).first*(*I).second.m_dwCount;
				// removing sold objects from trader ownership
				{
					bool						l_bFoundObject = false;
					xr_vector<u16>::iterator	ii = tTrader.children.begin();
					xr_vector<u16>::iterator	ee = tTrader.children.end();
					for ( ; ii != ee; ++ii) {
						// getting pointer to the purchased item from the object registry
						CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = objects().object(*ii);
						if (!xr_strcmp((*i).first,l_tpALifeDynamicObject->s_name)) {
							release				(l_tpALifeDynamicObject);
							l_bFoundObject		= true;
							break;
						}
					}
					R_ASSERT2					(l_bFoundObject,"Not enough objects to sell!");
				}
			}
		}
	}
}

void CALifeSurgeManager::buy_supplies(CSE_ALifeTrader &tTrader)
{
	// filling temporary map with the purchased items
	m_tpTraderItems.clear();
	{
		xr_vector<u16>::iterator	i = tTrader.children.begin();
		xr_vector<u16>::iterator	e = tTrader.children.end();
		for ( ; i != e; ++i) {
			// checking if the purchased item is an item
			CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>(objects().object(*i));
			R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory object has a parent?!");
			// adding item to the temporary item map
			ITEM_COUNT_PAIR_IT		k = m_tpTraderItems.find(l_tpALifeInventoryItem->base()->s_name);
			if (m_tpTraderItems.end() == k)
				m_tpTraderItems.insert(mk_pair(l_tpALifeInventoryItem->base()->s_name,1));
			else
				++((*k).second);
		}
	}

	// iterating on all the supply orders
	string64						S, S1;
	TRADER_SUPPLY_IT				I = tTrader.m_tpSupplies.begin();
	TRADER_SUPPLY_IT				E = tTrader.m_tpSupplies.end();
	for ( ; I != E; ++I) {
		// choosing item to purchase
		u32							l_dwIndex = randI(_GetItemCount(*(*I).m_caSections));
		_GetItem					(*(*I).m_caSections,l_dwIndex,S);
		
		// checking if item dependent discoveries are invented
		LPCSTR						l_caItemDependencies = pSettings->r_string(S,"discovery_dependency");
		bool						l_bIndependent = true;
		for (u32 i=0, n = _GetItemCount(l_caItemDependencies); i<n; ++i) {
			if (!organizations().discovery(_GetItem(S,i,S1),true)) {
				l_bIndependent = false;
				break;
			}
		}
		if (!l_bIndependent)
			continue;
		
		// counting how much items of the "item" we have to purchase
		ITEM_COUNT_PAIR_IT			j = m_tpTraderItems.find(S);
		u32							l_dwItemCount = 0;
		if (j != m_tpTraderItems.end())
			l_dwItemCount			= (*j).second;
		int							l_iDifference = int((*I).m_dwCount) - int(l_dwItemCount);
		if (l_iDifference > 0) {
			l_iDifference			= iFloor(float(l_iDifference)*randF((*I).m_fMinFactor,(*I).m_fMaxFactor) + .5f);
			// purchase an item
			for (int p=0; p<l_iDifference; ++p) {
				// create item object
				CSE_Abstract	*l_tpSE_Abstract = F_entity_Create	(S);
				R_ASSERT2		(l_tpSE_Abstract,"Can't create entity.");
				CSE_ALifeDynamicObject	*i = smart_cast<CSE_ALifeDynamicObject*>(l_tpSE_Abstract);
				R_ASSERT2		(i,"Non-ALife object in the 'game.spawn'");
				CSE_ALifeItem	*l_tpALifeItem = smart_cast<CSE_ALifeItem*>(i);
				R_ASSERT2		(l_tpALifeItem,"Non-item object in the trader supplies string!");
				
				// checking if there is enough money to buy an item
				if (l_tpALifeItem->m_dwCost > tTrader.m_dwMoney) {
					xr_delete	(l_tpSE_Abstract);
					break;
				}
				tTrader.m_dwMoney			-= l_tpALifeItem->m_dwCost;
				l_tpALifeItem->ID			= server().PerformIDgen(0xffff);
				l_tpALifeItem->ID_Parent	= tTrader.ID;
				l_tpALifeItem->m_tSpawnID	= _SPAWN_ID(-1);
				l_tpALifeItem->m_tGraphID	= tTrader.m_tGraphID;
				l_tpALifeItem->o_Position	= tTrader.o_Position;
				l_tpALifeItem->m_tNodeID	= tTrader.m_tNodeID;
				l_tpALifeItem->m_fDistance	= tTrader.m_fDistance;
				l_tpALifeItem->m_bALifeControl = true;
				register_object				(i,true);
			}
		}
	}
}

float CALifeSurgeManager::distance(const DWORD_VECTOR &path) const
{
	float							distance = 0.f;
	xr_vector<u32>::const_iterator	I = path.begin();
	xr_vector<u32>::const_iterator	E = path.end() - 1;
	for ( ; I != E; ++I) {
		CGameGraph::const_iterator	i, e;
		bool						ok = false;
		ai().game_graph().begin		(*I,i,e);
		for ( ; i != e; ++i)
			if ((*i).vertex_id() == *(I + 1)) {
				distance			+= (*i).distance();
				ok					= true;
				break;
			}
		VERIFY						(ok);
	}
	return							(distance);
}

void CALifeSurgeManager::update_tasks()
{
	tasks().clear				();
	
	// iterating on the ordered artefacts (this map is constructed from all the traders orders)
	TRADER_SET_MAP::const_iterator	I = organizations().cross_traders().begin();
	TRADER_SET_MAP::const_iterator	E = organizations().cross_traders().end();
	for ( ; I != E; ++I) {
		ITEM_SET_MAP::const_iterator	J = spawns().artefact_anomaly_map().find((LPSTR)(*I).first);
		if (spawns().artefact_anomaly_map().end() == J)
			continue;
		// iterating on the anomaly types that can generate this type of artefact
		U32_SET::const_iterator	i = (*J).second.begin();
		U32_SET::const_iterator	e = (*J).second.end();
		for ( ; i != e; ++i) {
			// iterating on all the active anomalies by the particular type
			ANOMALY_P_VECTOR::const_iterator	II = anomalies().cross()[*i].begin();
			ANOMALY_P_VECTOR::const_iterator	EE = anomalies().cross()[*i].end();
			for ( ; II != EE; ++II) {
				// iterating on all the traders who ordered this type of artefact
				float			l_fMinDistance = 10000000.f;
				TRADER_SET::const_iterator	ii = (*I).second.begin();
				TRADER_SET::const_iterator	ee = (*I).second.end(), jj = ee;
				for ( ; ii != ee; ++ii) {
					bool		successful = ai().graph_engine().search(ai().game_graph(),(*ii)->m_tGraphID,(*II)->game_vertex_id(),&m_tpTempPath,CGraphEngine::CBaseParameters());
					if (!successful)
						continue;
					float		l_fDistance = distance(m_tpTempPath);
					if (l_fDistance < l_fMinDistance) {
						l_fMinDistance = l_fDistance;
						jj = ii;
					}
				}
				R_ASSERT3				(jj != ee,"There is no way from trader to artefact",ai().game_graph().header().level(ai().game_graph().vertex((*II)->game_vertex_id())->level_id()).name());

				// creating _new task
				CALifeTask				*l_tpALifeTask = xr_new<CALifeTask>();
				l_tpALifeTask->m_tTaskType	= eTaskTypeSearchForItemCG;
				strcpy						(l_tpALifeTask->m_caSection,(*I).first);
				l_tpALifeTask->m_tGraphID	= (*II)->game_vertex_id();
				l_tpALifeTask->m_tTimeID	= time_manager().game_time();
				l_tpALifeTask->m_tCustomerID= (*jj)->ID;
#pragma todo("Dima to Dima : Correct task price")
				l_tpALifeTask->m_fCost		= 100.f;
				tasks().add					(l_tpALifeTask);
			}
		}
	}
}

void CALifeSurgeManager::assign_stalker_customers()
{
	SCHEDULE_P_MAP::const_iterator	I = scheduled().objects().begin();
	SCHEDULE_P_MAP::const_iterator	E = scheduled().objects().end();
	for ( ; I != E; ++I) {
		CSE_ALifeHumanAbstract		*l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>((*I).second);
		if (l_tpALifeHumanAbstract && xr_strlen(l_tpALifeHumanAbstract->m_caKnownCustomers)) {
//			u32						N = _GetItemCount(l_tpALifeHumanAbstract->m_caKnownCustomers);
//			if (!N)
//				continue;
//			
//			string32				S;
//			l_tpALifeHumanAbstract->m_tpKnownCustomers.clear();
//			for (u32 i=0; i<N; ++i) {
//				_GetItem(l_tpALifeHumanAbstract->m_caKnownCustomers,i,S);
//				bool				bOk = false;
//				D_OBJECT_P_MAP::const_iterator	II = objects().objects().begin();
//				D_OBJECT_P_MAP::const_iterator	EE = objects().objects().end();
//				for ( ; II != EE; ++II) {
//					CSE_ALifeTraderAbstract *l_tpTraderAbstract = smart_cast<CSE_ALifeTraderAbstract*>((*II).second);
//					if (l_tpTraderAbstract) {
//						if (!xr_strcmp((*II).second->s_name_replace,S)) {
//							l_tpALifeHumanAbstract->m_tpKnownCustomers.push_back((*II).second->ID);
//							bOk		= true;
//							break;
//						}
//					}
//				}
//				R_ASSERT3			(bOk,"There is no customer for the stalker ",l_tpALifeHumanAbstract->s_name_replace);
//			}
			l_tpALifeHumanAbstract->m_tpKnownCustomers.push_back(traders().trader_nearest(l_tpALifeHumanAbstract)->ID);
			l_tpALifeHumanAbstract->m_caKnownCustomers = 0;
		}
	}
}
