//---------------------------------------------------------------------------
#ifndef ShaderToolsH
#define ShaderToolsH

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
	CLibObject*			m_LibObject;
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
};
extern CShaderTools	SHTools;
//---------------------------------------------------------------------------
#endif
