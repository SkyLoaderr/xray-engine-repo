#ifndef UI_SoundToolsH
#define UI_SoundToolsH

#include "ui_control.h"

extern void InitMSCSound();

class TUI_ControlSoundSelect: public TUI_Control{
    bool bBoxSelection;
public:
    TUI_ControlSoundSelect();
    virtual ~TUI_ControlSoundSelect(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};

class TUI_ControlSoundAdd: public TUI_Control{
public:
    TUI_ControlSoundAdd();
    virtual ~TUI_ControlSoundAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};

class TUI_ControlSoundMove: public TUI_Control{
	Fvector m_XVector;
	Fvector m_YVector;
	Fvector m_MoveRemainder;
    float mX, mY;
public:
    TUI_ControlSoundMove();
    virtual ~TUI_ControlSoundMove(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual bool KeyDown(WORD Key, TShiftState Shift);
	virtual bool HiddenMode(){ return true; }
};

class TUI_ControlSoundRotate: public TUI_Control{
public:
    TUI_ControlSoundRotate();
    virtual ~TUI_ControlSoundRotate(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
	virtual bool HiddenMode(){ return true; }
};

class TUI_ControlSoundScale: public TUI_Control{
public:
    TUI_ControlSoundScale();
    virtual ~TUI_ControlSoundScale(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
	virtual bool HiddenMode(){ return true; }
};
#endif
