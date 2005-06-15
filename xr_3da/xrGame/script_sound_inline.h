////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_inline.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	u32	CScriptSound::Length				()
{
	VERIFY					(m_sound.handle);
	return					(m_sound.handle->length_ms());
}

IC	void CScriptSound::Play					(CScriptGameObject *object)
{
	Play					(object,0.f,0);
}

IC	void CScriptSound::Play					(CScriptGameObject *object, float delay)
{
	Play					(object,delay,0);
}
		
IC	void CScriptSound::PlayUnlimited		(CScriptGameObject *object)
{
	PlayUnlimited			(object,0.f,0);
}

IC	void CScriptSound::PlayUnlimited		(CScriptGameObject *object, float delay)
{
	PlayUnlimited			(object,delay,0);
}
		void				PlayUnlimited	(CScriptGameObject *object, float delay, int flags);

IC	void CScriptSound::PlayAtPos			(CScriptGameObject *object, const Fvector &position)
{
	PlayAtPos				(object,position,0.f,0);
}

IC	void CScriptSound::PlayAtPos			(CScriptGameObject *object, const Fvector &position, float delay)
{
	PlayAtPos				(object,position,delay,0);
}

IC	void CScriptSound::PlayAtPosUnlimited	(CScriptGameObject *object, const Fvector &position)
{
	PlayAtPosUnlimited		(object,position,0.f,0);
}

IC	void CScriptSound::PlayAtPosUnlimited	(CScriptGameObject *object, const Fvector &position, float delay)
{
	PlayAtPosUnlimited		(object,position,delay,0);
}

IC	void CScriptSound::SetMinDistance		(const float fMinDistance)
{
	VERIFY				(m_sound.handle);
	m_sound.set_range(fMinDistance,GetMaxDistance());
}

IC	void CScriptSound::SetMaxDistance		(const float fMaxDistance)
{
	VERIFY				(m_sound.handle);
	m_sound.set_range(GetMinDistance(),fMaxDistance);
}

IC	const float	CScriptSound::GetFrequency	() const
{
	VERIFY				(m_sound.handle);
	return				(m_sound.get_params()->freq);
}

IC	const float CScriptSound::GetMinDistance() const
{
	VERIFY				(m_sound.handle);
	return				(m_sound.get_params()->min_distance);
}

IC	const float CScriptSound::GetMaxDistance() const
{
	VERIFY				(m_sound.handle);
	return				(m_sound.get_params()->max_distance);
}

IC	const float	CScriptSound::GetVolume		() const
{
	VERIFY				(m_sound.handle);
	return				(m_sound.get_params()->volume);
}

IC	bool CScriptSound::IsPlaying			() const
{
	VERIFY				(m_sound.handle);
	return				(!!m_sound.feedback);
}

IC	void CScriptSound::Stop					()
{
	VERIFY				(m_sound.handle);
	m_sound.stop		();
}

IC	void CScriptSound::StopDeffered			()
{
	VERIFY				(m_sound.handle);
	m_sound.stop_deffered();
}

IC	void CScriptSound::SetPosition			(const Fvector &position)
{
	VERIFY				(m_sound.handle);
	m_sound.set_position(position);
}

IC	void CScriptSound::SetFrequency			(float frequency)
{
	VERIFY				(m_sound.handle);
	m_sound.set_frequency(frequency);
}

IC	void CScriptSound::SetVolume			(float volume)
{
	VERIFY				(m_sound.handle);
	m_sound.set_volume	(volume);
}

IC	const CSound_params *CScriptSound::GetParams	()
{
	VERIFY				(m_sound.handle);
	return				(m_sound.get_params());
}

IC	void CScriptSound::SetParams			(CSound_params *sound_params)
{
	VERIFY				(m_sound.handle);
	m_sound.set_params	(sound_params);
}
