#ifndef ui_toolsH
#define ui_toolsH
extern void _fastcall PanelMinimizeClick(TObject *Sender);
extern void _fastcall PanelMaximizeOnlyClick(TObject *Sender);

#include "ui_control.h"

//---------------------------------------------------------------------------
enum ETarget{
    etNone=0,
    etScene,
    etLight,
    etObject,
    etSound,
    etPClipper,
    etPivot,
    etTexturing,
    etMaxTarget,
    etForceDword = 0xffffffff
};

enum EAction{
    eaNone=0,
    eaSelect,
    eaAdd,
    eaMove,
    eaRotate,
    eaScale,
    eaMaxActions,
    eaForceDword = 0xffffffff
};

class TUI_CustomTools{
    vector<TUI_Control*> m_Actions;

public:
                    TUI_CustomTools();
    virtual         ~TUI_CustomTools();
};

class TUI_Tools{
    TUI_CustomTools* pCurTools;
protected:
    TPanel*         paParent;
    TFrame*         pCurFrame;
    TFrame*         Frames[etMaxTarget][eaMaxActions];
    TUI_Control*    ControlTools[etMaxTarget][eaMaxActions];
    ETarget         target;//, prev_target;
    EAction         action;//, prev_action;
    TUI_Control*    pCurControl;

    void __fastcall SetTargetAction();
public:
    __fastcall      TUI_Tools();
    virtual         ~TUI_Tools();
    void            Init    (TPanel* p){paParent = p;VERIFY(p);}
    ETarget         GetTarget(){return target;}
    EAction         GetAction(){return action;}
    void __fastcall Init();
    void __fastcall AddControlCB(ETarget tgt, EAction act, TUI_Control* cb) {ControlTools[tgt][act]=cb;}
    void __fastcall AddFrame    (ETarget tgt, EAction act, TFrame* f)       {Frames[tgt][act]=f;}
    void __fastcall SetTarget   (DWORD tgt);
    void __fastcall SetAction   (DWORD act);
    bool __fastcall MouseStart  (TShiftState Shift);
    bool __fastcall MouseEnd    (TShiftState Shift);
    void __fastcall MouseMove   (TShiftState Shift);
	bool HiddenMode ();
    bool __fastcall KeyDown     (WORD Key, TShiftState Shift);
    bool __fastcall KeyUp       (WORD Key, TShiftState Shift);
    bool __fastcall KeyPress    (WORD Key, TShiftState Shift);
};
extern LPSTR GetNameByClassID(int cls_id);
extern bool IsClassID(int cls_id);
extern int ClassIDFromTargetID( int target );
extern void ResetActionToSelect();
extern TShiftState ssRBOnly;
#endif
