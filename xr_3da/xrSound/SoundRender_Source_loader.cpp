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
	case SEEK_END: ((IReader*)datasource)->seek(((IReader*)datasource)->length()+offset); break;
	}
	return 0; 
}
size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{ 
	IReader* F = (IReader*)datasource; 
	size_t exist_block	= _max(0,iFloor(F->elapsed()/size));
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

int ov_seek_func1(void *datasource, s64 offset, int whence){ return fseek((FILE*)datasource, offset, whence); }
size_t ov_read_func1(void *ptr, size_t size, size_t nmemb, void *datasource){ return fread(ptr, size, nmemb, (FILE*)datasource);}
int ov_close_func1(void *datasource){return fclose((FILE*)datasource);}
long ov_tell_func1(void *datasource){return ftell((FILE*)datasource);}

void CSoundRender_Source::LoadWave	(LPCSTR pName, BOOL b3D)
{
	fname					= pName;
	// Load file into memory and parse WAV-format
	wave					= FS.r_open(pName); VERIFY(wave);
//	ov_callbacks ovc		= {ov_read_func,ov_seek_func,ov_close_func,ov_tell_func};
//	ov_open_callbacks		(wave,&ovf,NULL,0,ovc);

	ov_callbacks ovc		= {ov_read_func1,ov_seek_func1,ov_close_func1,ov_tell_func1};
	FILE* F = fopen(pName,"rb");
	ov_open_callbacks		(F,&ovf,NULL,0,ovc);

	vorbis_info* ovi		= ov_info(&ovf,-1);
	// verify
	R_ASSERT3				(b3D?ovi->channels==1:ovi->channels==2,"Invalid source num channels:",pName);
	R_ASSERT3				(ovi->rate==44100,"Invalid source rate:",pName);
	ov_halfrate				(&ovf,psSoundFreq==sf_22K);

	WAVEFORMATEX			wfxdest;
	SoundRender.pBuffer->GetFormat(&wfxdest,sizeof(wfxdest),0);
	wfxdest.nChannels		= u16(ovi->channels); 
	wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
	wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;

	s64 pcm_total			= ov_pcm_total(&ovf,-1)-1; 
	if (psSoundFreq==sf_22K) pcm_total/=2;
	dwBytesTotal			= u32(pcm_total*wfxdest.nBlockAlign); 
	dwBytesPerMS			= wfxdest.nAvgBytesPerSec/1000;
//	dwBytesPerSec			= wfxdest.nAvgBytesPerSec;
	dwTimeTotal				= sdef_source_footer + (dwBytesTotal*1000)/wfxdest.nAvgBytesPerSec;
}

void CSoundRender_Source::Load		(LPCSTR name,	BOOL b3D)
{
	string256			fn,N;
	strcpy				(N,name);
	strlwr				(N);
	if (strext(N))		*strext(N) = 0;

	fname				= N;
	_3D					= b3D;

	strconcat			(fn,N,".ogg");
	if (!FS.exist("$level$",fn))	FS.update_path	(fn,"$game_sounds$",fn);

	LoadWave			(fn,_3D);
	R_ASSERT			(wave);
	SoundRender.cache.cat_create			( CAT, dwBytesTotal );

	if (dwTimeTotal<100)
	{
		Msg	("! WARNING: Invalid wave length (must be at least 100ms), file: %s",fn);
	}
}
