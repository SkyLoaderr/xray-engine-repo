////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_sound.h"
#include "ai_script_classes.h"

CScriptSound::~CScriptSound		()
{
	m_sound.destroy();
}

void CScriptSound::Play			(CLuaGameObject *object, float delay, int flags)
{
	VERIFY				(m_sound.handle);
	m_sound.play		(object->object(),flags,delay);
}

void CScriptSound::PlayUnlimited	(CLuaGameObject *object, float delay, int flags)
{
	VERIFY				(m_sound.handle);
	VERIFY				((flags & sm_Looped) != sm_Looped);
	m_sound.play_unlimited(object->object(),flags,delay);
}

void CScriptSound::PlayAtPos		(CLuaGameObject *object, const Fvector &position, float delay, int flags)
{
	VERIFY				(m_sound.handle);
	m_sound.play_at_pos(object->object(), position,flags,delay);
}

void CScriptSound::PlayAtPosUnlimited(CLuaGameObject *object, const Fvector &position, float delay, int flags)
{
	VERIFY				(m_sound.handle);
	VERIFY				((flags & sm_Looped) != sm_Looped);
	m_sound.play_at_pos_unlimited(object->object(), position,flags,delay);
}