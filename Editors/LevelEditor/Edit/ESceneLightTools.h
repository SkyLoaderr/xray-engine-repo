#ifndef ESceneLightToolsH
#define ESceneLightToolsH

#include "ESceneCustomOTools.H"
#include "xr_efflensflare.h"

class CEditFlare: public CLensFlare{
public:
					CEditFlare();
  	void 			Load(IReader& F);
	void 			Save(IWriter& F);
    void			Render();
    void			DeleteShaders();
    void			CreateShaders();
};

class ESceneLightTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
    friend class 		SceneBuilder;
    friend class 		CLight;
protected:
	enum{
    	flShowSun		= (1<<0),
    };
	Flags32				m_LFlags;
	// hemisphere
	u8					m_HemiQuality;
    // sun
    u8					m_UseSunShadow;
    u8					m_SunShadowQuality;
    Fvector2			m_SunShadowDir;
protected:
    // light control
	int					lcontrol_last_idx;
	ATokenVec			lcontrols;
    void __fastcall 	OnControlAppendClick		(PropValue* sender, bool& bDataModified);
    void __fastcall 	OnControlRenameRemoveClick	(PropValue* sender, bool& bDataModified);
public:
						ESceneLightTools 	   	();
	virtual        	 	~ESceneLightTools		();

    virtual void		Clear					(bool bSpecific=false);       

    // IO
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);

    // utils
    virtual bool		Validate				();
    
    virtual void		OnRender				(int priority, bool strictB2F);
    
	virtual void 		FillProp				(LPCSTR pref, PropItemVec& items);

    AnsiString			GenLightControlName		();
    xr_a_token*   		FindLightControl		(int id);
    ATokenIt	   		FindLightControlIt		(LPCSTR name);
    xr_a_token*   		FindLightControl		(LPCSTR name){ATokenIt it = FindLightControlIt(name); return it!=lcontrols.end()?it:0;}
    void				AppendLightControl		(LPCSTR name, u32* idx=0);
    void				RemoveLightControl		(LPCSTR name);
};
#endif // ESceneCustomOToolsH

