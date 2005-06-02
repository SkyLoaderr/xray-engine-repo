////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_grenade_planner.cpp
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger grenade planner class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_danger_grenade_planner.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

CStalkerDangerGrenadePlanner::CStalkerDangerGrenadePlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerDangerGrenadePlanner::add_evaluators			()
{
}

void CStalkerDangerGrenadePlanner::add_actions				()
{
}

void CStalkerDangerGrenadePlanner::setup					(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup		(object,storage);
}

void CStalkerDangerGrenadePlanner::initialize				()
{
	inherited::initialize	();
}

void CStalkerDangerGrenadePlanner::update					()
{
	inherited::update		();
}

void CStalkerDangerGrenadePlanner::finalize					()
{
	inherited::finalize		();
}
