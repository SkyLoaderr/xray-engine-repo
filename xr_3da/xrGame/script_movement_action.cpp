////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_movement_action.h"
#include "script_game_object.h"

CScriptMovementAction::~CScriptMovementAction	()
{
}

void CScriptMovementAction::SetObjectToGo		(CScriptGameObject *tpObjectToGo)
{
	if (tpObjectToGo)
		m_tpObjectToGo	= tpObjectToGo->operator CObject*();
	else
		m_tpObjectToGo	= 0;
	m_tGoalType			= eGoalTypeObject;
	m_bCompleted		= false;
}
