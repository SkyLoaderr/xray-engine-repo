////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_player_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound player inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	Fvector CSoundPlayer::compute_sound_point(const CSoundSingle &sound)
{
	Fmatrix								l_tMatrix;
	l_tMatrix.mul_43					(m_object->XFORM(),PKinematics(m_object->Visual())->LL_GetBoneInstance(sound.m_bone_id).mTransform);
	return								(l_tMatrix.c);
}

IC	void CSoundPlayer::set_sound_mask		(u32 sound_mask)
{
	m_sound_mask						= sound_mask;
	remove_inappropriate_sounds			(m_sound_mask);
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
			if ((*I).m_sound->feedback || ((*I).m_start_time <= Level().timeServer()))
				++count;
	}
	else {
		for ( ; I != E; ++I)
			if ((*I).m_sound->feedback)
				++count;
	}
	return								(count);
}
