//---------------------------------------------------------------------------
#ifndef ESceneSectorToolsH
#define ESceneSectorToolsH

#include "ESceneCustomOTools.h"

class ESceneSectorTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
    friend class 		CSector;
protected:
	enum{
    	flDrawSolid		= (1<<31),
    };
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneSectorTools		():ESceneCustomOTools(OBJCLASS_SECTOR){;}
	// definition
    IC LPCSTR			ClassName				(){return "sector";}
    IC LPCSTR			ClassDesc				(){return "Sector";}
    IC int				RenderPriority			(){return 20;}

    virtual void 		OnObjectRemove			(CCustomObject* O);

	virtual void 		FillProp				(LPCSTR pref, PropItemVec& items);
};
//---------------------------------------------------------------------------
#endif
