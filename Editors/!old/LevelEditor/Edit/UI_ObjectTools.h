#ifndef UI_ObjectToolsH
#define UI_ObjectToolsH

#include "ui_control.h"

//------------------------------------------------------------------------------
class TUI_ControlObjectAdd: public TUI_CustomControl{
public:
    TUI_ControlObjectAdd(int st, int act, ESceneCustomMTools* parent);
    virtual ~TUI_ControlObjectAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
#endif
