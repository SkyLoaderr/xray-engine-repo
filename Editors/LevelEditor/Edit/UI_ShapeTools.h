#ifndef UI_ShapeToolsH
#define UI_ShapeToolsH

#include "ui_control.h"
#include "ui_customtools.h"

//refs
class TfraRPoint;

//---------------------------------------------------------------------------
class TUI_ShapeTools:public TUI_CustomTools{
public:
                    TUI_ShapeTools();
    virtual         ~TUI_ShapeTools(){;}
	virtual	void 	OnActivate  ();
	virtual void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlShapeAdd: public TUI_CustomControl{
	bool __fastcall AfterAppendCallback(CCustomObject* obj);
public:
    TUI_ControlShapeAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlShapeAdd(){;}
	virtual bool Start  (TShiftState _Shift);
};

#endif
