////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_player_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound player inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CSoundPlayer::set_sound_mask		(u32 sound_mask)
{
	m_sound_mask						= sound_mask;
	remove_inappropriate_sounds			(m_sound_mask);
}

IC	void CSoundPlayer::remove_active_sounds	(u32 sound_mask)
{
	u32									save_sound_mask = m_sound_mask;
	set_sound_mask						(sound_mask);
	set_sound_mask						(save_sound_mask);
}

IC	const xr_vector<CSoundPlayer::CSoundSingle> &CSoundPlayer::playing_sounds() const
{
	return								(m_playing_sounds);
}

IC	u32	CSoundPlayer::active_sound_count(bool only_playing) const
{
	u32									count = 0;
	xr_vector<CSoundSingle>::const_iterator	I = m_playing_sounds.begin();
	xr_vector<CSoundSingle>::const_iterator	E = m_playing_sounds.end();
	if (!only_playing) {
		for ( ; I != E; ++I)
			if ((*I).m_sound->feedback || ((*I).m_start_time <= Device.dwTimeGlobal))
				++count;
	}
	else {
		for ( ; I != E; ++I)
			if ((*I).m_sound->feedback)
				++count;
	}
	return								(count);
}

IC	ref_sound *CSoundPlayer::add			(ESoundTypes type, LPCSTR name, CSound_UserDataPtr data) const
{
	ref_sound			*temp = xr_new<ref_sound>();
	temp->create		(TRUE,name,type);
	temp->g_object		= m_object;
	temp->g_userdata	= data;
	return				(temp);
}