////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_action_wrapper.cpp
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script motivation action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_action_wrapper.h"
#include "script_game_object.h"

CScriptMotivationActionWrapper::~CScriptMotivationActionWrapper	()
{
}

void CScriptMotivationActionWrapper::reinit			(_object_type *object)
{
	luabind::call_member<void>				(this,"reinit",object);
}

void CScriptMotivationActionWrapper::reinit_static	(inherited *motivation, CScriptGameObject *object)
{
	motivation->inherited::reinit	(object);
}

void CScriptMotivationActionWrapper::Load				(LPCSTR section)
{
	luabind::call_member<void>				(this,"load",section);
}

void CScriptMotivationActionWrapper::Load_static		(inherited *motivation, LPCSTR section)
{
	motivation->inherited::Load		(section);
}

void CScriptMotivationActionWrapper::reload			(LPCSTR section)
{
	luabind::call_member<void>				(this,"reload",section);
}

void CScriptMotivationActionWrapper::reload_static	(inherited *motivation, LPCSTR section)
{
	motivation->inherited::reload	(section);
}

float CScriptMotivationActionWrapper::evaluate		(u32 sub_motivation_id)
{
	return							(luabind::call_member<float>(this,"evaluate",sub_motivation_id));
}

float CScriptMotivationActionWrapper::evaluate_static	(inherited *motivation, u32 sub_motivation_id)
{
	return							(motivation->inherited::evaluate(sub_motivation_id));
}