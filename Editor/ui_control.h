#ifndef ui_controlH
#define ui_controlH
//---------------------------------------------------------------------------
class TUI_Control{
public:
    TUI_Control();
    virtual ~TUI_Control();
	virtual bool Start  (TShiftState _Shift) = 0;
	virtual bool End    (TShiftState _Shift) = 0;
	virtual void Move   (TShiftState _Shift) = 0;
	virtual bool HiddenMode(){ return false; }
    virtual bool KeyDown(WORD Key, TShiftState Shift){return false;}
    virtual bool KeyUp  (WORD Key, TShiftState Shift){return false;}
    virtual bool KeyPress(WORD Key, TShiftState Shift){return false;}
    virtual void OnEnter(){;}
    virtual void OnExit(){;}
};
#endif
