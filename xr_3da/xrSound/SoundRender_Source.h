#ifndef SoundRender_SourceH
#define SoundRender_SourceH
#pragma once

#include "soundrender.h"

class XRSOUND_EDITOR_API 	CSoundRender_Source	: public CSound_source
{
public:
	ref_str					fname;
	void*					wave;
	BOOL					_3D;
	u32						dwTimeTotal;			// �����
	u32						dwBytesTotal;
	u32						dwBytesPerMS;
private:
	void					LoadWaveAs3D			(LPCSTR name);
	void					LoadWaveAs2D			(LPCSTR name);
public:
	CSoundRender_Source		();
	~CSoundRender_Source	();

	void					Load					(LPCSTR name,	BOOL b3D);
};
#endif