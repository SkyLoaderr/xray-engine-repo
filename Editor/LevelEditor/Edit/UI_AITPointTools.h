#ifndef UI_AITPointToolsH
#define UI_AITPointToolsH

#include "ui_control.h"
#include "ui_customtools.h"

// refs
class CAITPoint;

//---------------------------------------------------------------------------
class TUI_AITPointTools:public TUI_CustomTools{
public:
                    TUI_AITPointTools();
    virtual         ~TUI_AITPointTools(){;}
	virtual	void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlAITPointAdd: public TUI_CustomControl{
	CAITPoint* last_obj;
public:
    TUI_ControlAITPointAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlAITPointAdd(){;}
	virtual bool Start  (TShiftState _Shift);
    virtual void OnEnter();
};

#endif //UI_AITPointToolsH
