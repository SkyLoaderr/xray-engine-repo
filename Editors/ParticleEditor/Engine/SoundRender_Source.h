#ifndef SoundRender_SourceH
#define SoundRender_SourceH
#pragma once

#include "soundrender.h"
#include "soundrender_cache.h"

class XRSOUND_EDITOR_API 	CSoundRender_Source	: public CSound_source
{
public:
	OggVorbis_File			ovf;
	ref_str					fname;
	IReader*				wave;					// real source
	cache_cat				CAT;
	BOOL					_3D;
	u32						dwTimeTotal;			// всего
	u32						dwBytesTotal;
//	u32						dwBytesPerSec;
	u32						dwBytesPerMS;
private:
	void					LoadWave 				(LPCSTR name, BOOL b3D);
public:
	CSoundRender_Source		();
	~CSoundRender_Source	();

	void					load					(LPCSTR name,	BOOL b3D);
    void					unload					();
	void					decompress				(u32	line);
};
#endif