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
#include "stalker_animation_manager.h"

CStalkerActionBase::CStalkerActionBase		(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionBase::initialize			()
{
	inherited::initialize	();
	m_object->animation().clear_script_animations	();
	m_object->body_action	(eBodyActionNone);
}

void CStalkerActionBase::finalize			()
{
	inherited::finalize		();
	m_object->animation().clear_script_animations	();
}
