#ifndef ui_toolsH
#define ui_toolsH

// refs
class TUI_CustomTools;
class TUI_Control;

#include "SceneClassList.h"

//---------------------------------------------------------------------------
enum ETarget{
	etFirstTool=0,
	etGroup=0,
    etObject,
    etLight,
    etShape,
    etSound,
    etGlow,
    etSpawnPoint,
    etWay,
    etSector,
    etPortal,
    etEvent,
    etPS,
    etDO,
    etMaxTarget
};

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
#define estSelf 0
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

class TUI_Tools{
    TPanel*         paParent;
    int             target;
    int             action;

    bool 			bNeedChangeAction;
    int				iNeedAction;
    bool 			bNeedChangeTarget;
    int				iNeedTarget;

    TUI_CustomTools*m_pTools[etMaxTarget];
    TUI_CustomTools*pCurTools;

    void __fastcall SetTargetAction	();

    void __fastcall SetAction   	(int act);
    void __fastcall SetTarget   	(int tgt,bool bForced=false);

public:
                    TUI_Tools		();
    virtual         ~TUI_Tools		();

    void 			OnCreate		();
    void            OnDestroy      	();
    void            Reset       	();

    IC int          GetTarget   	(){return target;}
    IC EObjClass    GetTargetClassID(){return ClassIDFromTargetID(target);}
    IC int          GetAction   	(){return action;}

    TFrame*			GetFrame		();

    void __fastcall SetSubTarget	(int tgt);
    void __fastcall UnsetSubTarget	(int tgt);

    void __fastcall ChangeTarget	(int tgt);
    void __fastcall ChangeAction	(int act);
    void __fastcall Update			();
    void __fastcall OnObjectsUpdate	();
    void __fastcall ShowProperties	();

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

    bool			Pick			();
};
extern TUI_Tools Tools;
extern void ResetActionToSelect();
extern TShiftState ssRBOnly;
extern void _fastcall PanelMinimizeClick(TObject *Sender);
extern void _fastcall PanelMaximizeOnlyClick(TObject *Sender);
#endif
