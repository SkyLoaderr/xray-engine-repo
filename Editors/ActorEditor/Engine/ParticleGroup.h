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
	struct SEffect{
        enum{
            flDeferredStop	= (1<<0),
            flRandomizeTime	= (1<<1),
        };
    	enum EEffType{
        	etStopEnd		= 0,
        	etPeriodic		= 1,
            etMaxType		= u32(-1)
        };
        EEffType		m_Type;
        Flags32			m_Flags;
    	string64		m_EffectName;  
        u32				m_Time0;
        u32				m_Time1;
		SEffect(){m_Flags.zero();m_Type=etStopEnd;m_EffectName[0]=0;m_Time0=0;m_Time1=0;}
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

class CParticleGroup: public FHierrarhyVisual{
	const CPGDef*		m_Def;
    u32					m_CurrentTime;
public:
    enum{
    	flPlaying		= (1<<0),
    };
    Flags32				m_Flags;
public:
						CParticleGroup	();
						~CParticleGroup	(){;}
	void	 			OnFrame			(u32 dt);

	virtual void		Copy			(IRender_Visual* pFrom) {Debug.fatal("Can't duplicate particle system - NOT IMPLEMENTED");}

    void 				OnDeviceCreate	(){;}
    void 				OnDeviceDestroy	(){;}

    void				UpdateParent	(const Fmatrix& m, const Fvector& velocity);

    BOOL				Compile			(CPGDef* def);

	const CPGDef*		GetDefinition	(){return m_Def;}

	void				Play			();
    void				Stop			(bool bFinishPlaying=true);
};

}
#define PGD_VERSION				0x0003
//----------------------------------------------------
#define PGD_CHUNK_VERSION		0x0001
#define PGD_CHUNK_NAME			0x0002
#define PGD_CHUNK_FLAGS			0x0003
#define PGD_CHUNK_EFFECTS		0x0004

//---------------------------------------------------------------------------
#endif
