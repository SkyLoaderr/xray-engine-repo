//---------------------------------------------------------------------------
#ifndef ESceneSpawnToolsH
#define ESceneSpawnToolsH

#include "ESceneCustomOTools.h"

class ESceneSpawnTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
    friend class 		CSpawnPoint;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
	enum{
    	flPickSpawnType	= (1<<30),
    	flShowSpawnType	= (1<<31),
    };
    Flags32				m_Flags;
public:
						ESceneSpawnTools		():ESceneCustomOTools(OBJCLASS_SPAWNPOINT){m_Flags.zero();}
	// definition
    IC LPCSTR			ClassName				(){return "spawn";}
    IC LPCSTR			ClassDesc				(){return "Spawn";}
    IC int				RenderPriority			(){return 1;}

    void 				FillProp				(LPCSTR pref, PropItemVec& items);

    // IO
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);
};
//---------------------------------------------------------------------------
#endif
