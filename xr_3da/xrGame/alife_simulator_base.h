////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_base.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator base class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "random32.h"
#include "alife_space.h"

class xrServer;
class CALifeSimulatorHeader;
class CALifeTimeManager;
class CALifeSpawnRegistry;
class CALifeObjectRegistry;
class CALifeEventRegistry; 
class CALifeTaskRegistry;
class CALifeGraphRegistry;
class CALifeTraderRegistry;
class CALifeScheduleRegistry;
class CALifeAnomalyRegistry;
class CALifeOrganizationRegistry;
class CALifeNewsRegistry;

class CSE_Abstract;
class CSE_ALifeObject;
class CSE_ALifeDynamicObject;
class CSE_ALifeGroupAbstract;
class CSE_ALifeCreatureAbstract;

class CALifeSimulatorBase {
protected:
	xrServer									*m_server;
	CALifeSimulatorHeader						*m_header;
	CALifeTimeManager							*m_time_manager;
	CALifeSpawnRegistry							*m_spawns;
	CALifeObjectRegistry						*m_objects;
	CALifeEventRegistry							*m_events; 
	CALifeTaskRegistry							*m_tasks; 
	CALifeGraphRegistry							*m_graph_objects;
	CALifeTraderRegistry						*m_traders;
	CALifeScheduleRegistry						*m_scheduled;
	CALifeAnomalyRegistry						*m_anomalies;
	CALifeOrganizationRegistry					*m_organizations;
	CALifeNewsRegistry							*m_news;
	CRandom32									m_random;
	bool										m_initialized;
	LPSTR										*m_server_command_line;
	// temp
	ALife::SCHEDULE_P_VECTOR					m_tpaCombatGroups[2];

protected:
	IC		CALifeSimulatorHeader				&header						();
	IC		CALifeTimeManager					&time						();
	IC		CALifeSpawnRegistry					&spawns						();
	IC		CALifeObjectRegistry				&objects					();
	IC		CALifeEventRegistry					&events						();
	IC		CALifeTaskRegistry					&tasks						();
	IC		CALifeTraderRegistry				&traders					();
	IC		CALifeScheduleRegistry				&scheduled					();
	IC		CALifeAnomalyRegistry				&anomalies					();
	IC		CALifeOrganizationRegistry			&organizations				();
	IC		CALifeNewsRegistry					&news						();
public:
	IC		CALifeGraphRegistry					&graph						();
	IC		CALifeTimeManager					&time_manager				();

public:
												CALifeSimulatorBase			(xrServer *server, LPCSTR section);
	virtual										~CALifeSimulatorBase		();
	IC		bool								initialized					() const;
	IC		const CALifeSimulatorHeader			&header						() const;
	IC		const CALifeTimeManager				&time						() const;
	IC		const CALifeSpawnRegistry			&spawns						() const;
	IC		const CALifeObjectRegistry			&objects					() const;
	IC		const CALifeEventRegistry			&events						() const;
	IC		const CALifeTaskRegistry			&tasks						() const;
	IC		const CALifeGraphRegistry			&graph						() const;
	IC		const CALifeTraderRegistry			&traders					() const;
	IC		const CALifeScheduleRegistry		&scheduled					() const;
	IC		const CALifeAnomalyRegistry			&anomalies					() const;
	IC		const CALifeOrganizationRegistry	&organizations				() const;
	IC		const CALifeNewsRegistry			&news						() const;
	IC		CRandom32							&random						();
	IC		xrServer							&server						() const;
	IC		const CALifeTimeManager				&time_manager				() const;

protected:
			void								create						(CSE_ALifeDynamicObject	*&object, CSE_ALifeDynamicObject *spawn_object,	const ALife::_SPAWN_ID &spawn_id);
			void								update						(CSE_ALifeDynamicObject	*object, bool add_object = false);
			void								unload						();
	virtual	void								reload						(LPCSTR section);
	IC		void								setup_command_line			(LPSTR &command_line);
	IC		LPSTR								*server_command_line		() const;
			void								assign_death_position		(CSE_ALifeCreatureAbstract *tpALifeCreatureAbstract, ALife::_GRAPH_ID tGraphID,	CSE_ALifeSchedulable *tpALifeSchedulable = 0);
	virtual void								setup_simulator				(CSE_ALifeObject *object) = 0;
			void								remove						(CSE_ALifeDynamicObject *object, bool alife_query = true);

public:
			void								release						(CSE_Abstract			*object, bool alife_query = true);
			void								create						(CSE_ALifeObject		*object);
			CSE_Abstract						*create						(CSE_ALifeGroupAbstract	*object, CSE_ALifeDynamicObject	*j);
			CSE_Abstract						*spawn_item					(LPCSTR section,		const Fvector &position, u32 level_vertex_id, ALife::_GRAPH_ID game_vertex_id, u16 parent_id);
			void								append_item_vector			(ALife::OBJECT_VECTOR	&tObjectVector,	ALife::ITEM_P_VECTOR &tItemList);

public:
	// temp
	ALife::ITEM_P_VECTOR						m_temp_item_vector;
};

#include "alife_simulator_base_inline.h"