//---------------------------------------------------------------------------
#ifndef ESceneGroupToolsH
#define ESceneGroupToolsH

#include "ESceneCustomOTools.h"

class ESceneGroupTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneGroupTools		():ESceneCustomOTools(OBJCLASS_GROUP){;}
	// definition
    IC LPCSTR			ClassName				(){return "group";}
    IC LPCSTR			ClassDesc				(){return "Group";}
    IC int				RenderPriority			(){return 1;}

    // IO
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);
};
//---------------------------------------------------------------------------
#endif
