#ifndef __XR_STREAM_SOUND_H__
#define __XR_STREAM_SOUND_H__

#include <msacm.h>

// refs
class ENGINE_API CInifile;
class ENGINE_API CStream;

class ENGINE_API CSoundStream
{
protected:
	struct sxr_riff{
		char  id[4];  	// identifier string = "RIFF"
		DWORD len;    	// remaining length after this header
		char  wave_id[4];// "WAVE"
	};
	
	struct sxr_hdr{
		char  id[4];	// identifier, e.g. "fmt " or "data"
		DWORD len; 		// remaining chunk length after header
	};
	
private:
	friend class			CMusicStream;
	string					fName;

	float 					fVolume;
	float 					fRealVolume;
	float 					fBaseVolume;

	BOOL					bMustLoop;
	int 					iLoopCountRested;

	BOOL					bNeedUpdate;
	BOOL					bMustPlay;

	DWORD					dwStatus;
	BOOL					isPause;

    LPDIRECTSOUNDBUFFER     pBuffer;

// ADPCM
	HACMSTREAM				hAcmStream;
	ACMSTREAMHEADER			stream;
	WAVEFORMATEX*			pwfx;
	WAVEFORMATEX*			psrc;
	DWORD					dwFMT_Size;
	DWORD					dwSrcBufSize;
	DWORD					dwTotalSize;
	unsigned char			*WaveSource,*WaveDest;

    DWORD					writepos;
	BOOL					isPresentData; // признак окончания буфера
	DWORD					dwDecPos;
	int					    hf,DataPos;

private:
//-----------------------------------------------------
	BOOL					Decompress				(unsigned char *dest);
	void					AppWriteDataToBuffer	(DWORD dwOffset,			// our own write cursor
													 LPBYTE lpbSoundData,		// start of our data
													 DWORD dwSoundBytes);		// size of block to copy

	void					LoadADPCM				( );

	void					Load					( CInifile* ini, LPCSTR section );
	void					Load					( LPCSTR _fName );

public:
							CSoundStream			( );
							~CSoundStream			( );

	void					Play					( BOOL loop = false, int loop_cnt = 0 );
	void					Stop					( );
	void					Pause					( );

	BOOL					isPlaying				(void)			{ return (dwStatus&DSBSTATUS_PLAYING)||bMustPlay; }
	void					Commit					( );
	void					SetVolume				( float vol )	{ fVolume = vol; bNeedUpdate = true; }
	float					GetVolume				( )				{ return fVolume; }
	void					Restore					( );
	void					Update					( );

	void					OnMove					( );
};

#endif //__XR_STREAM_SOUND_H__
