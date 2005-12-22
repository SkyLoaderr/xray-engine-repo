////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_velocity_holder.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker velocity holder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "associative_vector.h"

class CStalkerVelocityHolder {
public:
	typedef associative_vector<> ;

private:

public:
					CStalkerVelocityHolder	();
	virtual			~CStalkerVelocityHolder	();
};