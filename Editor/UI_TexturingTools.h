#ifndef UI_TexturingToolsH
#define UI_TexturingToolsH

#include "ui_control.h"

extern void InitMSCTexturing();

enum ETexturingAction{
    taNone,
    taAddFace,
    taRemoveFace,
    taNormalAlign,
    taPickTarget
};

class TUI_ControlTexturingSelect: public TUI_Control{
    ETexturingAction m_TAction;
public:
    TUI_ControlTexturingSelect();
    virtual ~TUI_ControlTexturingSelect(){;}
	virtual bool Start  (TShiftState Shift);
	virtual bool End    (TShiftState Shift);
	virtual void Move   (TShiftState Shift);
    virtual void OnEnter();
    virtual void OnExit ();
    virtual bool KeyPress(WORD Key, TShiftState Shift);
};
#endif //UI_TexturingToolsH
