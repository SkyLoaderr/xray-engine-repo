//---------------------------------------------------------------------------
#ifndef ESceneSoundSrcToolsH
#define ESceneSoundSrcToolsH

#include "ESceneCustomOTools.h"

class ESceneSoundSrcTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneSoundSrcTools		():ESceneCustomOTools(OBJCLASS_SOUND_SRC){;}
	// definition
    IC LPCSTR			ClassName				(){return "sound_src";}
    IC LPCSTR			ClassDesc				(){return "Sound Source";}
    IC int				RenderPriority			(){return 10;}
};
//---------------------------------------------------------------------------
#endif
