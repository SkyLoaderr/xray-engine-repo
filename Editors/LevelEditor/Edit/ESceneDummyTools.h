//---------------------------------------------------------------------------
#ifndef ESceneDummyToolsH
#define ESceneDummyToolsH

#include "ESceneCustomOTools.h"

class ESceneDummyTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneDummyTools		():ESceneCustomOTools(OBJCLASS_DUMMY){;}
                        
    virtual void		Clear					(bool bSpecific=false){inherited::Clear(bSpecific);}
	// definition
    IC LPCSTR			ClassName				(){THROW; }
    IC LPCSTR			ClassDesc				(){THROW; }
    IC int				RenderPriority			(){return -1; }
};
//---------------------------------------------------------------------------
#endif
