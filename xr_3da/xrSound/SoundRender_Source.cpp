#include "stdafx.h"
#pragma hdrstop

#include "soundrender_core.h"
#include "soundrender_source.h"

CSoundRender_Source::CSoundRender_Source	()
{
	m_fMinDist		= 1.f;
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
	u32		line_size	= SoundRender->cache.get_linesize();
	char*	dest		= (char*)	SoundRender->cache.get_dataptr	(CAT,line);
	u32		seek_offs	= (psSoundFreq==sf_22K)?(line*line_size):(line*line_size)/2;
	u32		left_file	= dwBytesTotal - seek_offs;
	u32		left		= _min	(left_file,line_size);

	int					dummy;
	ov_pcm_seek			(ovf,seek_offs);
	while (left)
	{
		int ret			= ov_read	(ovf,dest,left,0,2,1,&dummy);
        if (ret==0){
//        	Log("EOF",left);
        	break;                         
        }else if (ret>0){
            left		-= ret;
            dest		+= ret;
		}
        else{
#ifdef DEBUG        
			switch (ret){
// info
			case OV_HOLE:		Msg("Vorbisfile encoutered missing or corrupt data in the bitstream. Recovery is normally automatic and this return code is for informational purposes only."); continue; break;
			case OV_EBADLINK:	Msg("The given link exists in the Vorbis data stream, but is not decipherable due to garbacge or corruption."); continue; break;
// error
			case OV_FALSE: 		Msg("Not true, or no data available"); break;
			case OV_EREAD:		Msg("Read error while fetching compressed data for decode"); break;
			case OV_EFAULT:		Msg("Internal inconsistency in decode state. Continuing is likely not possible."); break;
			case OV_EIMPL:		Msg("Feature not implemented"); break; 
			case OV_EINVAL:		Msg("Either an invalid argument, or incompletely initialized argument passed to libvorbisfile call"); break;
			case OV_ENOTVORBIS:	Msg("The given file/data was not recognized as Ogg Vorbis data."); break;
			case OV_EBADHEADER:	Msg("The file/data is apparently an Ogg Vorbis stream, but contains a corrupted or undecipherable header."); break;
			case OV_EVERSION:	Msg("The bitstream format revision of the given stream is not supported."); break;
			case OV_ENOSEEK:	Msg("The given stream is not seekable"); break;
			}
#endif
            Memory.mem_fill		(dest,0,left);
            left		= 0;
		}
	}
}
