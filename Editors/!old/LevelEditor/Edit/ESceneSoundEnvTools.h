//---------------------------------------------------------------------------
#ifndef ESceneSoundEnvToolsH
#define ESceneSoundEnvToolsH

#include "ESceneCustomOTools.h"

class ESceneSoundEnvTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneSoundEnvTools		():ESceneCustomOTools(OBJCLASS_SOUND_ENV){;}
	// definition
    IC LPCSTR			ClassName				(){return "sound_env";}
    IC LPCSTR			ClassDesc				(){return "Sound Environment";}
    IC int				RenderPriority			(){return 10;}
};
//---------------------------------------------------------------------------
#endif
