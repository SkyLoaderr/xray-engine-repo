//---------------------------------------------------------------------------
#ifndef ESceneObjectToolsH
#define ESceneObjectToolsH

#include "ESceneCustomOTools.h"

class ESceneObjectTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();

    enum{
    	flAppendRandom			= (1<<29),
    	flAppendRandomScale		= (1<<30),
    	flAppendRandomRotation	= (1<<31),
    };
    Flags32				m_Flags;
public:
    Fvector				m_AppendRandomMinScale;
    Fvector				m_AppendRandomMaxScale;
    Fvector				m_AppendRandomMinRotation;
    Fvector				m_AppendRandomMaxRotation;
    AStringVec			m_AppendRandomObjects;
public:
						ESceneObjectTools		();

    virtual bool		Validate				();
	// definition
    IC LPCSTR			ClassName				(){return "scene_object";}
    IC LPCSTR			ClassDesc				(){return "Scene Object";}
    IC int				RenderPriority			(){return 1;}

    // IO
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);

    // append random
    void				FillAppendRandomProperties	();
    void				ActivateAppendRandom		(BOOL val){m_Flags.set(flAppendRandom,val);}
    BOOL				IsAppendRandomActive		(){return m_Flags.is(flAppendRandom);}
    BOOL				IsAppendRandomScaleActive	(){return m_Flags.is(flAppendRandomScale);}
    BOOL				IsAppendRandomRotationActive(){return m_Flags.is(flAppendRandomRotation);}
};
//---------------------------------------------------------------------------
#endif
