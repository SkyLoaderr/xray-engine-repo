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
	bool __fastcall				ItemExist			(LPCSTR name){return !!FindItem(name);}
	CSoundRender_Environment*	FindItem			(LPCSTR name);
    void						SetCurrentEnv		(CSoundRender_Environment* B);

    SoundEnvironment_LIB		m_Library;

    ESoundSource*				m_PreviewSnd;
    void __fastcall 			OnRevResetClick		(PropValue* sender, bool& bModif, bool& bSafe);
public:
    CSoundRender_Environment* 	m_Env;
    virtual LPCSTR				AppendItem			(LPCSTR folder_name, LPCSTR parent=0);            
    virtual BOOL __fastcall 	OnRemoveItem		(LPCSTR name, EItemType type); 
	virtual void __fastcall 	OnRenameItem		(LPCSTR old_full_name, LPCSTR new_full_name, EItemType type);
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
    virtual void				SetCurrentItem		(LPCSTR name, bool bView);
    virtual void				ApplyChanges		(bool bForced=false);

	virtual void 				RealUpdateProperties();

	virtual void 				OnFrame				();
	virtual void 				OnRender			();

    virtual void				OnDeviceCreate		(){;}
    virtual void				OnDeviceDestroy		(){;}
};
//---------------------------------------------------------------------------
#endif
