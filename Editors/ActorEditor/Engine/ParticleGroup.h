//---------------------------------------------------------------------------
#ifndef ParticleGroupH
#define ParticleGroupH

#include "FHierrarhyVisual.h"

namespace PS{
class ENGINE_API CParticleEffect;

class ENGINE_API CPGDef
{
public:
    enum{
        df__			= (1<<0),
    };

    string64			m_Name;
    Flags32				m_Flags;

	struct SEffect{
    	string64		effect;
        float			offset_time;
        SEffect(){effect[0]=0;offset_time=0.f;}
    };
    DEFINE_VECTOR(SEffect,EffectVec,EffectIt);
    EffectVec			m_Effects;
#ifdef _PARTICLE_EDITOR
    u32					m_Count;
	void __fastcall 	OnCountChange	(PropValue* sender);
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
	CPGDef*				m_Def;
    float				m_CurrentTime;
    DEFINE_VECTOR(CParticleEffect*,EffectVec,EffectIt);
    EffectVec			m_Effects;
public:
						CParticleGroup	(){;}
						~CParticleGroup	(){;}
	void	 			OnFrame			(u32 dt){;}

	u32					RenderTO		(FVF::TL* V){;}
	virtual void		Render			(float LOD){;}
	virtual void		Copy			(IRender_Visual* pFrom){;}

    void 				OnDeviceCreate	(){;}
    void 				OnDeviceDestroy	(){;}

    void				UpdateParent	(const Fmatrix& m, const Fvector& velocity){;}

    BOOL				Compile			(CPGDef* def){;}

	CPGDef*				GetDefinition	(){return m_Def;}

	void				Play			(){;}
    void				Stop			(bool bFinishPlaying=true){;}
};

}
#define PGD_VERSION				0x0001
//----------------------------------------------------
#define PGD_CHUNK_VERSION		0x0001
#define PGD_CHUNK_NAME			0x0002
#define PGD_CHUNK_FLAGS			0x0003
#define PGD_CHUNK_EFFECTS		0x0004

//---------------------------------------------------------------------------
#endif
