////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_surge_manager_inline.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator surge manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeSurgeManager::CALifeSurgeManager				(xrServer *server, LPCSTR section) :
	inherited		(server,section)
{
}

IC	ALife::_TIME_ID	CALifeSurgeManager::last_surge_time	() const
{
	return			(m_last_surge_time);
}

IC	ALife::_TIME_ID	CALifeSurgeManager::next_surge_time	() const
{
	return			(m_next_surge_time);
}
