////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_cover_location.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Danger cover location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "danger_location.h"

class CCoverPoint;

class CDangerCoverLocation : public CDangerLocation {
private:
	CCoverPoint				*m_cover;
	
public:
	IC						CDangerCoverLocation	(CCoverPoint *cover, u32 level_time, u32 interval, float radius);
	virtual const Fvector	&position				() const;
};

#include "danger_cover_location_inline.h"