#ifndef UI_PSToolsH
#define UI_PSToolsH

#include "ui_control.h"
#include "ui_customtools.h"

class CEvent;
//---------------------------------------------------------------------------
class TUI_PSTools:public TUI_CustomTools{
public:
                    TUI_PSTools();
    virtual         ~TUI_PSTools(){;}
	virtual void 	OnActivate  ();
	virtual void	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlPSAdd: public TUI_CustomControl{
public:
    TUI_ControlPSAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlPSAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
//---------------------------------------------------------------------------
#endif //UI_PSToolsH
