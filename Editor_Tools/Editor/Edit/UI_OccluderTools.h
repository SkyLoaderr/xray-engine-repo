#ifndef UI_PClipperToolsH
#define UI_PClipperToolsH

#include "ui_control.h"
#include "ui_customtools.h"

class COccluder;
class TfraOccluder;
//---------------------------------------------------------------------------
class TUI_OccluderTools:public TUI_CustomTools{
public:
                    TUI_OccluderTools();
	virtual void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlOccluderAdd: public TUI_CustomControl{
    TfraOccluder*   pFrame;
public:
    TUI_ControlOccluderAdd(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual void OnEnter();
    virtual void OnExit ();
};
class TUI_ControlOccluderMove: public TUI_CustomControl{
    TfraOccluder*   pFrame;
    COccluder* 		obj;
public:
    TUI_ControlOccluderMove(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual void OnEnter();
    virtual void OnExit ();
};
class TUI_ControlOccluderSelect: public TUI_CustomControl{
    TfraOccluder*   pFrame;
    COccluder* 		obj;
public:
    TUI_ControlOccluderSelect(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual void OnEnter();
    virtual void OnExit ();
};
#endif
