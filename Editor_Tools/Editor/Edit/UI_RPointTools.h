#ifndef UI_RPointToolsH
#define UI_RPointToolsH

#include "ui_control.h"
#include "ui_customtools.h"

//refs
class TfraRPoint;

//---------------------------------------------------------------------------
class TUI_RPointTools:public TUI_CustomTools{
public:
                    TUI_RPointTools();
    virtual         ~TUI_RPointTools(){;}
	virtual	void 	OnActivate  ();
	virtual void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlRPointAdd: public TUI_CustomControl{
public:
    TUI_ControlRPointAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlRPointAdd(){;}
	virtual bool Start  (TShiftState _Shift);
};

#endif
