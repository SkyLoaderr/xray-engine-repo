//---------------------------------------------------------------------------
#ifndef EScenePortalToolsH
#define EScenePortalToolsH

#include "ESceneCustomOTools.h"

class EScenePortalTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						EScenePortalTools		():ESceneCustomOTools(OBJCLASS_PORTAL){;}
	// definition
    IC LPCSTR			ClassName				(){return "portal";}
    IC LPCSTR			ClassDesc				(){return "Portal";}
    IC int				RenderPriority			(){return 20;}
};
//---------------------------------------------------------------------------
#endif
