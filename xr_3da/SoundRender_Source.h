#pragma once

#include "soundrender.h"

class CSoundRender_Source
{
public:
	LPCSTR					fname;
	void*					wave;
	BOOL					_3D;
	u32						dwTimeTotal;			// всего
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
