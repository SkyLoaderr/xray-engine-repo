//---------------------------------------------------------------------------
#ifndef SoundManagerH
#define SoundManagerH

class CSoundManager{
public:
				CSoundManager		(){;}
				~CSoundManager		(){;}
	// texture routines
    int			GetSounds			(FS_QueryMap& files);

    bool		OnCreate			();
    void		OnDestroy			();

    void 		OnFrame				();
};

extern CSoundManager SndLib;
//---------------------------------------------------------------------------
#endif
