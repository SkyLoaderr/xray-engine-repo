//---------------------------------------------------------------------------
#ifndef SHSoundEnvToolsH
#define SHSoundEnvToolsH

#include "SHToolsInterface.h"
#include "soundrender_environment.h"
#include "ESound_Source.h"

// refs
class PropValue;

class CSHSoundEnvTools: public ISHTools
{
	CSoundRender_Environment*	FindItem			(LPCSTR name);
    void						SetCurrentEnv		(CSoundRender_Environment* B);

    virtual LPCSTR				GenerateItemName	(LPSTR name, LPCSTR pref, LPCSTR source);
    SoundEnvironment_LIB		m_Library;

    ESoundSource*				m_PreviewSnd;
    void __fastcall 			OnRevResetClick		(PropValue* sender, bool& bModif);
    void __fastcall 			OnEchoResetClick	(PropValue* sender, bool& bModif);
public:
    CSoundRender_Environment* 	m_Env;
    virtual LPCSTR				AppendItem			(LPCSTR folder_name, LPCSTR parent=0);
    virtual void				RemoveItem			(LPCSTR name);
	virtual void				RenameItem			(LPCSTR old_full_name, LPCSTR ren_part, int level);
	virtual void				RenameItem			(LPCSTR old_full_name, LPCSTR new_full_name);
	virtual void 				FillItemList		();

    void						UseEnvironment		(){Sound->set_user_env(m_Env);}
public:
								CSHSoundEnvTools 	(ISHInit& init);
    virtual 					~CSHSoundEnvTools	();

    virtual LPCSTR				ToolsName			(){return "Sound Environment";}

	virtual void				Reload				();
	virtual void				Load				();
	virtual void				Save				();

    virtual bool				OnCreate			();
    virtual void				OnDestroy			();
	virtual void 				OnActivate			();
	virtual void 				OnDeactivate		();

    // misc
    virtual void				ResetCurrentItem	();
    virtual void				SetCurrentItem		(LPCSTR name);
    virtual void				ApplyChanges		(bool bForced=false);

	virtual void 				UpdateProperties	();

	virtual void 				OnFrame				();
	virtual void 				OnRender			();

    virtual void				OnDeviceCreate		(){;}
    virtual void				OnDeviceDestroy		(){;}
};
//---------------------------------------------------------------------------
#endif
