#ifndef UI_LightToolsH
#define UI_LightToolsH

#include "ui_control.h"

extern void InitMSCLight();

class TUI_ControlLightSelect: public TUI_Control{
    bool bBoxSelection;
public:
    TUI_ControlLightSelect();
    virtual ~TUI_ControlLightSelect(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};

class TUI_ControlLightAdd: public TUI_Control{
public:
    TUI_ControlLightAdd();
    virtual ~TUI_ControlLightAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};

class TUI_ControlLightMove: public TUI_Control{
	Fvector m_XVector;
	Fvector m_YVector;
	Fvector m_MoveRemainder;
    float mX, mY;
public:
    TUI_ControlLightMove();
    virtual ~TUI_ControlLightMove(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual bool KeyDown(WORD Key, TShiftState Shift);
	virtual bool HiddenMode(){ return true; }
};

class TUI_ControlLightRotate: public TUI_Control{
public:
    TUI_ControlLightRotate();
    virtual ~TUI_ControlLightRotate(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
	virtual bool HiddenMode(){ return true; }
};

class TUI_ControlLightScale: public TUI_Control{
public:
    TUI_ControlLightScale();
    virtual ~TUI_ControlLightScale(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
	virtual bool HiddenMode(){ return true; }
};
#endif
