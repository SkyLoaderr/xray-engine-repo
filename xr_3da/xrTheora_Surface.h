#ifndef xrTheora_SurfaceH
#define xrTheora_SurfaceH
#pragma once

#ifdef SDL_OUTPUT
#include <SDL.h>
#pragma comment	(lib,"SDL.lib")
#endif

// refs
class ctheora_stream;

class ctheora_surface{
#ifdef SDL_OUTPUT
	// SDL Video playback structures 
	SDL_Surface*		sdl_screen;
	SDL_Overlay*		sdl_yuv_overlay;
	SDL_Rect			sdl_rect;
#endif
	ctheora_stream*		m_rgb;
	ctheora_stream*		m_alpha;

	u32					tm_play;
	u32					tm_total;
	bool				ready;
public:
	bool				playing;
	bool				looped;
protected:
	void				invalidate			();
	void				reset				();

#ifdef SDL_OUTPUT
	void				open_sdl_video		();
	void				write_sdl_video		();
#endif
public:
						ctheora_surface		();
	virtual				~ctheora_surface	();

	bool				valid				();
	bool				load				(const char* fname);

	void				update				(u32 dt);

	void				play				(bool _looped)		{playing=true;looped=_looped;}
	void				pause				(bool _pause)		{playing=!_pause;}
	void				stop				()					{playing=false;reset();}
};

#endif //xrTheora_SurfaceH
