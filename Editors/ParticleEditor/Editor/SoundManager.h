//---------------------------------------------------------------------------
#ifndef SoundManagerH
#define SoundManagerH

#include "soundrender_environment.h"
class CSoundManager{
public:
				CSoundManager		(){;}
				~CSoundManager		(){;}
	// texture routines
    int			GetSounds			(FS_QueryMap& files);
    int			GetSoundEnvs		(AStringVec& items);

    bool		OnCreate			();
    void		OnDestroy			();

    void 		OnFrame				();

    void		RefreshEnvLibrary	();
    void		SetEnvGeometry		();
};

extern CSoundManager SndLib;
//---------------------------------------------------------------------------
#endif
