////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor_manager.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSpaceRestrictor;

class CSpaceRestrictorManager {
protected:

public:
	IC				CSpaceRestrictorManager		();
	virtual			~CSpaceRestrictorManager	();
			void	add							(CSpaceRestrictor *space_restrictor);
};

#include "space_restrictor_manager_inline.h"