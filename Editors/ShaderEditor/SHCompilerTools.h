//---------------------------------------------------------------------------
#ifndef SHCompilerToolsH
#define SHCompilerToolsH

#include "Shader_xrLC.h"
#include "ElTree.hpp"

// refs
class PropValue;

class CSHCompilerTools
{
	BOOL				m_bModified;

	Shader_xrLC*		FindShader			(LPCSTR name);
    LPCSTR				GenerateShaderName	(LPSTR name, LPCSTR source);

    Shader_xrLC_LIB		m_Library;
    bool 				m_bUpdateCurrent;	// если менялся объект непосредственно  Update____From___()
public:
    Shader_xrLC*		m_LibShader;
    Shader_xrLC			m_EditShader;
    Shader_xrLC*		AppendShader		(LPCSTR folder_name, Shader_xrLC* parent);
    Shader_xrLC*		CloneShader			(LPCSTR name);
    void				RemoveShader		(LPCSTR name);
	void				RenameShader		(LPCSTR old_full_name, LPCSTR ren_part, int level);
	void				RenameShader		(LPCSTR old_full_name, LPCSTR new_full_name);
public:
						CSHCompilerTools 	();
    virtual 			~CSHCompilerTools	();

	void				Reload				();
	void				Load				();
	void				Save				();

    bool				IfModified			();
    bool				IsModified			(){return m_bModified;}
    void _fastcall		Modified			();

    void				OnCreate			();
    void				OnDestroy			();

    // misc
    void				ResetCurrentShader	();
    void				SetCurrentShader	(Shader_xrLC* B);
    void				SetCurrentShader	(LPCSTR name);
    void				ApplyChanges		();

	void 				UpdateProperties	();
	void 				Update				();
};
//---------------------------------------------------------------------------
#endif
