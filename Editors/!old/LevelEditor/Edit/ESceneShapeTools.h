//---------------------------------------------------------------------------
#ifndef ESceneShapeToolsH
#define ESceneShapeToolsH

#include "ESceneCustomOTools.h"

class ESceneShapeTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneShapeTools		():ESceneCustomOTools(OBJCLASS_SHAPE){;}
	// definition
    IC LPCSTR			ClassName				(){return "shape";}
    IC LPCSTR			ClassDesc				(){return "Shape";}
    IC int				RenderPriority			(){return 20;}

    // IO
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);
};
//---------------------------------------------------------------------------
#endif
