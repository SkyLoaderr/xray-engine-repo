//---------------------------------------------------------------------------
#ifndef UIToolsH
#define UIToolsH

#include "SHEngineTools.h"
#include "SHCompilerTools.h"

// refs
class CEditableObject;
class CLibObject;
class CBlender;

enum EActiveEditor{
	aeEngine = 0,
    aeCompiler
};

enum EAction{
    eaSelect=0,
    eaAdd,
    eaMove,
    eaRotate,
    eaScale,
    eaMaxActions
};

enum EAxis{
    eAxisX=0,
	eAxisY,
    eAxisZ,
    eAxisZX
};

class CShaderTools: public pureDeviceCreate, public pureDeviceDestroy
{
	CEditableObject*	m_EditObject;
    bool				m_bCustomEditObject;
    EAction 			m_Action;
    bool				m_bNeedUpdateDeviceShaders;
public:
    CSHEngineTools		Engine;
    CSHCompilerTools	Compiler;
public:
						CShaderTools		();
    virtual 			~CShaderTools		();

    void				Render				();
    void				Update				();

    void				OnCreate			();
    void				OnDestroy			();

    bool				IfModified			(){return Engine.IfModified()&&Compiler.IfModified();}
    bool				IsModified			(){return Engine.IsModified()||Compiler.IsModified();}
    void				Modified			();

    void				ZoomObject			();

    virtual void		OnDeviceCreate		();
    virtual void		OnDeviceDestroy		();

    void				SelectPreviewObject	(int p);
    void				ResetPreviewObject	();

    EActiveEditor		ActiveEditor		();
    void				OnChangeEditor		();

    void				UpdateObjectShader	();
    void                UpdateDeviceShaders	(){m_bNeedUpdateDeviceShaders=true;}

    void				OnShowHint			(AStringVec& ss);

    void				ChangeAction		(EAction action){m_Action=action;}
    void				ApplyChanges		();

    bool __fastcall 	MouseStart  		(TShiftState Shift){return false;}
    bool __fastcall 	MouseEnd    		(TShiftState Shift){return false;}
    void __fastcall 	MouseMove   		(TShiftState Shift){;}
	bool __fastcall 	HiddenMode  		(){return false;}
    bool __fastcall 	KeyDown     		(WORD Key, TShiftState Shift){return false;}
    bool __fastcall 	KeyUp       		(WORD Key, TShiftState Shift){return false;}
    bool __fastcall 	KeyPress    		(WORD Key, TShiftState Shift){return false;}
};
extern CShaderTools	Tools;
//---------------------------------------------------------------------------
#endif
