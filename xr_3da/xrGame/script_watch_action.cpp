////////////////////////////////////////////////////////////////////////////
//	Module 		: script_watch_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script watch action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_watch_action.h"
#include "script_game_object.h"

CScriptWatchAction::~CScriptWatchAction	()
{
}

void CScriptWatchAction::SetWatchObject	(CScriptGameObject *tpObjectToWatch)
{
	m_tpObjectToWatch	= tpObjectToWatch->operator CObject*();
	m_tGoalType			= eGoalTypeObject;
	m_bCompleted		= false;
}
