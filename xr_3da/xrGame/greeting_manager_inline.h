////////////////////////////////////////////////////////////////////////////
//	Module 		: greeting_manager_inline.h
//	Created 	: 09.12.2004
//  Modified 	: 09.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Greeting manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CGreetingManager::GREETINGS &CGreetingManager::greetings	() const
{
	return				(objects());
}
