////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_motivations.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivations classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wrapper_abstract.h"
#include "motivation.h"
#include "motivation_action.h"
#include "ai_script_classes.h"

class CAI_Stalker;

typedef CWrapperAbstract<CAI_Stalker,CMotivation>			CStalkerMotivation;
typedef CWrapperAbstract<CAI_Stalker,CMotivationAction>	CStalkerMotivationAction;

//////////////////////////////////////////////////////////////////////////
// CMotivationGlobal
//////////////////////////////////////////////////////////////////////////

class CMotivationGlobal : public CStalkerMotivation {
public:
	virtual float	evaluate		(u32 sub_motivation_id);
};

#include "stalker_motivations_inline.h"