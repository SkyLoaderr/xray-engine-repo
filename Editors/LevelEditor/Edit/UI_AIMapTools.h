#ifndef UI_AIMapToolsH
#define UI_AIMapToolsH

#include "ui_control.h"
#include "ui_customtools.h"

// refs
class ESceneAIMapTools;

#define estAIMapNode 	0
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
	int 			append_nodes;
public:
    TUI_ControlAIMapNodeAdd(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
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
class TUI_ControlAIMapNodeMove: public TUI_CustomControl{
public:
    TUI_ControlAIMapNodeMove(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
//---------------------------------------------------------------------------
class TUI_ControlAIMapNodeRotate: public TUI_CustomControl{
public:
    TUI_ControlAIMapNodeRotate(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
//---------------------------------------------------------------------------
#endif //UI_AIMapToolsH
