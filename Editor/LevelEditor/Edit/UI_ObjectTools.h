#ifndef UI_ObjectToolsH
#define UI_ObjectToolsH

#include "ui_control.h"
#include "ui_customtools.h"

//------------------------------------------------------------------------------
class TUI_ObjectTools:public TUI_CustomTools{
public:
                    TUI_ObjectTools();
    virtual         ~TUI_ObjectTools(){;}
	virtual void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//------------------------------------------------------------------------------
class TUI_ControlObjectAdd: public TUI_CustomControl{
public:
    TUI_ControlObjectAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlObjectAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
/*
//------------------------------------------------------------------------------
// Texturing
//------------------------------------------------------------------------------
enum ETexturingAction{
    taNone,
    taAddFace,
    taRemoveFace,
    taFaceBoxSelection,
    taNormalAlign,
    taPickTarget
};

#define estTexturing 1
#define estGizmo 2

class TUI_ControlGizmoMove: public TUI_CustomControl{
	Fvector m_XVector;
	Fvector m_YVector;
public:
    TUI_ControlGizmoMove(int st, int act);
    virtual ~TUI_ControlGizmoMove(){;}
	virtual bool Start  (TShiftState Shift);
	virtual bool End    (TShiftState Shift);
	virtual void Move   (TShiftState Shift);
	virtual bool HiddenMode(){ return true; }
};
class TUI_ControlGizmoRotate: public TUI_CustomControl{
	Fvector m_RCenter;
	Fvector m_RVector;
public:
    TUI_ControlGizmoRotate(int st, int act);
    virtual ~TUI_ControlGizmoRotate(){;}
	virtual bool Start  (TShiftState Shift);
	virtual bool End    (TShiftState Shift);
	virtual void Move   (TShiftState Shift);
	virtual bool HiddenMode(){ return true; }
};
class TUI_ControlGizmoScale: public TUI_CustomControl{
	Fvector m_XVector;
	Fvector m_YVector;
public:
    TUI_ControlGizmoScale(int st, int act);
    virtual ~TUI_ControlGizmoScale(){;}
	virtual bool Start  (TShiftState Shift);
	virtual bool End    (TShiftState Shift);
	virtual void Move   (TShiftState Shift);
	virtual bool HiddenMode(){ return true; }
};

//---------------------------------------------------------------------------
class TUI_ControlTexturingSelect: public TUI_CustomControl{
    ETexturingAction m_TAction;
    bool bBoxSelection;
    void AddFace();
    void RemoveFace();
    void NormalAlign();
    void PickTarget();
public:
    TUI_ControlTexturingSelect(int st, int act);
    virtual ~TUI_ControlTexturingSelect(){;}
	virtual bool Start  (TShiftState Shift);
	virtual bool End    (TShiftState Shift);
	virtual void Move   (TShiftState Shift);
    virtual void OnEnter();
    virtual void OnExit ();
    virtual bool KeyPress(WORD Key, TShiftState Shift);
};
*/
#endif
