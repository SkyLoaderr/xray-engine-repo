//---------------------------------------------------------------------------
#ifndef SHCompilerToolsH
#define SHCompilerToolsH

#include "Shader_xrLC.h"
#include "SHToolsInterface.h"

// refs
class PropValue;                               

class CSHCompilerTools: public ISHTools
{
	Shader_xrLC*			FindItem			(LPCSTR name);
    virtual LPCSTR			GenerateItemName	(LPSTR name, LPCSTR pref, LPCSTR source);
    Shader_xrLC_LIB			m_Library;
public:
    Shader_xrLC*			m_Shader;
    virtual LPCSTR			AppendItem			(LPCSTR folder_name, LPCSTR parent=0);
    virtual void			RemoveItem			(LPCSTR name);
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR ren_part, int level);
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR new_full_name);
	virtual void			FillItemList		();
public:
							CSHCompilerTools 	(ISHInit& init);
    virtual 				~CSHCompilerTools	();

    virtual LPCSTR			ToolsName			(){return "Compiler Shader";}

	virtual void			Reload				();
	virtual void			Load				();
	virtual void			Save				();

    virtual bool			OnCreate			();
    virtual void			OnDestroy			();
	virtual void 			OnActivate			();
	virtual void 			OnDeactivate		(){;}

    // misc
    virtual void			ResetCurrentItem	();
    virtual void			SetCurrentItem		(LPCSTR name);
    virtual void			ApplyChanges		(bool bForced=false);

	virtual void 			RealUpdateProperties();

	virtual void 			OnFrame				();
	virtual void 			OnRender			(){;}

    virtual void			OnDeviceCreate		(){;}
    virtual void			OnDeviceDestroy		(){;}
};
//---------------------------------------------------------------------------
#endif
