#ifndef UI_RPointToolsH
#define UI_RPointToolsH

#include "ui_control.h"
#include "ui_customtools.h"

//refs
class TfraRPoint;

//---------------------------------------------------------------------------
class TUI_SpawnPointTools:public TUI_CustomTools{
public:
                    TUI_SpawnPointTools();
    virtual         ~TUI_SpawnPointTools(){;}
	virtual	void 	OnActivate  ();
	virtual void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlSpawnPointAdd: public TUI_CustomControl{
    bool __fastcall AppendCallback(SBeforeAppendCallbackParams* p);
public:
    TUI_ControlSpawnPointAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlSpawnPointAdd(){;}
	virtual bool Start  (TShiftState _Shift);
};

#endif
