#ifndef UI_GroupToolsH
#define UI_GroupToolsH

#include "ui_control.h"
#include "ui_customtools.h"

class CGroupObject;
//---------------------------------------------------------------------------
class TUI_GroupTools:public TUI_CustomTools{
public:
                    TUI_GroupTools();
    virtual         ~TUI_GroupTools(){;}
	virtual void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlGroupAdd: public TUI_CustomControl{
public:
    				TUI_ControlGroupAdd(int st, int act, TUI_CustomTools* parent);
    virtual 		~TUI_ControlGroupAdd(){;}
	virtual bool 	Start  (TShiftState _Shift);
	virtual bool 	End    (TShiftState _Shift);
	virtual void 	Move   (TShiftState _Shift);
};
#endif
