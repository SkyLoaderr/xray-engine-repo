#ifndef ui_customtoolsH
#define ui_customtoolsH

// refs
class TUI_CustomControl;
class TUI_Control;
class ESceneCustomMTools;

#include "UI_Tools.h"

DEFINE_VECTOR(TUI_CustomControl*,ControlsVec,ControlsIt);

//---------------------------------------------------------------------------
class TUI_CustomTools{
    friend class    TUI_Tools;
    friend class    TUI_CustomControl;

    ControlsVec 	m_Controls;
    TUI_CustomControl* pCurControl;
    int             action;
    int				sub_target;

    EObjClass		objclass;
	ESceneCustomMTools* scene_tools;

    TUI_CustomControl* FindControl	(int subtarget, int action);
    void            UpdateControl	();
    void			CreateDefaultControls();
protected:
    void            AddControlCB	(TUI_CustomControl* c);
public:
    TFrame*			pFrame;
public:
                    TUI_CustomTools (EObjClass cls, bool bCreateDefaultControls);
    virtual         ~TUI_CustomTools();

    void            SetAction   	(int action);
    void            SetSubTarget	(int target);
    void            ResetSubTarget	();

    virtual void    OnActivate  	();
    virtual void    OnDeactivate	();
    virtual void    OnObjectsUpdate	(){;}

    virtual void 	SetNumPosition	(CCustomObject* O);
    virtual void 	SetNumRotation	(CCustomObject* O);
    virtual void 	SetNumScale		(CCustomObject* O);

	const EObjClass	ObjClass		()const {return objclass;}
	ESceneCustomMTools* SceneTools	(){return scene_tools;}
};
#endif //ui_customtoolsH
