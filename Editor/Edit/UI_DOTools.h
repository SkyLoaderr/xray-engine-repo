#ifndef UI_DOToolsH
#define UI_DOToolsH

#include "ui_control.h"
#include "ui_customtools.h"

// refs
class TfraDetailObject;

//---------------------------------------------------------------------------
class TUI_DOTools:public TUI_CustomTools{
public:
                    TUI_DOTools();
	virtual	void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlDOAdd: public TUI_CustomControl{
	TfraDetailObject* fraDO;
public:
    TUI_ControlDOAdd(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
    virtual void OnEnter();
    virtual void OnExit ();
};

#endif //UI_DOToolsH
