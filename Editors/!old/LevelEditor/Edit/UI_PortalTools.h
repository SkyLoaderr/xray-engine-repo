#ifndef UI_PortalToolsH
#define UI_PortalToolsH

#include "ui_control.h"

//---------------------------------------------------------------------------
class TUI_ControlPortalSelect: public TUI_CustomControl{
public:
    TUI_ControlPortalSelect(int st, int act, ESceneCustomMTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
};
#endif //UI_SectorToolsH
