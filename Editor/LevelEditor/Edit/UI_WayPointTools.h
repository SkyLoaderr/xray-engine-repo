#ifndef UI_WayPointToolsH
#define UI_WayPointToolsH

#include "ui_control.h"
#include "ui_customtools.h"

// refs
class CWayPoint;

//---------------------------------------------------------------------------
class TUI_WayPointTools:public TUI_CustomTools{
public:
                    TUI_WayPointTools();
    virtual         ~TUI_WayPointTools(){;}
	virtual	void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlWayPointAdd: public TUI_CustomControl{
public:
    TUI_ControlWayPointAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlWayPointAdd(){;}
	virtual bool Start  (TShiftState _Shift);
    virtual void OnEnter();
};

#endif //UI_WayPointToolsH
