#ifndef UI_PortalToolsH
#define UI_PortalToolsH

#include "ui_control.h"
#include "ui_customtools.h"

// refs
class TfraPortal;

//---------------------------------------------------------------------------
class TUI_PortalTools:public TUI_CustomTools{
public:
                    TUI_PortalTools();
    virtual         ~TUI_PortalTools(){;}
    virtual void    OnObjectsUpdate();
	virtual void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//---------------------------------------------------------------------------

class TUI_ControlPortalAdd: public TUI_CustomControl{
	bool bBoxSelection;
public:
    TUI_ControlPortalAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlPortalAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual void OnEnter();
    virtual void OnExit ();
};

class TUI_ControlPortalSelect: public TUI_CustomControl{
    TfraPortal*   	pFrame;
public:
    TUI_ControlPortalSelect(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual void OnEnter();
    virtual void OnExit ();
};
#endif //UI_PortalToolsH
