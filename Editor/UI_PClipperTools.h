#ifndef UI_PClipperToolsH
#define UI_PClipperToolsH

#include "ui_control.h"
extern void InitMSCPClipper();

class PClipper;

class TUI_ControlPClipperSelect: public TUI_Control{
    bool bBoxSelection;
public:
    TUI_ControlPClipperSelect();
    virtual ~TUI_ControlPClipperSelect(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};

class TUI_ControlPClipperAdd: public TUI_Control{
    PClipper *l;
    int cur_point;
    bool bActionComplete;
    void CancelAdd();
    void Reset();
public:
    TUI_ControlPClipperAdd();
    virtual ~TUI_ControlPClipperAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
//	virtual bool HiddenMode(){ return true; }
    virtual bool KeyDown(WORD Key, TShiftState Shift);
//    virtual void KeyUp  (WORD Key, TShiftState Shift);
    virtual void OnEnter();
    virtual void OnExit ();
};

class TUI_ControlPClipperMove: public TUI_Control{
	Fvector m_XVector;
	Fvector m_YVector;
	Fvector m_MoveRemainder;
    float mX, mY;
public:
    TUI_ControlPClipperMove();
    virtual ~TUI_ControlPClipperMove(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual bool KeyDown(WORD Key, TShiftState Shift);
	virtual bool HiddenMode(){ return true; }
};

class TUI_ControlPClipperRotate: public TUI_Control{
	Fvector m_RCenter;
	Fvector m_RVector;
public:
    TUI_ControlPClipperRotate();
    virtual ~TUI_ControlPClipperRotate(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
	virtual bool HiddenMode(){ return true; }
};

class TUI_ControlPClipperScale: public TUI_Control{
	Fvector m_Center;
public:
    TUI_ControlPClipperScale();
    virtual ~TUI_ControlPClipperScale(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
	virtual bool HiddenMode(){ return true; }
};
#endif
