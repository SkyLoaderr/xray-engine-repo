#ifndef UI_LevelToolsH
#define UI_LevelToolsH

// refs
class ESceneCustomMTools;
class TProperties;
class TfrmObjectList;

#include "ESceneClassList.h"
#include "UI_ToolsCustom.h"
#include "UI_mainCommand.h"

//---------------------------------------------------------------------------
#define estDefault 0
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

class CLevelTools: public CToolsCustom{
	typedef CToolsCustom inherited;

    TPanel*         paParent;
    int             sub_target;
    ObjClassID		target;

    Flags32			m_Flags;

    enum{
    	flChangeAction		= (1<<0),
        flChangeTarget		= (1<<1),
        flUpdateProperties	= (1<<2),
        flUpdateObjectList	= (1<<3)
    };

    int					iNeedAction;
    ObjClassID			iNeedTarget;
    int					iNeedSubTarget;

    ESceneCustomMTools*	pCurTools;
    
    TfrmObjectList*		pObjectListForm;

    void __fastcall 	SetTargetAction		();

    void __fastcall 	RealSetAction   	(ETAction act);
    void __fastcall 	RealSetTarget   	(ObjClassID tgt,int sub_tgt, bool bForced);//=false);

    TProperties* 		m_Props;
    void __stdcall  	OnPropsModified		();
    void __stdcall  	OnPropsClose		();

    void				RealUpdateProperties();
    void				RealUpdateObjectList();
public:
    float 				fFogness;
    u32					dwFogColor;
public:
                    	CLevelTools			();
    virtual         	~CLevelTools		();

    IC ObjClassID		GetTarget   		(){return target;}
    IC int          	GetSubTarget   		(){return sub_target;}
    virtual void		SetAction			(ETAction act);
    void 			 	SetTarget			(ObjClassID tgt, int sub_tgt);

    virtual void		SetFog				(u32 fog_color, float fogness){dwFogColor=fog_color;fFogness=fogness;}
    virtual void		GetCurrentFog		(u32& fog_color, float& s_fog, float& e_fog);

    virtual void		Render				();
	virtual void		RenderEnvironment	();
    virtual void		OnFrame				();

    virtual bool		OnCreate			();
    virtual void		OnDestroy			();

    virtual bool		IfModified			();
    virtual bool		IsModified			();
    virtual void		Modified			(){;}

    virtual LPCSTR		GetInfo				();
    
    virtual void		ZoomObject			(BOOL bSelOnly);

    virtual bool		Load				(LPCSTR path, LPCSTR name){return true;}
    virtual bool		Save				(LPCSTR path, LPCSTR name, bool bInternal=false){return true;}
    virtual void		Reload				(){;}
    
    virtual void		OnDeviceCreate		(){;}
    virtual void		OnDeviceDestroy		(){;}

    virtual void		Clear				(){inherited::Clear();}

    virtual void		OnShowHint			(AStringVec& SS);

    virtual bool __fastcall 	MouseStart	(TShiftState Shift);
    virtual bool __fastcall 	MouseEnd  	(TShiftState Shift);
    virtual void __fastcall 	MouseMove 	(TShiftState Shift);
	virtual bool __fastcall 	HiddenMode	();
    virtual bool __fastcall 	KeyDown    	(WORD Key, TShiftState Shift);
    virtual bool __fastcall 	KeyUp       (WORD Key, TShiftState Shift);
    virtual bool __fastcall 	KeyPress    (WORD Key, TShiftState Shift);

    virtual bool		Pick				(TShiftState Shift);
	virtual bool 		RayPick				(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt, Fvector* n);

    virtual void		ShowProperties		();
    virtual void		UpdateProperties	(BOOL bForced){m_Flags.set(flUpdateProperties|flUpdateObjectList,TRUE); if (bForced) OnFrame();}
    virtual void		RefreshProperties	();

    // specified functions
    void            	Reset       		();

    TForm*				GetFrame			();

    void __fastcall 	ResetSubTarget		();

    void __fastcall 	OnObjectsUpdate		();

    ObjClassID 			CurrentClassID		();

    void				ShowObjectList		();

    // commands
    CCommandVar			CommandChangeTarget		(CCommandVar p1, CCommandVar p2);
	CCommandVar			CommandShowObjectList	(CCommandVar p1, CCommandVar p2);
    CCommandVar			CommandEnableTarget		(CCommandVar p1, CCommandVar p2);
};
extern CLevelTools*&	LTools;

extern void ResetActionToSelect();
extern TShiftState ssRBOnly;
#endif
