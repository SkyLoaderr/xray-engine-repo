#ifndef ui_customtoolsH
#define ui_customtoolsH

// refs
class TUI_CustomControl;
class TUI_Control;

#include "SceneClassList.h"
#include "SceneObject.h"

//---------------------------------------------------------------------------
class TUI_CustomTools{
    friend class    TUI_Tools;
    friend class    TUI_CustomControl;

    vector<TUI_CustomControl*> m_Controls;
    TUI_CustomControl* pCurControl;
    int             action;
    vector<int>     sub_target_stack;

    EObjClass		objclass;

    TUI_CustomControl* FindControl	(int subtarget, int action);
    void            UpdateControl	();
protected:
    void            AddControlCB	(TUI_CustomControl* c);
public:
    TFrame*			pFrame;
public:
                    TUI_CustomTools (EObjClass cls);
    virtual         ~TUI_CustomTools();

    void            SetAction   	(int action);
    void            SetSubTarget	(int target);
    void            UnsetSubTarget	(int target);
    void            ResetSubTarget	();

    virtual void    OnActivate  	();
    virtual void    OnDeactivate	();
    virtual void    OnObjectsUpdate	(){;}
    
    virtual void	ShowProperties	();

    virtual void 	SetNumPosition	(SceneObject* O);
    virtual void 	SetNumRotation	(SceneObject* O);
    virtual void 	SetNumScale		(SceneObject* O);

	const EObjClass	ObjClass		()const {return objclass;}
};
#endif //ui_customtoolsH
