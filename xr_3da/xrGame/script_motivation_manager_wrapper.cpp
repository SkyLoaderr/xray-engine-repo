////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_manager_wrapper.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script manager manager wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_manager_wrapper.h"

CScriptMotivationManagerWrapper::~CScriptMotivationManagerWrapper	()
{
}

void CScriptMotivationManagerWrapper::reinit			(_object_type *object, bool clear_all)
{
	call_member<void>						("reinit",object,clear_all);
}

void CScriptMotivationManagerWrapper::reinit_static	(CScriptMotivationManager *manager, CLuaGameObject *object, bool clear_all)
{
	manager->CScriptMotivationManager::reinit		(object,clear_all);
}

void CScriptMotivationManagerWrapper::Load			(LPCSTR section)
{
	call_member<void>						("load",section);
}

void CScriptMotivationManagerWrapper::Load_static		(CScriptMotivationManager *manager, LPCSTR section)
{
	manager->CScriptMotivationManager::Load		(section);
}

void CScriptMotivationManagerWrapper::reload			(LPCSTR section)
{
	call_member<void>						("reload",section);
}

void CScriptMotivationManagerWrapper::reload_static	(CScriptMotivationManager *manager, LPCSTR section)
{
	manager->CScriptMotivationManager::reload	(section);
}

void CScriptMotivationManagerWrapper::update			(u32 time_delta)
{
	call_member<void>						("update",time_delta);
}

void CScriptMotivationManagerWrapper::update_static	(CScriptMotivationManager *manager, u32 time_delta)
{
	manager->CScriptMotivationManager::update	(time_delta);
}