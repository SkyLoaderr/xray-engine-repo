////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_wrapper.cpp
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script motivation wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_wrapper.h"
#include "ai_script_classes.h"

CScriptMotivationWrapper::~CScriptMotivationWrapper	()
{
}

void CScriptMotivationWrapper::reinit			(_object_type *object)
{
	call_member<void>				("reinit",object);
}

void CScriptMotivationWrapper::reinit_static	(inherited *motivation, _object_type *object)
{
	motivation->inherited::reinit	(object);
}

void CScriptMotivationWrapper::Load				(LPCSTR section)
{
	call_member<void>				("load",section);
}

void CScriptMotivationWrapper::Load_static		(inherited *motivation, LPCSTR section)
{
	motivation->inherited::Load		(section);
}

void CScriptMotivationWrapper::reload			(LPCSTR section)
{
	call_member<void>				("reload",section);
}

void CScriptMotivationWrapper::reload_static	(inherited *motivation, LPCSTR section)
{
	motivation->inherited::reload	(section);
}

float CScriptMotivationWrapper::evaluate		(u32 sub_motivation_id)
{
	return									(call_member<float>("evaluate",sub_motivation_id));
}

float CScriptMotivationWrapper::evaluate_static	(inherited *motivation, u32 sub_motivation_id)
{
	return									(motivation->inherited::evaluate(sub_motivation_id));
}