//---------------------------------------------------------------------------
#ifndef EScenePSToolsH
#define EScenePSToolsH

#include "ESceneCustomOTools.h"

class EScenePSTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						EScenePSTools			():ESceneCustomOTools(OBJCLASS_PS){;}
	// definition
    IC LPCSTR			ClassName				(){return "ps";}
    IC LPCSTR			ClassDesc				(){return "Particle System";}
    IC int				RenderPriority			(){return 30;}
};
//---------------------------------------------------------------------------
#endif
