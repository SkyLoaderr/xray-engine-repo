//---------------------------------------------------------------------------
#ifndef ParticleGroupH
#define ParticleGroupH

#include "FHierrarhyVisual.h"

namespace PS{
class ENGINE_API CParticleEffect;

class ENGINE_API CPGDef
{
public:
    string64			m_Name;
    Flags32				m_Flags;
    float				m_fTimeLimit;
	struct SEffect{
        enum{
            flDeferredStop	= (1<<0),
            flRandomizeTime	= (1<<1),
            flEnabled		= (1<<2),
        };
    	enum EEffType{
        	etStopEnd		= 0,
            etMaxType		= u32(-1)
        };
        EEffType		m_Type;
        Flags32			m_Flags;
    	string64		m_EffectName;  
        float			m_Time0;
        float			m_Time1;
		SEffect(){m_Flags.set(flEnabled);m_Type=etStopEnd;m_EffectName[0]=0;m_Time0=0;m_Time1=0;}
    };
    DEFINE_VECTOR(SEffect,EffectVec,EffectIt);
    EffectVec			m_Effects;
#ifdef _PARTICLE_EDITOR
	void __fastcall 	OnEffectsEditClick	(PropValue* sender, bool& bDataModified);
	void __fastcall 	OnEffectTypeChange	(PropValue* sender);
	void __fastcall 	OnEffectEditClick	(PropValue* sender, bool& bDataModified);
	void __fastcall 	OnControlClick	(PropValue* sender, bool& bDataModified);
	void __fastcall 	OnParamsChange	(PropValue* sender);
	void				FillProp	   	(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner);
#endif
public:
						CPGDef		  	();
                        ~CPGDef		  	();
    void				SetName		  	(LPCSTR name);

   	void 				Save		  	(IWriter& F);
   	BOOL 				Load		 	(IReader& F);
};
DEFINE_VECTOR(CPGDef*,PGDVec,PGDIt);

class CParticleGroup: public FHierrarhyVisual, IParticleCustom{
	const CPGDef*		m_Def;
    float				m_CurrentTime;
	Fvector				m_InitialPosition;
public:
    enum{
    	flRT_Playing		= (1<<0),
    	flRT_DefferedStop	= (1<<1),
    };
    Flags8				m_RT_Flags;
public:
						CParticleGroup	();
						~CParticleGroup	(){;}
	void	 			OnFrame			(u32 dt);

	virtual void		Copy			(IRender_Visual* pFrom) {Debug.fatal("Can't duplicate particle system - NOT IMPLEMENTED");}

    void 				OnDeviceCreate	();
    void 				OnDeviceDestroy	();

    void				UpdateParent	(const Fmatrix& m, const Fvector& velocity);

    BOOL				Compile			(CPGDef* def);

	const CPGDef*		GetDefinition	(){return m_Def;}

	void				Play			();
    void				Stop			(bool bFinishPlaying=true);
    BOOL				IsPlaying		(){return m_RT_Flags.is(flRT_Playing);}
};

}
#define PGD_VERSION				0x0003
//----------------------------------------------------
#define PGD_CHUNK_VERSION		0x0001
#define PGD_CHUNK_NAME			0x0002
#define PGD_CHUNK_FLAGS			0x0003
#define PGD_CHUNK_EFFECTS		0x0004
#define PGD_CHUNK_TIME_LIMIT	0x0005

//---------------------------------------------------------------------------
#endif
