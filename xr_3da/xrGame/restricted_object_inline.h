////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object_inline.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CRestrictedObject::CRestrictedObject	()
{
}

IC	bool CRestrictedObject::applied			() const
{
	return				(m_applied);
}

