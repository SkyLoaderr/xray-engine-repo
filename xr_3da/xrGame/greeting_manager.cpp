////////////////////////////////////////////////////////////////////////////
//	Module 		: greeting_manager.cpp
//	Created 	: 09.12.2004
//  Modified 	: 09.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Greeting manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "greeting_manager.h"

CGreetingManager::CGreetingManager	(CCustomMonster *object)
{
	VERIFY				(object);
	m_object			= smart_cast<CAI_Stalker*>(object);
}

void CGreetingManager::reload		(LPCSTR section)
{
}

bool CGreetingManager::useful		(const CAI_Stalker *object) const
{
	return				(false);
}

float CGreetingManager::evaluate	(const CAI_Stalker *object) const
{
	return				(0.f);
}

void CGreetingManager::update		()
{
	if (!m_object)
		return;

	inherited::update			();
}
