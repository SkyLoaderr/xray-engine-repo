//---------------------------------------------------------------------------
#ifndef ESceneSectorToolsH
#define ESceneSectorToolsH

#include "ESceneCustomOTools.h"

class ESceneSectorTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneSectorTools		():ESceneCustomOTools(OBJCLASS_SECTOR){;}
	// definition
    IC LPCSTR			ClassName				(){return "sector";}
    IC LPCSTR			ClassDesc				(){return "Sector";}
    IC int				RenderPriority			(){return 20;}
};
//---------------------------------------------------------------------------
#endif
