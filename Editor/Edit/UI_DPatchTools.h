#ifndef UI_DPatchToolsH
#define UI_DPatchToolsH

#include "ui_control.h"
#include "ui_customtools.h"
//---------------------------------------------------------------------------
class TUI_DPatchTools:public TUI_CustomTools{
public:
                    TUI_DPatchTools();
    virtual         ~TUI_DPatchTools(){;}
	virtual void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//---------------------------------------------------------------------------
// refs
class TfraDPatch;

class TUI_ControlDPatchSelect: public TUI_CustomControl{
    bool bBoxSelection;
public:
    TUI_ControlDPatchSelect(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlDPatchSelect(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};

class TUI_ControlDPatchAdd: public TUI_CustomControl{
    void GenerateDPatch();
	TfraDPatch* fraDPatch;            
public:
    TUI_ControlDPatchAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlDPatchAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual void OnEnter();
    virtual void OnExit();
};

#endif
