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
    	flShowSpawnType	= (1<<31),
    };
public:
						ESceneSpawnTools		():ESceneCustomOTools(OBJCLASS_SPAWNPOINT){;}
	// definition
    IC LPCSTR			ClassName				(){return "spawn";}
    IC LPCSTR			ClassDesc				(){return "Spawn";}
    IC int				RenderPriority			(){return 1;}

    void 				FillProp				(LPCSTR pref, PropItemVec& items);
};
//---------------------------------------------------------------------------
#endif
