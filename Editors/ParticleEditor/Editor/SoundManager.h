//---------------------------------------------------------------------------
#ifndef ESoundManagerH
#define ESoundManagerH

class CSoundManager{
public:
				CSoundManager		(){;}
				~CSoundManager		(){;}
	// texture routines
    int			GetSounds			(FileMap& files);
};

extern CSoundManager SoundManager;
//---------------------------------------------------------------------------
#endif
