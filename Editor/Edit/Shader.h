//----------------------------------------------------
// file: Shader.h
//----------------------------------------------------
#ifndef _INCDEF_Shader_H_
#define _INCDEF_Shader_H_

#include "XRShaderLib.h"

DEFINE_VECTOR(SH_ShaderDef, ShaderList, ShaderIt);

#define SHLIB_FILENAME "shaders.xr"

class CShaderLibrary{
	friend class TfrmEditShaders;
    ShaderList		m_Shaders;
	bool 			SearchName		(char *name);
    bool 			Load			(const char* nm);
    bool 			LoadExternal	(const char* nm);
public:
					CShaderLibrary	(){;}
    virtual 		~CShaderLibrary	(){;}

    // get object properties methods
    IC ShaderIt		FirstShader		()	{return m_Shaders.begin();}
    IC ShaderIt		LastShader		()	{return m_Shaders.end();}
    IC int			ShaderCount		()	{return m_Shaders.size();}
    
    void			Init			(){Load(SHLIB_FILENAME);}
    void			Clear			();
    int 			Merge			(const char* nm);
    void			SaveLibrary		(const char* nm=SHLIB_FILENAME);
    void			Reload			(const char* nm=SHLIB_FILENAME);
    void			Backup			();
    void			RestoreBackup	();

    void 			GenerateName	(AnsiString& buffer, AnsiString* pref=0);

    SH_ShaderDef*	AddShader		(AnsiString& name, SH_ShaderDef* src=0);
    void			DeleteShader	(AnsiString& name);
    SH_ShaderDef*	FindShader		(AnsiString& name){ return shLibrary_Find(name.c_str(), m_Shaders);}
    void			Sort			();
    
    bool 			Validate		(){return true;}
};

extern CShaderLibrary* SHLib;
#endif /*_INCDEF_Shader_H_*/

