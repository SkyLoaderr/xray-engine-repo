#ifndef UI_AIMapToolsH
#define UI_AIMapToolsH

#include "ui_control.h"
#include "ui_customtools.h"

#define estAIMapNode 	0
#define estAIMapEmitter	1
//---------------------------------------------------------------------------
class TUI_AIMapTools:public TUI_CustomTools{
public:
                    TUI_AIMapTools();
    virtual         ~TUI_AIMapTools(){;}
	virtual void 	OnActivate  ();
	virtual void	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlAIMapNodeAdd: public TUI_CustomControl{
public:
    TUI_ControlAIMapNodeAdd(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
//---------------------------------------------------------------------------
class TUI_ControlAIMapEmitterAdd: public TUI_CustomControl{
public:
    TUI_ControlAIMapEmitterAdd(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift){return true;}
	virtual void Move   (TShiftState _Shift){;}
};
//---------------------------------------------------------------------------
class TUI_ControlAIMapNodeSelect: public TUI_CustomControl{
public:
    TUI_ControlAIMapNodeSelect(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
//---------------------------------------------------------------------------
class TUI_ControlAIMapEmitterSelect: public TUI_CustomControl{
public:
    TUI_ControlAIMapEmitterSelect(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
//---------------------------------------------------------------------------
class TUI_ControlAIMapEmitterMove: public TUI_CustomControl{
public:
    TUI_ControlAIMapEmitterMove(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
//---------------------------------------------------------------------------
#endif //UI_AIMapToolsH
