#include "stdafx.h"
#pragma hdrstop

#include "soundrender_source.h"

CSoundRender_Source::CSoundRender_Source	()
{
	fname	= 0;
	wave	= 0;
}

CSoundRender_Source::~CSoundRender_Source	()
{
	xr_free	(wave);
	xr_free	(fname);
}
