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
    CAT.table		= 0;
    CAT.size		= 0;
}

CSoundRender_Source::~CSoundRender_Source	()
{
	unload		();
	xr_delete	(ovf);
}

bool ov_error(int res)
{
    switch (res){
    case 0:				return false;
// info
    case OV_HOLE:		Msg("Vorbisfile encoutered missing or corrupt data in the bitstream. Recovery is normally automatic and this return code is for informational purposes only."); return true;
    case OV_EBADLINK:	Msg("The given link exists in the Vorbis data stream, but is not decipherable due to garbacge or corruption."); return true;
// error
    case OV_FALSE: 		Msg("Not true, or no data available"); return false;
    case OV_EREAD:		Msg("Read error while fetching compressed data for decode"); return false;
    case OV_EFAULT:		Msg("Internal inconsistency in decode state. Continuing is likely not possible."); return false;
    case OV_EIMPL:		Msg("Feature not implemented"); return false; 
    case OV_EINVAL:		Msg("Either an invalid argument, or incompletely initialized argument passed to libvorbisfile call"); return false;
    case OV_ENOTVORBIS:	Msg("The given file/data was not recognized as Ogg Vorbis data."); return false;
    case OV_EBADHEADER:	Msg("The file/data is apparently an Ogg Vorbis stream, but contains a corrupted or undecipherable header."); return false;
    case OV_EVERSION:	Msg("The bitstream format revision of the given stream is not supported."); return false;
    case OV_ENOSEEK:	Msg("The given stream is not seekable"); return false;
    }
    return false;
}

void CSoundRender_Source::i_decompress_fr(char* _dest, u32 left)
{
//*
	float **pcm; 
    int val;
    long channels		= ov_info(ovf,-1)->channels;
    long bytespersample	= 2 * channels;
	int					dummy;
    left				/= bytespersample;
    short* buffer  		= (short*)_dest;
	while (left){
		int samples		= ov_read_float	(ovf,&pcm,left,&dummy);
        if (samples>0){
			for(int i=0;i<channels;i++) {
            	float *src=pcm[i];
                short *dest=((short *)buffer)+i;
                for(int j=0;j<samples;j++) {
                  	val=iFloor(src[j]*32768.f);
                    if(val>32767)val=32767;
                    else if(val<-32768)val=-32768;
                    *dest=short(val);
                    dest+=channels;
                }
            }
            left 	-= samples;
            buffer	+= samples;
        }else{
        	if (ov_error(samples)) continue; else break;
        };
	}
}
void CSoundRender_Source::i_decompress_hr(char* _dest, u32 left)
{
//*
	float **pcm; 
    int val;
    long channels		= ov_info(ovf,-1)->channels;
    long bytespersample	= 2 * channels;
	int					dummy;
    left				/= bytespersample;	left*=2;
    short* buffer  		= (short*)_dest;
	while (left){
		int samples		= ov_read_float	(ovf,&pcm,left,&dummy);
        if (samples>0){
			for(int i=0;i<channels;i++) {
            	float *src=pcm[i];
                short *dest=((short *)buffer)+i;
                for(int j=0;j<samples/2;j++) {
                	float val0 = src[j*2];
                	float val1 = src[j*2+1];
                	val=iFloor((val0+val1)*0.5f*32768.f);
                    if(val>32767)val=32767;
                    else if(val<-32768)val=-32768;
                    *dest=short(val);
                    dest+=channels;
                }
            }
            left 	-= samples;
            buffer	+= samples/2;
        }else{
        	if (ov_error(samples)) continue; else break;
        };
	}
}

void CSoundRender_Source::decompress		(u32 line)
{
	// decompression of one cache-line
	u32		line_size	= SoundRender->cache.get_linesize();
	char*	dest		= (char*)	SoundRender->cache.get_dataptr	(CAT,line);
	u32		buf_offs	= (psSoundFreq==sf_22K)?(line*line_size):(line*line_size)/2;
	u32		left_file	= dwBytesTotal - buf_offs;
	u32		left		= _min	(left_file,line_size);
    // seek
	u32	cur_pos			= u32	(ov_pcm_tell(ovf));
	if (cur_pos!=buf_offs){
        ov_pcm_seek		(ovf,buf_offs);
    }
    // decompress
    if (psSoundFreq==sf_22K)	i_decompress_hr(dest,left);
    else						i_decompress_fr(dest,left);
}

/*/

	int					dummy;
	while (left){
		int ret			= ov_read	(ovf,_dest,left,0,2,1,&dummy);
//	    Msg	("Read: %d, Pos: %d",ret,(int)ov_pcm_tell(ovf));
        if (ret==0){
//        	Log("EOF",left);
        	break;                         
        }else if (ret>0){
            left		-= ret;
            _dest		+= ret;
		}else{
#ifdef DEBUG        
			switch (ret){
// info
			case OV_HOLE:		Msg("Vorbisfile encoutered missing or corrupt data in the bitstream. Recovery is normally automatic and this return code is for informational purposes only."); continue;
			case OV_EBADLINK:	Msg("The given link exists in the Vorbis data stream, but is not decipherable due to garbacge or corruption."); continue;
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
            Memory.mem_fill		(_dest,0,left);
            left		= 0;
		}
	}
//*/

