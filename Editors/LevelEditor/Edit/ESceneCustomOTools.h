#ifndef EScene__ToolsH
#define EScene__ToolsH

#include "ESceneCustomOTools.h"

class ESceneLightToolsH: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
	int					lcontrol_count;
	DEFINE_MAP			(int,AnsiString,LCMap,LCMapPairIt);
	LCMap				lcontrols;
public:
						ESceneLightToolsH 	   	();
	virtual        	 	~ESceneLightToolsH		();
    // IO
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);

    void				AppendLightControl		();
    void				RemoveLightControl		();
};
#endif // ESceneCustomOToolsH

