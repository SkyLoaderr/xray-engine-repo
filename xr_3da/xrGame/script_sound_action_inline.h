////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_action_inline.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound action class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptSoundAction::CScriptSoundAction	()
{
	m_caSoundToPlay		= "";
	m_caBoneName		= "";
	m_tGoalType			= eGoalTypeDummy;
	m_bStartedToPlay	= false;
	m_bLooped			= false;
	m_tSoundPosition.set(0,0,0);
	m_tSoundAngles.set	(0,0,0);
	m_bCompleted		= true;
	m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

IC	CScriptSoundAction::CScriptSoundAction		(LPCSTR caSoundToPlay, LPCSTR caBoneName, const Fvector &tPositionOffset, const Fvector &tAngleOffset, bool bLooped, ESoundTypes sound_type)
{
	m_bLooped			= bLooped;
	SetBone				(caBoneName);
	SetPosition			(tPositionOffset);
	SetAngles			(tAngleOffset);
	SetSound			(caSoundToPlay);
	SetSoundType		(sound_type);
	m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

IC	CScriptSoundAction::CScriptSoundAction		(LPCSTR caSoundToPlay, const Fvector &tPosition, const Fvector &tAngleOffset, bool bLooped, ESoundTypes sound_type)
{
	m_bLooped			= bLooped;
	SetSound			(caSoundToPlay);
	SetPosition			(tPosition);
	SetAngles			(tAngleOffset);
	SetSoundType		(sound_type);
	m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

IC	CScriptSoundAction::CScriptSoundAction		(CScriptSound &sound, LPCSTR caBoneName, const Fvector &tPositionOffset, const Fvector &tAngleOffset, bool bLooped, ESoundTypes sound_type)
{
	m_bLooped			= bLooped;
	SetBone				(caBoneName);
	SetPosition			(tPositionOffset);
	SetAngles			(tAngleOffset);
	SetSound			(sound);
	SetSoundType		(sound_type);
	m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

IC	CScriptSoundAction::CScriptSoundAction		(CScriptSound &sound, const Fvector &tPosition, const Fvector &tAngleOffset, bool bLooped, ESoundTypes sound_type)
{
	m_bLooped			= bLooped;
	SetSound			(sound);
	SetPosition			(tPosition);
	SetAngles			(tAngleOffset);
	SetSoundType		(sound_type);
	m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

////////////////////////////////////////////////////////////////////////////////////
// Monster Specific
///////////////////////////////////////////////////////////////////////////////////
						
IC	CScriptSoundAction::CScriptSoundAction		(MonsterSpace::EMonsterSounds sound_type)
{
	m_monster_sound			= sound_type;
	m_monster_sound_delay	= int (-1);
	m_bCompleted			= false;
}

IC	CScriptSoundAction::CScriptSoundAction		(MonsterSpace::EMonsterSounds sound_type, int delay)
{
	m_monster_sound			= sound_type;
	m_monster_sound_delay	= delay;
	m_bCompleted			= false;
}

////////////////////////////////////////////////////////////////////////////////////
// Trader Specific
///////////////////////////////////////////////////////////////////////////////////

IC	CScriptSoundAction::CScriptSoundAction		(LPCSTR caSoundToPlay, LPCSTR caBoneName, MonsterSpace::EMonsterHeadAnimType head_anim_type)
{
	SetBone				(caBoneName);
	SetSound			(caSoundToPlay);		
	m_tSoundPosition.set(0,0,0);
	m_tSoundAngles.set	(0,0,0);
	m_bCompleted		= false;
	m_bLooped			= false;
	m_tHeadAnimType		= head_anim_type;
	m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
}

///////////////////////////////////////////////////////////////////////////////////

IC	void CScriptSoundAction::SetSound			(LPCSTR caSoundToPlay)
{
	m_caSoundToPlay		= caSoundToPlay;
	m_tGoalType			= eGoalTypeSoundAttached;
	m_bStartedToPlay	= false;
	string256			l_caFileName;
	if (FS.exist(l_caFileName,"$game_sounds$",*m_caSoundToPlay,".ogg")) {
		m_bStartedToPlay= false;
		m_bCompleted	= false;
	}
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"File not found \"%s\"!",l_caFileName);
		m_bStartedToPlay= true;
		m_bCompleted	= true;
	}
}

IC	void CScriptSoundAction::SetSound			(const CScriptSound &sound)
{
	m_caSoundToPlay		= sound.m_caSoundToPlay;
	m_tGoalType			= eGoalTypeSoundAttached;
	m_bStartedToPlay	= false;
	m_bCompleted		= false;
}

IC	void CScriptSoundAction::SetPosition		(const Fvector &tPosition)
{
	m_tSoundPosition	= tPosition;
	m_tGoalType			= eGoalTypeSoundPosition;
	m_bStartedToPlay	= false;
}

IC	void CScriptSoundAction::SetBone			(LPCSTR caBoneName)
{
	m_caBoneName		= caBoneName;
	m_bStartedToPlay	= false;
}

IC	void CScriptSoundAction::SetAngles			(const Fvector &tAngles)
{
	m_tSoundAngles		= tAngles;
	m_bStartedToPlay	= false;
}

IC	void CScriptSoundAction::SetSoundType		(const ESoundTypes sound_type)
{
	m_sound_type		= sound_type;
	m_bStartedToPlay	= false;
}

IC	void CScriptSoundAction::initialize			()
{
	m_bStartedToPlay	= false;
}
