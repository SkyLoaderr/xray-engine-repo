//---------------------------------------------------------------------------
#ifndef SoundManagerH
#define SoundManagerH

class CSoundManager{
public:
				CSoundManager		(){;}
				~CSoundManager		(){;}
	// texture routines
    int			GetSounds			(FS_QueryMap& files);
};

extern CSoundManager SoundManager;
//---------------------------------------------------------------------------
#endif
