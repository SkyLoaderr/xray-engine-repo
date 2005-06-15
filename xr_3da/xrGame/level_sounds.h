//---------------------------------------------------------------------------
#ifndef LevelSoundsH
#define LevelSoundsH
#pragma once

struct SStaticSound
{
	ref_sound		m_Source;
	Ivector2		m_ActiveTime;
	Ivector2		m_PlayTime;
	Ivector2		m_PauseTime;
	u32				m_NextTime;
	u32				m_StopTime;
	Fvector			m_Position;
	float			m_Volume;
	float			m_Freq;
public:
	void 			Load			(IReader& F);
	void 			Update			(u32 gt, u32 rt);
};

class CStaticSoundManager
{
	DEFINE_VECTOR(SStaticSound,StaticSoundsVec,StaticSoundsVecIt);
	StaticSoundsVec	m_StaticSounds;
//	u32				m_UpdateIndex;
public:
	void			Load			();
	void			Unload			();
	void			Update			();
};

#endif