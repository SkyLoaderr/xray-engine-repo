////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator.h
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_interaction_manager.h"
#include "alife_update_manager.h"

class CALifeSimulator : 
	public CALifeUpdateManager,
	public CALifeInteractionManager
{
protected:
	virtual void	setup_simulator		(CSE_ALifeObject *object);
	virtual void	reload				(LPCSTR section);

public:
					CALifeSimulator		(xrServer *server, LPSTR *command_line);
	virtual			~CALifeSimulator	();
};

#include "alife_simulator_inline.h"