#ifndef __XR_CDAUDIO_H__
#define __XR_CDAUDIO_H__

typedef enum CDA_STATE{
	CDA_STATE_PLAY,
	CDA_STATE_STOP,
	CDA_STATE_PAUSE,
	CDA_STATE_OPEN,
	CDA_STATE_NOTREADY
};

class ENGINE_API CCDA
{
	char					retStr[64];
	UINT					retLen;
	MCIERROR				err;

	float					fSaveCDVol;

	HMIXER							hMixer;
	MIXERCONTROLDETAILS				cd_detail;
    MIXERCONTROLDETAILS_UNSIGNED	cd_volume;
	BOOL							bVolume;

	DWORD					dwCurTrack;
	BOOL					bWorking;
	BOOL					bPaused;

	int 					lKeepTime;
	int 					lTotalTime;

	CDA_STATE				GetState( );
public:
							CCDA				( HMIXER _hMixer );
							~CCDA			( );

	float					GetVolume				( );
	void					SetVolume				( float vol );

	void					Open					( );
	void					Close					( );
	void					SetTrack				( int track );
	void					Play					( );
	void					Stop					( );
	void					Pause					( );

	void					OnMove					( );
};

#endif //__XR_CDAudio_H__
