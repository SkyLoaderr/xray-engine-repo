////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_sound.h"
#include "script_game_object.h"
#include "gameobject.h"
#include "ai_space.h"
#include "script_engine.h"

CScriptSound::CScriptSound				(LPCSTR caSoundName, ESoundTypes sound_type)
{
	m_caSoundToPlay			= caSoundName;
	string256				l_caFileName;
	
	if (FS.exist(l_caFileName,"$game_sounds$",caSoundName,".ogg"))
		m_sound.create		(TRUE,caSoundName,sound_type);
	else
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"File not found \"%s\"!",l_caFileName);
}

CScriptSound::~CScriptSound		()
{
	THROW3					(!m_sound.feedback,"playing sound is not completed, but is destroying",m_sound.handle ? m_sound.handle->file_name() : "unknown");
	m_sound.destroy			();
}

const Fvector &CScriptSound::GetPosition() const
{
	VERIFY				(m_sound.handle);
	const CSound_params	*l_tpSoundParams = m_sound.get_params();
	if (l_tpSoundParams)
		return			(l_tpSoundParams->position);
	else {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"Sound was not launched, can't get position!");
		return			(Fvector().set(0,0,0));
	}
}

void CScriptSound::Play			(CScriptGameObject *object, float delay, int flags)
{
	VERIFY				(m_sound.handle);
//	Msg					("%6d : CScriptSound::Play (%s), delay %f, flags %d",Device.dwTimeGlobal,m_sound.handle->file_name(),delay,flags);
	m_sound.play		(&object->object(),flags,delay);
}

void CScriptSound::PlayUnlimited	(CScriptGameObject *object, float delay, int flags)
{
	VERIFY				(m_sound.handle);
//	Msg					("%6d : CScriptSound::Play (%s), delay %f, flags %d",m_sound.handle->file_name(),delay,flags);
	VERIFY				((flags & sm_Looped) != sm_Looped);
	m_sound.play_unlimited(&object->object(),flags,delay);
}

void CScriptSound::PlayAtPos		(CScriptGameObject *object, const Fvector &position, float delay, int flags)
{
	VERIFY				(m_sound.handle);
//	Msg					("%6d : CScriptSound::Play (%s), delay %f, flags %d",m_sound.handle->file_name(),delay,flags);
	m_sound.play_at_pos(&object->object(), position,flags,delay);
}

void CScriptSound::PlayAtPosUnlimited(CScriptGameObject *object, const Fvector &position, float delay, int flags)
{
	VERIFY				(m_sound.handle);
	VERIFY				((flags & sm_Looped) != sm_Looped);
//	Msg					("%6d : CScriptSound::Play (%s), delay %f, flags %d",m_sound.handle->file_name(),delay,flags);
	m_sound.play_at_pos_unlimited(&object->object(), position,flags,delay);
}