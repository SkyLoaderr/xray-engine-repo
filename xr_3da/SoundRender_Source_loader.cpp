#include "stdafx.h"
#include <msacm.h>

#include "soundrender_core.h"
#include "soundrender_source.h"

void* ParseWave		(IReader *data, LPWAVEFORMATEX &wfx, u32 &len)
{
	u32	dwRiff		= data->r_u32();
	u32	dwLength	= data->r_u32();
	u32	dwType		= data->r_u32();
	u32	dwPos;
	void	*ptr		= NULL;
	wfx					= NULL;

	if (dwRiff != mmioFOURCC('R', 'I', 'F', 'F')) return NULL;
	if (dwType != mmioFOURCC('W', 'A', 'V', 'E')) return NULL;

	while (!data->eof()) {
		dwType		= data->r_u32();
		dwLength	= data->r_u32();
		dwPos		= data->tell();

		switch (dwType){
		case mmioFOURCC('f', 'm', 't', ' '):
			if (!wfx) {
				wfx = LPWAVEFORMATEX (xr_malloc(dwLength));
				data->r(wfx,dwLength);
			}
			break;
		case mmioFOURCC('d', 'a', 't', 'a'):
			if (!ptr) {
				ptr = data->pointer();
				len = dwLength;
			}
			break;
		}
		if (wfx && ptr) return ptr;
		data->seek(dwPos+dwLength);
	}
	return NULL;
}

u32 Freq2Size(u32 freq) {
	switch (freq) {
	case 11025:	return 1;
	case 22050: return 2;
	case 44100: return 4;
	default:	return 0;
	}
}

// Convert data to SRC bits -MONO- PB Hz
// MONO - because sound must be 3D source - elsewhere performance penalty occurs
void *ConvertWave(WAVEFORMATEX &wfx_dest, LPWAVEFORMATEX &wfx, void *data, u32 &dwLen)
{
	HACMSTREAM	hc;
	DWORD		dwNewLen;
	if (FAILED(acmStreamOpen(&hc, NULL, wfx, &wfx_dest, NULL, NULL, 0, ACM_STREAMOPENF_NONREALTIME))) return NULL;
	if (!hc) return NULL;
	if (FAILED(acmStreamSize(hc,dwLen,&dwNewLen,ACM_STREAMSIZEF_SOURCE))) return NULL;
	if (!dwNewLen) return NULL;

	void *dest		= xr_malloc(dwNewLen);
	ACMSTREAMHEADER acmhdr;
	acmhdr.cbStruct	=sizeof(acmhdr);
	acmhdr.fdwStatus=0;
	acmhdr.pbSrc=(BYTE *)data;
	acmhdr.cbSrcLength=dwLen;
	acmhdr.pbDst=(BYTE *)dest;
	acmhdr.cbDstLength=dwNewLen;

	if (FAILED(acmStreamPrepareHeader(hc,&acmhdr,0))) {
		acmStreamClose			(hc,0);
		xr_free					(dest);
		return			NULL;
	}
	if (FAILED(acmStreamConvert(hc,&acmhdr,ACM_STREAMCONVERTF_START|ACM_STREAMCONVERTF_END))) {
		acmStreamUnprepareHeader(hc,&acmhdr,0);
		acmStreamClose			(hc,0);
		xr_free					(dest);
		return			NULL;
	}
	dwLen = acmhdr.cbDstLengthUsed;
	acmStreamUnprepareHeader	(hc,&acmhdr,0);
	acmStreamClose				(hc,0);
	return dest;
}

void CSoundRender_Source::LoadWaveAs2D	(LPCSTR pName)
{
	// Load file into memory and parse WAV-format
	destructor<IReader>		data	(FS.r_open(pName));
	WAVEFORMATEX*			pFormat;
	u32						dwLen;
	void *					wavedata = ParseWave(&data(),pFormat,dwLen);
	if (!wavedata)			return;

	// Parsing OK, converting to best format
	WAVEFORMATEX			wfxdest;
	void*					converted;

	//	2411252 - Andy
	SoundRender.pBuffer->GetFormat(&wfxdest,sizeof(wfxdest),0);
	if ((pFormat->wFormatTag!=1)&&(pFormat->nSamplesPerSec!=wfxdest.nSamplesPerSec)) {
		// Firstly convert to PCM with SRC freq and Channels; BPS = as Dest
		wfxdest.nChannels		= pFormat->nChannels;
		wfxdest.nSamplesPerSec	= pFormat->nSamplesPerSec;
		wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
		wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;
		void *conv				= ConvertWave(wfxdest, pFormat, wavedata, dwLen);
		if (!conv)				{xr_free(pFormat); return; }

		// Secondly convert to best format for 2D
		Memory.mem_copy			(pFormat,&wfxdest,sizeof(wfxdest));
		SoundRender.pBuffer->GetFormat(&wfxdest,sizeof(wfxdest),0);
		wfxdest.nChannels		= pFormat->nChannels;
		wfxdest.wBitsPerSample	= pFormat->wBitsPerSample;
		wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
		wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;
		converted				= ConvertWave(wfxdest, pFormat, conv, dwLen);
		xr_free					(conv);
	} else {
		// Wave has PCM format - so only one conversion
		// Freq as in PrimaryBuf, Channels = ???, Bits as in source data if possible
		if (pFormat->wFormatTag==1)	wfxdest.wBitsPerSample	= pFormat->wBitsPerSample;
		wfxdest.nBlockAlign		= wfxdest.nChannels			* wfxdest.wBitsPerSample / 8;
		wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec	* wfxdest.nBlockAlign;
		converted				= ConvertWave(wfxdest, pFormat, wavedata, dwLen);
	}
	if (!converted)				{ xr_free(pFormat); return; }

	dwTimeTotal					= 1000 * dwLen / wfxdest.nAvgBytesPerSec;
	dwBytesPerMS				= wfxdest.nAvgBytesPerSec / 1000;
	dwBytesTotal				= dwLen;
	wave						= converted;
	xr_free						(pFormat);
}

