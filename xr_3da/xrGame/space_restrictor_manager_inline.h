////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor_manager_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSpaceRestrictorManager::CSpaceRestrictorManager	()
{
}

IC	const CSpaceRestriction *CSpaceRestrictorManager::restriction	(ALife::_OBJECT_ID id)
{
	CLIENT_REGISTRY::const_iterator	I = m_clients.find(id);
	VERIFY							(m_clients.end() != I);
	return							((*I).second);
}