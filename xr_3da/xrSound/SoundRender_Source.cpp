#include "stdafx.h"
#pragma hdrstop

#include "soundrender_core.h"
#include "soundrender_source.h"

CSoundRender_Source::CSoundRender_Source	()
{
	m_fMinDist		= DS3D_DEFAULTMINDISTANCE;
	m_fMaxDist		= 300.f;
	m_fVolume		= 1.f;
	m_uGameType		= 0;
	fname			= 0;
	wave			= 0;
    ovf				= xr_new<OggVorbis_File>();
}

CSoundRender_Source::~CSoundRender_Source	()
{
	xr_delete	(ovf);
	unload		();
}

void CSoundRender_Source::decompress		(u32 line)
{
	// decompression of one cache-line
	u32		line_size	= SoundRender.cache.get_linesize();
	char*	dest		= (char*)	SoundRender.cache.get_dataptr	(CAT,line);
	u32		seek_offs	= (psSoundFreq==sf_22K)?(line*line_size):(line*line_size)/2;
	u32		left_file	= dwBytesTotal - seek_offs;
	u32		left		= _min	(left_file,line_size);

	int					dummy;
	ov_pcm_seek			(ovf,seek_offs);
	while (left)
	{
		int ret		= ov_read	(ovf,dest,left,0,2,1,&dummy);
		if (ret==0)	break;
		if (ret<0)	continue;
		left		-= ret;
		dest		+= ret;
	}
}
