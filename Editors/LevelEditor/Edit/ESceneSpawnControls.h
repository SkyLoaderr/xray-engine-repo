#ifndef UI_SpawnToolsH
#define UI_SpawnToolsH

#include "ui_control.h"

//refs
class TfraRPoint;

//---------------------------------------------------------------------------
class TUI_ControlSpawnAdd: public TUI_CustomControl{
    bool __fastcall AppendCallback(SBeforeAppendCallbackParams* p);
public:
    TUI_ControlSpawnAdd(int st, int act, ESceneCustomMTools* parent);
    virtual ~TUI_ControlSpawnAdd(){;}
	virtual bool Start  (TShiftState _Shift);
};

#endif
