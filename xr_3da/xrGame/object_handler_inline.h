////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CObjectHandler::firing		() const
{
	return					(m_bFiring);
}
