#ifndef ESceneWayControlsH
#define ESceneWayControlsH

#include "ESceneControlsCustom.h"

// refs
class CWayPoint;

//---------------------------------------------------------------------------
class TUI_ControlWayPointAdd: public TUI_CustomControl{
public:
    TUI_ControlWayPointAdd(int st, int act, ESceneCustomMTools* parent);
    virtual ~TUI_ControlWayPointAdd(){;}
	virtual bool Start  (TShiftState _Shift);
    virtual void OnEnter();
};

#endif //UI_WayPointToolsH
