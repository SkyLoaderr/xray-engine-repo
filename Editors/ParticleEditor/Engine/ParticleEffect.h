//---------------------------------------------------------------------------
#ifndef ParticleEffectH
#define ParticleEffectH
//---------------------------------------------------------------------------

#include "FBasicVisual.h"
#include "ParticleCustom.h"

// refs
namespace PAPI{
struct Particle;
struct ParticleEffect;
struct PAHeader;
}

namespace PS{
class PFunction;
struct SFrame
{
    Fvector2			m_fTexSize;
    Fvector2			m_fFrameSize;
    int     			m_iFrameDimX;
    int 				m_iFrameCount;
    float				m_fSpeed;

    IC void 			InitDefault()
    {
        m_fFrameSize.set(32.f,64.f);
        m_fTexSize.set	(32.f/256.f,64.f/128.f);
        m_iFrameDimX 	= 8;
        m_iFrameCount 	= 16;
        m_fSpeed		= 24.f;
    }
    IC void 			Set(int frame_count, float w, float h, float fw, float fh)
    {
        m_iFrameCount 	= frame_count;
        m_fFrameSize.set(fw,fh);
        m_fTexSize.set	(fw/w,fh/h);
        m_iFrameDimX 	= iFloor(1.f/m_fTexSize.x);
    }
    IC void       		CalculateTC(int frame, Fvector2& lt, Fvector2& rb)
    {
        lt.x       	 	= (frame%m_iFrameDimX)*m_fTexSize.x;
        lt.y        	= (frame/m_iFrameDimX)*m_fTexSize.y;
        rb.x        	= lt.x+m_fTexSize.x;
        rb.y        	= lt.y+m_fTexSize.y;
    }
};

class ENGINE_API CPEDef
{
public:
    enum{
    	dfSprite		= (1<<0),
    	dfModel			= (1<<1),

    	dfFramed		= (1<<10),
    	dfAnimated		= (1<<11),
        dfRandomFrame   = (1<<12),
        dfRandomPlayback= (1<<13),
        dfTimeLimit		= (1<<14),
    };

    string64			m_Name;
    Flags32				m_Flags;

    LPSTR				m_ShaderName;
    LPSTR				m_TextureName;

    SFrame				m_Frame;

	u32					m_TimeLimit;

    int					m_MaxParticles;

    int					m_ActionCount;
    PAPI::PAHeader*		m_ActionList;

	ref_shader			m_CachedShader;
#ifdef _PARTICLE_EDITOR
public:
	AnsiString			m_SourceText;
	void __fastcall 	OnSourceTextEdit	(PropValue* sender, bool& bDataModified);
	void __fastcall 	OnControlClick		(PropValue* sender, bool& bDataModified);
	void				FillProp		   	(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner);
#endif
public:
    // api function
	// state api
    void				pSprite				(string64& sh_name, string64& tex_name);
    void				pFrame				(BOOL random_frame=TRUE, u32 frame_count=16, u32 texture_width=128, u32 texture_height=128, u32 frame_width=32, u32 frame_height=32);
    // action api
	void 				pAnimate			(float speed=24.f, BOOL random_playback=FALSE);
	void 				pTimeLimit			(float time_limit);
    // action
    void				pFrameInitExecute	(PAPI::ParticleEffect *effect);
    void				pAnimateExecute		(PAPI::ParticleEffect *effect, float dt);
protected:
	BOOL 				SaveActionList		(IWriter& F);
	BOOL 				LoadActionList		(IReader& F);
public:
						CPEDef				();
                        ~CPEDef				();
    void				SetName				(LPCSTR name);

   	void 				Save				(IWriter& F);
   	BOOL 				Load				(IReader& F);

#ifdef _PARTICLE_EDITOR
	void 				Compile				();
	static PFunction*	FindCommandPrototype(LPCSTR src, LPCSTR& dest);
#endif
};

class ENGINE_API CParticleEffect: public IRender_Visual, IParticleCustom
{
	friend class PFunction;
protected:
	CPEDef*				m_Def;

    s32					m_ElapsedLimit;

	int					m_HandleEffect;
	int					m_HandleActionList;

	s32					m_MemDT;
	
	Fvector				m_InitialPosition;
public:
    enum{
    	flRT_Playing		= (1<<0),
        flRT_DefferedStop	= (1<<1)
    };
    Flags8				m_RT_Flags;
protected:
	void				ResetParticles		();

	BOOL 				SaveActionList		(IWriter& F);
	BOOL 				LoadActionList		(IReader& F);

    void				RefreshShader		();
public:
						CParticleEffect		();
	virtual 			~CParticleEffect	();

	void	 			OnFrame				(u32 dt);

	u32					RenderTO			(FVF::TL* V);
	virtual void		Render				(float LOD);
	virtual void		Copy				(IRender_Visual* pFrom);

    virtual void 		OnDeviceCreate		();
    virtual void 		OnDeviceDestroy		();

    virtual void		UpdateParent		(const Fmatrix& m, const Fvector& velocity);

    BOOL				Compile				(CPEDef* def);

	CPEDef*				GetDefinition		(){return m_Def;}

	virtual void		Play				();
    virtual void		Stop				(BOOL bDefferedStop=TRUE);
    virtual BOOL		IsPlaying			(){return m_RT_Flags.is(flRT_Playing);}
};
DEFINE_VECTOR			(PS::CPEDef*,PEDVec,PEDIt);
}
#define PED_VERSION				0x0001
//----------------------------------------------------
#define PED_CHUNK_VERSION		0x0001
#define PED_CHUNK_NAME			0x0002
#define PED_CHUNK_EFFECTDATA	0x0003
#define PED_CHUNK_ACTIONLIST   	0x0004
#define PED_CHUNK_FLAGS			0x0005
#define PED_CHUNK_FRAME			0x0006
#define PED_CHUNK_SPRITE	   	0x0007
#define PED_CHUNK_TIMELIMIT		0x0008
#define PED_CHUNK_SOURCETEXT   	0x0020

//---------------------------------------------------------------------------
#endif
