////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_location.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Danger location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "intrusive_ptr.h"

class CObject;

class CDangerLocation : public intrusive_base {
public:
	u32						m_level_time;
	u32						m_interval;
	float					m_radius;

public:
	IC		bool			operator==	(const Fvector &position) const;
	virtual	bool			operator==	(const CObject *object) const;
	virtual bool			useful		() const;
	virtual const Fvector	&position	() const = 0;
};

#include "danger_location_inline.h"