//---------------------------------------------------------------------------
#ifndef ESceneGlowToolsH
#define ESceneGlowToolsH

#include "ESceneCustomOTools.h"

class ESceneGlowTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
    friend class 		CGlow;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
	enum{
    	flTestVisibility= (1<<31),
    };
public:
						ESceneGlowTools		():ESceneCustomOTools(OBJCLASS_GLOW){;}
	// definition
    IC LPCSTR			ClassName				(){return "glow";}
    IC LPCSTR			ClassDesc				(){return "Glow";}
    IC int				RenderPriority			(){return 20;}

    void 				FillProp				(LPCSTR pref, PropItemVec& items);
};
//---------------------------------------------------------------------------
#endif
