#ifndef UI_SceneToolsH
#define UI_SceneToolsH

#include "ui_control.h"

extern void InitMSCScene();

class TUI_ControlSceneSelect: public TUI_Control{
    bool bBoxSelection;
public:
    TUI_ControlSceneSelect();
    virtual ~TUI_ControlSceneSelect(){;}
	virtual bool Start  (TShiftState Shift);
	virtual bool End    (TShiftState Shift);
	virtual void Move   (TShiftState Shift);
};

class TUI_ControlSceneMove: public TUI_Control{
	Fvector m_XVector;
	Fvector m_YVector;
    float mX, mY;
public:
    TUI_ControlSceneMove();
    virtual ~TUI_ControlSceneMove(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
	virtual bool HiddenMode(){ return true; }
    virtual bool KeyDown(WORD Key, TShiftState Shift);
    virtual bool KeyUp  (WORD Key, TShiftState Shift);
    virtual bool KeyPress(WORD Key, TShiftState Shift);
};
#endif
