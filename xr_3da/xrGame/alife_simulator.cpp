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

union params {
	struct {
		string256	m_game_or_spawn;
		string256	m_game_type;
		string256	m_alife;
		string256	m_new_or_load;
	};
	string256		m_params[4];

	IC				params()
	{
		for (int i=0; i<4; ++i)
			strcpy				(m_params[i],"");
	}
};

CALifeSimulator::CALifeSimulator		(xrServer *server, shared_str *command_line) :
	CALifeUpdateManager			(server,alife_section),
	CALifeInteractionManager	(server,alife_section),
	CALifeSimulatorBase			(server,alife_section)
{
	VERIFY						(!ai().get_alife());
	ai().set_alife				(this);
	setup_command_line			(*command_line);

	params						p;
	int							n = _GetItemCount(**command_line,'/');
	for (int i=0; i<n; ++i) {
		_GetItem				(**command_line,i,p.m_params[i],'/');
		strlwr					(p.m_params[i]);
	}
	
	R_ASSERT2					(
		xr_strlen(p.m_game_or_spawn) && 
		!xr_strcmp(p.m_alife,"alife") && 
		!xr_strcmp(p.m_game_type,"single"),
		"Invalid server options!"
	);
	
	string256					temp;
	strcpy						(temp,p.m_game_or_spawn);
	strcat						(temp,"/");
	strcat						(temp,p.m_game_type);
	strcat						(temp,"/");
	strcat						(temp,p.m_alife);
	*command_line				= temp;
	
	load						(p.m_game_or_spawn,!xr_strcmp(p.m_new_or_load,"load") ? false : true, !xr_strcmp(p.m_new_or_load,"new"));
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
