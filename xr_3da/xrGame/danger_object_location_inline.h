////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_object_location_inline.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Danger object location inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CDangerObjectLocation::CDangerObjectLocation	(const CGameObject *object, u32 level_time, u32 interval, float radius)
{
	VERIFY			(object);
	m_object		= object;
	m_level_time	= level_time;
	m_interval		= interval;
	m_radius		= radius;
}
