#ifndef UI_EventToolsH
#define UI_EventToolsH

#include "ui_control.h"
#include "ui_customtools.h"

class CEvent;
//---------------------------------------------------------------------------
class TUI_EventTools:public TUI_CustomTools{
public:
                    TUI_EventTools();
    virtual         ~TUI_EventTools(){;}
	virtual void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlEventAdd: public TUI_CustomControl{
public:
    TUI_ControlEventAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlEventAdd(){;}
	virtual bool Start  (TShiftState _Shift);
};
#endif
