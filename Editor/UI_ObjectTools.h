#ifndef UI_ObjectToolsH
#define UI_ObjectToolsH

#include "ui_control.h"

extern void InitMSCObject();

class TUI_ControlObjectSelect: public TUI_Control{
    bool bBoxSelection;
public:
    TUI_ControlObjectSelect();
    virtual ~TUI_ControlObjectSelect(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};

class TUI_ControlObjectAdd: public TUI_Control{
public:
    TUI_ControlObjectAdd();
    virtual ~TUI_ControlObjectAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};

class TUI_ControlObjectMove: public TUI_Control{
	Fvector m_XVector;
	Fvector m_YVector;
	Fvector m_MoveRemainder;
    float mX, mY;
public:
    TUI_ControlObjectMove();
    virtual ~TUI_ControlObjectMove(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual bool KeyDown(WORD Key, TShiftState Shift);
	virtual bool HiddenMode(){ return true; }
};

class TUI_ControlObjectRotate: public TUI_Control{
	Fvector m_RCenter;
	Fvector m_RVector;
public:
    TUI_ControlObjectRotate();
    virtual ~TUI_ControlObjectRotate(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
	virtual bool HiddenMode(){ return true; }
};

class TUI_ControlObjectScale: public TUI_Control{
public:
    TUI_ControlObjectScale();
    virtual ~TUI_ControlObjectScale(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
	virtual bool HiddenMode(){ return true; }
};
#endif
