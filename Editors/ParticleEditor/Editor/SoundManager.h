//---------------------------------------------------------------------------
#ifndef SoundManagerH
#define SoundManagerH

#include "soundrender_environment.h"
class CSoundManager{
	bool		bNeedRefreshEnvGeom;
    void		RealRefreshEnvGeometry();
public:
				CSoundManager		(){bNeedRefreshEnvGeom = false;}
				~CSoundManager		(){;}
	// texture routines
    int			GetSounds			(FS_QueryMap& files);
    int			GetSoundEnvs		(AStringVec& items);

    bool		OnCreate			();
    void		OnDestroy			();

    void 		OnFrame				();

    void		RefreshEnvLibrary	();
    void		RefreshEnvGeometry	(){bNeedRefreshEnvGeom = true;}

    bool 		MakeEnvGeometry		(CMemoryWriter& F, bool bErrMsg=false);

    bool		Validate			();

    void		MuteSounds			(BOOL bVal);
};

extern CSoundManager SndLib;
//---------------------------------------------------------------------------
#endif
