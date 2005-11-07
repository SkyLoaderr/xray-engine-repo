////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_brain.h
//	Created 	: 06.10.2005
//  Modified 	: 06.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human brain class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"
#include "xrserver_space.h"
#include "alife_space.h"
#include "script_export_space.h"

class CALifeHumanObjectHandler;
class CALifeHumanMovementManager;
class CSE_ALifeSmartZone;
class NET_Packet;

class CALifeHumanBrain {
public:
	typedef CSE_ALifeHumanAbstract		object_type;
	typedef CALifeHumanObjectHandler	object_handler_type;
	typedef CALifeHumanMovementManager	movement_manager_type;

private:
	object_type							*m_object;
	object_handler_type					*m_object_handler;
	movement_manager_type				*m_movement_manager;

// old not yet obsolete stuff
public:
	ALife::OBJECT_VECTOR				m_tpKnownCustomers;
	svector<char,5>						m_cpEquipmentPreferences;
	svector<char,4>						m_cpMainWeaponPreferences;

// old, to be obsolete
public:
	u32									m_dwTotalMoney;

public:
	CSE_ALifeSmartZone					*m_smart_terrain;
	ALife::_TIME_ID						m_last_search_time;
	ALife::_TIME_ID						m_time_interval;

private:
			void						select_task			();
			void						process_task		();
			void						default_behaviour	();

public:
										CALifeHumanBrain	(object_type *object);
	virtual								~CALifeHumanBrain	();

public:
			void						on_state_write		(NET_Packet &packet);
			void						on_state_read		(NET_Packet &packet);
			void						on_surge			();
			void						on_register			();
			void						on_unregister		();
			void						on_location_change	();

public:
			void						update				();
			bool						perform_attack		();
			ALife::EMeetActionType		action_type			(CSE_ALifeSchedulable *tpALifeSchedulable, const int &iGroupIndex, const bool &bMutualDetection);

public:
	IC		object_type					&object				() const;
	IC		object_handler_type			&objects			() const;
	IC		movement_manager_type		&movement			() const;
	IC		CSE_ALifeSmartZone			&smart_terrain		();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CALifeHumanBrain)
#undef script_type_list
#define script_type_list save_type_list(CALifeHumanBrain)

#include "alife_human_brain_inline.h"