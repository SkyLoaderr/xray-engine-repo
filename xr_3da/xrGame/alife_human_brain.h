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

class CALifeHumanObjectHandler;

class CALifeHumanBrain {
public:
	typedef CSE_ALifeHumanAbstract		object_type;
	typedef CALifeHumanObjectHandler	object_handler_type;

private:
	object_type						*m_object;
	object_handler_type				*m_object_handler;
	bool							m_failed;

// old not yet obsolete stuff
public:
	ALife::OBJECT_VECTOR			m_tpKnownCustomers;
	svector<char,5>					m_cpEquipmentPreferences;
	svector<char,4>					m_cpMainWeaponPreferences;

// old, to be obsolete
public:
	u32								m_dwTotalMoney;

public:
									CALifeHumanBrain	(object_type *object);
	virtual							~CALifeHumanBrain	();

public:
	virtual	void					on_state_write		(NET_Packet &packet);
	virtual	void					on_state_read		(NET_Packet &packet);
	virtual	void					on_surge			();
	virtual	void					on_register			();
	virtual	void					on_unregister		();

public:
			void					update				();
			bool					perform_attack		();
			ALife::EMeetActionType	action_type			(CSE_ALifeSchedulable *tpALifeSchedulable, const int &iGroupIndex, const bool &bMutualDetection);

public:
	IC		object_type				&object				() const;
	IC		object_handler_type		&object_handler		() const;
	IC		bool					failed				() const;
};

#include "alife_human_brain_inline.h"