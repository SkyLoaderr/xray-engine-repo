////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_player_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound player inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	Fvector CSoundPlayer::compute_sound_point(const CSoundParams &sound)
{
	CObject								*object = dynamic_cast<CObject*>(this);
	VERIFY								(object);

	Fmatrix								l_tMatrix;
	CBoneInstance						&l_tBoneInstance = PKinematics(object->Visual())->LL_GetBoneInstance(PKinematics(object->Visual())->LL_BoneID(*sound.m_bone_name));
	l_tMatrix							= l_tBoneInstance.mTransform;
	l_tMatrix.mulA						(object->XFORM());
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
