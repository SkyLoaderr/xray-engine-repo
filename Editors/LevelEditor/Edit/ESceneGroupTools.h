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

    virtual void		Clear					(bool bSpecific=false){inherited::Clear(bSpecific);}
    // IO
    virtual bool   		IsNeedSave				(){return inherited::IsNeedSave();}
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);

    virtual void		GetStaticDesc			(int& v_cnt, int& f_cnt);

    // group function
    void				UngroupObjects			(bool bUndo=true);
	void 				GroupObjects			(bool bUndo=true);
	void 				CenterToGroup			();
    void 				AlignToObject			();

    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};
//---------------------------------------------------------------------------
#endif
