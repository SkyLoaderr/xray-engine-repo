////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_sound.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_sound.h"
#include "ai_script_classes.h"

void CLuaSound::Play			(CLuaGameObject *object)
{
	VERIFY				(m_tpSound);
	m_tpSound->play		(object->object());
}

void CLuaSound::PlayUnlimited	(CLuaGameObject *object)
{
	VERIFY				(m_tpSound);
	m_tpSound->play_unlimited(object->object());
}

void CLuaSound::PlayAtPos		(CLuaGameObject *object, const Fvector &position)
{
	VERIFY				(m_tpSound);
	m_tpSound->play_at_pos(object->object(), position);
}

void CLuaSound::PlayAtPosUnlimited(CLuaGameObject *object, const Fvector &position)
{
	VERIFY				(m_tpSound);
	m_tpSound->play_at_pos_unlimited(object->object(), position);
}