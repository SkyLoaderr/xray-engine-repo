//---------------------------------------------------------------------------

#ifndef ESceneWallmarkControlsH
#define ESceneWallmarkControlsH

#include "ESceneControlsCustom.h"

// refs
class ESceneWallmarkTools;

//---------------------------------------------------------------------------
class TUI_ControlWallmarkAdd: public TUI_CustomControl{
	u32 wm_cnt;
public:
    TUI_ControlWallmarkAdd(int st, int act, ESceneCustomMTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
//---------------------------------------------------------------------------

#endif