void	CSoundRender_Source::LoadWaveAs3D(LPCSTR pName)
{
	// Load file into memory and parse WAV-format
	R_ASSERT2			(FS.exist(pName),pName);
	destructor<IReader>	data(FS.r_open(pName));
	WAVEFORMATEX*	pFormat;
	u32				dwLen;
	void *			wavedata = ParseWave	(&data(),pFormat,dwLen);
	if (!wavedata)	return;

	// Parsing OK, converting to best format
	WAVEFORMATEX			wfxdest;
	void*					converted;

	SoundRender.pBuffer->GetFormat	(&wfxdest,sizeof(wfxdest),0);
	if ((pFormat->wFormatTag!=1)||(pFormat->nChannels!=1)||(pFormat->nSamplesPerSec!=wfxdest.nSamplesPerSec))
	{
		Msg("! WARNING: Invalid wave format (must be 44KHz,16bit,mono), file: %s",pName);
	}
	if ((pFormat->wFormatTag!=1)&&(pFormat->nSamplesPerSec!=wfxdest.nSamplesPerSec)) {
		// Firstly convert to PCM with SRC freq and Channels; BPS = as Dest
		wfxdest.nChannels		= pFormat->nChannels;
		wfxdest.nSamplesPerSec	= pFormat->nSamplesPerSec;
		wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
		wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;
		void *conv				= ConvertWave(wfxdest, pFormat, wavedata, dwLen);
		if (!conv)				{xr_free(pFormat); return; }

		// Secondly convert to best format for 3D
		Memory.mem_copy			(pFormat,&wfxdest,sizeof(wfxdest));
		SoundRender.pBuffer->GetFormat(&wfxdest,sizeof(wfxdest),0);
		wfxdest.nChannels		= 1;
		wfxdest.wBitsPerSample	= pFormat->wBitsPerSample;
		wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
		wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;
		converted				= ConvertWave(wfxdest, pFormat, conv, dwLen);
		xr_free					(conv);
	} else {
		// Wave has PCM format - so only one conversion
		// Freq as in PrimaryBuf, Channels = 1, Bits as in source data if possible
		wfxdest.nChannels		= 1;
		if (pFormat->wFormatTag==1)	wfxdest.wBitsPerSample	= pFormat->wBitsPerSample;
		wfxdest.nBlockAlign		= wfxdest.nChannels * wfxdest.wBitsPerSample / 8;
		wfxdest.nAvgBytesPerSec = wfxdest.nSamplesPerSec * wfxdest.nBlockAlign;
		converted				= ConvertWave(wfxdest, pFormat, wavedata, dwLen);
	}
	if (!converted)				{ xr_free(pFormat); }

	dwTimeTotal					= 1000 * dwLen / wfxdest.nAvgBytesPerSec;
	dwBytesPerMS				= wfxdest.nAvgBytesPerSec / 1000;
	dwBytesTotal				= dwLen;
	wave						= converted;
	xr_free						(pFormat);
}

void CSoundRender_Source::Load		(LPCSTR name,	BOOL b3D)
{
	fname				= strlwr	(xr_strdup(name));

	string256			fn,N;
	strcpy				(N,name);
	strlwr				(N);
	if (strext(N))		*strext(N) = 0;

	strconcat			(fn,Path.Current,N,".wav");
	if (!FS.exist(fn))	{
		strconcat		(fn,N,".wav");
		FS.update_path	(fn,"$sounds$",fn);
	}

	if (_3D)			LoadWaveAs3D		( fn );
	else				LoadWaveAs2D		( fn );
	R_ASSERT			(wave);

	if (dwTimeTotal<250)
	{
		Msg	("! WARNING: Invalid wave length (must be at least 250ms), file: %s",fn);
	}
}
