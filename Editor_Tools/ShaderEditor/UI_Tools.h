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

class CShaderTools: public pureDeviceCreate, public pureDeviceDestroy
{
	CEditableObject*	m_EditObject;
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
