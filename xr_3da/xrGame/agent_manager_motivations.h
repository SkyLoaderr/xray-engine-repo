////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_motivations.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager motivations
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation.h"
#include "motivation_action.h"
#include "ai_script_classes.h"

class CAgentManager;

typedef CMotivation<CAgentManager>			CAgentManagerMotivation;
typedef CMotivationAction<CAgentManager>	CAgentManagerMotivationAction;

//////////////////////////////////////////////////////////////////////////
// CAgentManagerMotivationGlobal
//////////////////////////////////////////////////////////////////////////

class CAgentManagerMotivationGlobal : public CAgentManagerMotivation {
public:
	virtual float	evaluate		(u32 sub_motivation_id);
};

#include "agent_manager_motivations_inline.h"