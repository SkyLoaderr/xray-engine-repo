#ifndef ui_toolsH
#define ui_toolsH

// refs
class TUI_Control;
class TProperties;
class TfrmObjectList;

#include "SceneClassList.h"
#include "ui_customtools.h"

//---------------------------------------------------------------------------
enum EAction{
    eaSelect=0,
    eaAdd,
    eaMove,
    eaRotate,
    eaScale,
    eaMaxActions
};

enum EAxis{
    eAxisX=0,
	eAxisY,
    eAxisZ,
    eAxisZX
};
#define estDefault 0
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

class TUI_Tools{
    TPanel*         paParent;
    int             sub_target;
    EObjClass		target;
    int             action;

    Flags32			m_Flags;

    enum{
    	flChangeAction		= (1<<0),
        flChangeTarget		= (1<<1),
        flUpdateProperties	= (1<<2),
        flUpdateObjectList	= (1<<3)
    };

    int				iNeedAction;
    EObjClass		iNeedTarget;

    UIToolsMap		UITools;
    TUI_CustomTools*pCurTools;

    void			RegisterTools	(TUI_CustomTools* tools);
    
    TfrmObjectList*	pObjectListForm;

    void __fastcall SetTargetAction	();

    void __fastcall SetAction   	(int act);
    void __fastcall SetTarget   	(EObjClass tgt,bool bForced=false);

    TProperties* 	m_Props;
    void __fastcall OnPropsModified	();
    void __fastcall OnPropsClose	();

    void			RealUpdateProperties();
    void			RealUpdateObjectList();
public:
    float 			fFogness;
    u32				dwFogColor;
public:
                    TUI_Tools		();
    virtual         ~TUI_Tools		();

    bool 			OnCreate		();
    void            OnDestroy      	();
    void            Reset       	();

	bool 			IfModified		();
	bool			IsModified		();

    void			ZoomObject		(bool bSelectedOnly);
    void			SetFog			(u32 fog_color, float fogness){dwFogColor=fog_color;fFogness=fogness;}
    void			GetCurrentFog	(u32& fog_color, float& s_fog, float& e_fog);
    LPCSTR			GetInfo			();

    void __fastcall OnFrame			();
    void __fastcall Render			();
    void __fastcall RenderEnvironment();

    IC EObjClass	GetTarget   	(){return target;}
    IC int          GetAction   	(){return action;}
    IC int          GetSubTarget   	(){return sub_target;}

    TFrame*			GetFrame		();

    void __fastcall ResetSubTarget	();
    void __fastcall SetSubTarget	(int tgt);

    void __fastcall ChangeTarget	(EObjClass tgt, bool forced=false);
    void __fastcall ChangeAction	(int act, bool forced=false);
    void __fastcall OnObjectsUpdate	();

    void __fastcall	SetNumPosition	(CCustomObject* O);
    void __fastcall	SetNumRotation	(CCustomObject* O);
    void __fastcall	SetNumScale		(CCustomObject* O);

    void			OnShowHint		(AStringVec& ss);

    bool __fastcall MouseStart  	(TShiftState Shift);
    bool __fastcall MouseEnd    	(TShiftState Shift);
    void __fastcall MouseMove   	(TShiftState Shift);
	bool __fastcall HiddenMode  	();
    bool __fastcall KeyDown     	(WORD Key, TShiftState Shift);
    bool __fastcall KeyUp       	(WORD Key, TShiftState Shift);
    bool __fastcall KeyPress    	(WORD Key, TShiftState Shift);
    EObjClass 		CurrentClassID();

    bool			Pick			(TShiftState Shift);

    void			ShowObjectList	();

    void			ShowProperties	();
    void			HideProperties	();
    void			UpdateProperties(bool bForced){m_Flags.set(flUpdateProperties|flUpdateObjectList,TRUE); if (bForced) OnFrame();}
    void			RefreshProperties();
};
extern TUI_Tools Tools;
extern void ResetActionToSelect();
extern TShiftState ssRBOnly;
#endif
