////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CMotivation {
protected:
public:
	virtual float	evaluate	(u32 sub_motivation_id);
};

#include "motivation_inline.h"