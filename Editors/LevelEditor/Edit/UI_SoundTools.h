#ifndef UI_SoundToolsH
#define UI_SoundToolsH

#include "ui_control.h"
#include "ui_customtools.h"

//---------------------------------------------------------------------------
class TUI_SoundTools:public TUI_CustomTools{
public:
                    TUI_SoundTools();
    virtual         ~TUI_SoundTools(){;}
	virtual	void 	OnActivate  ();
	virtual void 	OnDeactivate();
};
//---------------------------------------------------------------------------
class TUI_ControlSoundAdd: public TUI_CustomControl{
    bool __fastcall AppendCallback(SBeforeAppendCallbackParams* p);
	bool __fastcall AfterAppendCallback(TShiftState _Shift, CCustomObject* obj);
public:
    TUI_ControlSoundAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlSoundAdd(){;}
	virtual bool Start  (TShiftState _Shift);
};
//---------------------------------------------------------------------------
#endif
