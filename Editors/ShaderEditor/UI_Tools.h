//---------------------------------------------------------------------------
#ifndef UIToolsH
#define UIToolsH

#include "SHEngineTools.h"
#include "SHCompilerTools.h"

// refs
class CEditableObject;
class CLibObject;
class CBlender;
class TProperties;

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
public:
    TProperties*		m_Props;
    CSHEngineTools		SEngine;
    CSHCompilerTools	SCompiler;
public:
						CShaderTools		();
    virtual 			~CShaderTools		();

    void				Render				();
    void				Update				();

    bool				OnCreate			();
    void				OnDestroy			();

    bool				IfModified			(){return SEngine.IfModified()&&SCompiler.IfModified();}
    bool				IsModified			(){return SEngine.IsModified()||SCompiler.IsModified();}
    void				Modified			();

    void				ZoomObject			();

    virtual void		OnDeviceCreate		();
    virtual void		OnDeviceDestroy		();

    void				SelectPreviewObject	(int p);
    void				ResetPreviewObject	();

    EActiveEditor		ActiveEditor		();
    void				OnChangeEditor		();

    void				UpdateObjectShader	();

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

    bool				Pick				(){return false;}
    void				ShowProperties		();
};
extern CShaderTools	Tools;
//---------------------------------------------------------------------------
#endif
