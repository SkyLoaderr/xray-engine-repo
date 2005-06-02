////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_by_sound_planner.cpp
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger by sound planner class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_danger_by_sound_planner.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

CStalkerDangerBySoundPlanner::CStalkerDangerBySoundPlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerDangerBySoundPlanner::add_evaluators			()
{
}

void CStalkerDangerBySoundPlanner::add_actions				()
{
}

void CStalkerDangerBySoundPlanner::setup					(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup		(object,storage);
}

void CStalkerDangerBySoundPlanner::initialize				()
{
	inherited::initialize	();
}

void CStalkerDangerBySoundPlanner::update					()
{
	inherited::update		();
}

void CStalkerDangerBySoundPlanner::finalize					()
{
	inherited::finalize		();
}
