//////////////////////////////////////////////////////////////////////
// HudSound.h:		структура для работы со звуками применяемыми в 
//					HUD-объектах (обычные звуки, но с доп. параметрами)
//////////////////////////////////////////////////////////////////////

#pragma once


struct HUD_SOUND
{
	HUD_SOUND() {enable = false;}


	////////////////////////////////////
	// работа со звуками
	/////////////////////////////////////
	static void		LoadSound	(LPCSTR section, LPCSTR line,
		ref_sound& hud_snd, BOOL _3D,
		int type = st_SourceType,
		float* volume = NULL,
		float* delay = NULL);

	static void		LoadSound	(LPCSTR section, LPCSTR line,
		HUD_SOUND& hud_snd, BOOL _3D,
		int type = st_SourceType);

	static void		DestroySound (HUD_SOUND& hud_snd);

	static void		PlaySound	(HUD_SOUND& snd,
								const Fvector& position,
								const CObject* parent,
								bool hud_mode,
								bool looped = false);

	static void		StopSound	(HUD_SOUND& snd);


	void set_position(const Fvector& pos) {snd.set_position(pos);}

	ref_sound snd;
	float delay;	//задержка перед проигрыванием
	float volume;	//громкость
	
	bool  enable;	//присутствует включен ли звук
};

