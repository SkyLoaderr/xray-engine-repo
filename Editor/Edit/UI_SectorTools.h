#ifndef UI_SectorToolsH
#define UI_SectorToolsH

#include "ui_control.h"
#include "ui_customtools.h"

//---------------------------------------------------------------------------
class TUI_SectorTools:public TUI_CustomTools{
public:
                    TUI_SectorTools();
    virtual         ~TUI_SectorTools(){;}
	virtual void 	OnActivate();
	virtual	void 	OnDeactivate();
    virtual void    OnObjectsUpdate();
};
//---------------------------------------------------------------------------
// refs
class TfraSector;

enum ESectorAction{
    saNone,
    saAddFace,
    saDelFace,
    saFaceBoxSelection
};

class TUI_ControlSectorAdd: public TUI_CustomControl{
	ESectorAction 	m_Action;
	void 			AddFace();
	void 			DelFace();
    TfraSector*		fraSector;
public:
    TUI_ControlSectorAdd(int st, int act, TUI_CustomTools* parent);
    virtual ~TUI_ControlSectorAdd(){;}
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual void OnEnter();
    virtual void OnExit ();
};

class TUI_ControlSectorSelect: public TUI_CustomControl{
    TfraSector*   	pFrame;
public:
    TUI_ControlSectorSelect(int st, int act, TUI_CustomTools* parent);
	virtual bool Start  (TShiftState _Shift);
	virtual bool End    (TShiftState _Shift);
	virtual void Move   (TShiftState _Shift);
    virtual void OnEnter();
    virtual void OnExit ();
};
#endif //UI_SectorToolsH
