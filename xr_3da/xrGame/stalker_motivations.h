////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_motivations.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivations classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action.h"

class CAI_Stalker;

typedef CMotivation<CAI_Stalker>		CStalkerMotivation;
typedef CMotivationAction<CAI_Stalker>	CStalkerMotivationAction;

//////////////////////////////////////////////////////////////////////////
// CMotivationGlobal
//////////////////////////////////////////////////////////////////////////

class CMotivationGlobal : public CStalkerMotivation {
public:
	virtual float	evaluate		(u32 sub_motivation_id);
};

#include "stalker_motivations_inline.h"