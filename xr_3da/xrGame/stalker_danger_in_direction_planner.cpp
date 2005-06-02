////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_in_direction_planner.cpp
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger in direction planner class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_danger_in_direction_planner.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

CStalkerDangerInDirectionPlanner::CStalkerDangerInDirectionPlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerDangerInDirectionPlanner::add_evaluators				()
{
}

void CStalkerDangerInDirectionPlanner::add_actions					()
{
}

void CStalkerDangerInDirectionPlanner::setup						(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup		(object,storage);
}

void CStalkerDangerInDirectionPlanner::initialize					()
{
	inherited::initialize	();
}

void CStalkerDangerInDirectionPlanner::update						()
{
	inherited::update		();
}

void CStalkerDangerInDirectionPlanner::finalize						()
{
	inherited::finalize		();
}
