////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_personal.cpp
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife personal event class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "alife_event_personal.h"

CALifeEventPersonal::~CALifeEventPersonal	()
{
}

void CALifeEventPersonal::load				(IReader &stream)
{
	load_data					(stream);
}

void CALifeEventPersonal::save				(IWriter &stream)
{
	save_data					(stream);
}

void CALifeEventPersonal::load				(NET_Packet &packet)
{
	load_data					(packet);
}

void CALifeEventPersonal::save				(NET_Packet &packet)
{
	save_data					(packet);
}
