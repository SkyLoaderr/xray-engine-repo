#include "stdafx.h"
#pragma hdrstop

#include <msacm.h>

#include "soundrender_core.h"
#include "soundrender_source.h"

//	SEEK_SET	0	File beginning
//	SEEK_CUR	1	Current file pointer position
//	SEEK_END	2	End-of-file
int ov_seek_func(void *datasource, s64 offset, int whence)	
{
	switch (whence){
	case SEEK_SET: ((IReader*)datasource)->seek((int)offset);	 break;
	case SEEK_CUR: ((IReader*)datasource)->advance((int)offset); break;
	case SEEK_END: ((IReader*)datasource)->seek(offset + ((IReader*)datasource)->length()); break;
	}
	return 0; 
}
size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{ 
	IReader* F			= (IReader*)datasource; 
	size_t exist_block	= _max(0ul,iFloor(F->elapsed()/size));
	size_t read_block	= _min(exist_block,nmemb);
	F->r				(ptr,read_block*size);	
	return read_block;
}
int ov_close_func(void *datasource)									
{	
	return 0; 
}
long ov_tell_func(void *datasource)									
{	
	return ((IReader*)datasource)->tell(); 
}

void CSoundRender_Source::LoadWave	(LPCSTR pName, BOOL b3D)
{
	// Load file into memory and parse WAV-format
	wave					= FS.r_open(pName); 
	R_ASSERT2				(wave,pName);

	ov_callbacks ovc		= {ov_read_func,ov_seek_func,ov_close_func,ov_tell_func};
	ov_open_callbacks		(wave,ovf,NULL,0,ovc);

	vorbis_info* ovi		= ov_info(ovf,-1);
	// verify
	R_ASSERT3				(b3D?ovi->channels==1:ovi->channels==2,"Invalid source num channels:",pName);
	R_ASSERT3				(ovi->rate==44100,"Invalid source rate:",pName);
	ov_halfrate				(ovf,psSoundFreq==sf_22K);

	WAVEFORMATEX wfxdest 	= SoundRender->wfm;
	wfxdest.nChannels		= u16(ovi->channels); 
	wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
	wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;

	s64 pcm_total			= ov_pcm_total(ovf,-1)-1; 
	if (psSoundFreq==sf_22K) pcm_total/=2;
	dwBytesTotal			= u32(pcm_total*wfxdest.nBlockAlign); 
	dwBytesPerMS			= wfxdest.nAvgBytesPerSec/1000;
//	dwBytesPerSec			= wfxdest.nAvgBytesPerSec;
	dwTimeTotal				= u32 ( sdef_source_footer + u64( (u64(dwBytesTotal)*u64(1000))/u64(wfxdest.nAvgBytesPerSec) ) );

	vorbis_comment*	ovm		= ov_comment(ovf,-1);
	if (ovm->comments){
		IReader F			(ovm->user_comments[0],ovm->comment_lengths[0]);
		u32 vers			= F.r_u32	();
        if (vers==0x0001){
			m_fMinDist		= F.r_float	();
			m_fMaxDist		= F.r_float	();
	        m_fVolume		= 1.f;
			m_uGameType		= F.r_u32	();
        }else if (vers==OGG_COMMENT_VERSION){
			m_fMinDist		= F.r_float	();
			m_fMaxDist		= F.r_float	();
            m_fVolume		= F.r_float	();
			m_uGameType		= F.r_u32	();
		}else{
			Log				("! Invalid ogg-comment version, file: ",pName);
		}
	}else{
		Log				("! Missing ogg-comment, file: ",pName);
	}
}

void CSoundRender_Source::load(LPCSTR name,	BOOL b3D)
{
	string256			fn,N;
	strcpy				(N,name);
	strlwr				(N);
	if (strext(N))		*strext(N) = 0;

	fname				= N;
	_3D					= b3D;

	strconcat			(fn,N,".ogg");
	if (!FS.exist("$level$",fn))	FS.update_path	(fn,"$game_sounds$",fn);

#ifdef _EDITOR
	if (!FS.exist(fn)){ 
		FS.update_path	(fn,"$game_sounds$","$no_sound.ogg");
    }
#endif
	LoadWave			(fn,_3D);	R_ASSERT(wave);
	SoundRender->cache.cat_create	(CAT, dwBytesTotal);

	if (dwTimeTotal<100)					{
		Msg	("! WARNING: Invalid wave length (must be at least 100ms), file: %s",fn);
	}
}

void CSoundRender_Source::unload()
{
	ov_clear						(ovf);
	FS.r_close						(wave);
	SoundRender->cache.cat_destroy	(CAT);
    dwTimeTotal						= 0;
    dwBytesTotal					= 0;
    dwBytesPerMS					= 0;
}

