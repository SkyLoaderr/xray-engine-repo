#include "stdafx.h"
#pragma hdrstop

#include "soundrender_core.h"
#include "soundrender_source.h"

CSoundRender_Source::CSoundRender_Source	()
{
	fname	= 0;
	wave	= 0;
}

CSoundRender_Source::~CSoundRender_Source	()
{
	FS.r_close(wave);

	SoundRender.cache.cat_destroy			(CAT);
}
