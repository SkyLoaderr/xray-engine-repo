////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_base_action.cpp
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker base action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_base_action.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

CStalkerActionBase::CStalkerActionBase		(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionBase::initialize			()
{
	inherited::initialize	();
	m_object->m_script_animations.clear	();
}

void CStalkerActionBase::finalize			()
{
	inherited::finalize		();
	m_object->m_script_animations.clear	();
}
