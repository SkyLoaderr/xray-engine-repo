////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator.cpp
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_simulator.h"
#include "xrServer_Objects_ALife.h"
#include "ai_space.h"

LPCSTR alife_section = "alife";

CALifeSimulator::CALifeSimulator		(xrServer *server, shared_str* command_line) :
	CALifeUpdateManager			(server,alife_section),
	CALifeInteractionManager	(server,alife_section),
	CALifeSimulatorBase			(server,alife_section)
{
	VERIFY						(!ai().get_alife());
	ai().set_alife				(this);
	setup_command_line			(*command_line);

	string64					saved_game;
	strcpy						(saved_game,**command_line);
	LPSTR						temp = strchr(saved_game,'/');
	R_ASSERT2					(temp,"Invalid server options!");
	*temp						= 0;
	load						(saved_game,true);
}

CALifeSimulator::~CALifeSimulator		()
{
	VERIFY						(ai().get_alife());
	ai().set_alife				(0);
}

void CALifeSimulator::setup_simulator	(CSE_ALifeObject *object)
{
//	VERIFY2						(!object->m_alife_simulator,object->s_name_replace);
	object->m_alife_simulator	= this;
}

void CALifeSimulator::reload			(LPCSTR section)
{
	CALifeUpdateManager::reload	(section);
}
