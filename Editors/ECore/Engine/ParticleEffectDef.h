//---------------------------------------------------------------------------
#ifndef ParticleEffectDefH
#define ParticleEffectDefH

#ifdef _EDITOR
    #include "FBasicVisual.h"
    #include "ParticleCustom.h"
#else
    #include "..\FBasicVisual.h"
    #include "..\ParticleCustom.h"
#endif

namespace PAPI
{
	struct Particle;
	struct ParticleEffect;
	struct PAHeader;
	struct ParticleAction;
    DEFINE_VECTOR(ParticleAction*,PAVec,PAVecIt);
}
struct EParticleAction;        

namespace PS
{
	class CParticleEffect;

	typedef BOOL (__stdcall * CollisionCallback)(CParticleEffect* E, PAPI::Particle& P, const Fvector& pt, const Fvector& norm); // TRUE-continue collision exec
	typedef void (__stdcall * DestroyCallback)	(CParticleEffect* E, PAPI::Particle& P);

	class PFunction;
	struct SFrame
	{
		Fvector2			m_fTexSize;
		Fvector2			reserved; 
		int     			m_iFrameDimX;
		int 				m_iFrameCount;
		float				m_fSpeed;

		void 				InitDefault()
		{
			m_fTexSize.set	(32.f/256.f,64.f/128.f);
			m_iFrameDimX 	= 8;
			m_iFrameCount 	= 16;
			m_fSpeed		= 24.f;
		}
		IC void       		CalculateTC(int frame, Fvector2& lt, Fvector2& rb)
		{
			lt.x       	 	= (frame%m_iFrameDimX)*m_fTexSize.x;
			lt.y        	= (frame/m_iFrameDimX)*m_fTexSize.y;
			rb.x        	= lt.x+m_fTexSize.x;
			rb.y        	= lt.y+m_fTexSize.y;
		}
	};

	class CPEDef
	{
	public:
		enum{
			dfSprite		= (1<<0),
//			dfObject		= (1<<1),

			dfFramed		= (1<<10),
			dfAnimated		= (1<<11),
			dfRandomFrame   = (1<<12),
			dfRandomPlayback= (1<<13),
            
			dfTimeLimit		= (1<<14),

            dfAlignToPath	= (1<<15),
            dfCollision		= (1<<16),
            dfCollisionDel	= (1<<17),
            dfVelocityScale	= (1<<18),
		};
		ref_str				m_Name;
		Flags32				m_Flags;
	// texture
		ref_str				m_ShaderName;
		ref_str				m_TextureName;
		ref_shader			m_CachedShader;
		SFrame				m_Frame;
	// compiled actions
        CMemoryWriter		m_Actions;
	// def        
		float				m_fTimeLimit;			// time limit
		int					m_MaxParticles;			// max particle count
	    Fvector				m_VelocityScale;		// velocity scale
	    Fvector				m_APDefaultRotation;	// align to path
    // collision
	    float 				m_fCollideOneMinusFriction;
        float 				m_fCollideResilience;
        float 				m_fCollideSqrCutoff; 
	public:
		BOOL 				SaveActionList		(IWriter& F);
		BOOL 				LoadActionList		(IReader& F);
	// execute
		void				ExecuteAnimate		(PAPI::Particle *particles, u32 p_cnt, float dt);
        void				ExecuteCollision	(PAPI::Particle *particles, u32 p_cnt, float dt, CParticleEffect* owner, CollisionCallback cb);
	public:
                            CPEDef				();
                            ~CPEDef				();
        
		void				SetName				(LPCSTR name);
        IC LPCSTR			Name				()const{return *m_Name;}
        void				CreateShader		();
        void				DestroyShader		();

		void 				Save				(IWriter& F);
		BOOL 				Load				(IReader& F);
#ifdef _EDITOR         
// change Copy&Equal if variables changed
	public:
        ref_str				m_OwnerName;
        ref_str				m_ModifName;
        time_t				m_CreateTime;
        time_t				m_ModifTime;
        
	    DEFINE_VECTOR		(EParticleAction*,EPAVec,EPAVecIt);
		EPAVec 				m_EActionList;
	public:
		bool __fastcall 	FindActionByName	(LPCSTR new_name);
		void __fastcall 	CollisionFrictionOnAfterEdit	(PropItem* sender, LPVOID edit_val);
		void __fastcall 	CollisionFrictionOnBeforeEdit	(PropItem* sender, LPVOID edit_val);
		void __fastcall 	CollisionFrictionOnDraw			(PropValue* sender, LPVOID draw_val);
		void __fastcall 	CollisionCutoffOnAfterEdit		(PropItem* sender, LPVOID edit_val);
		void __fastcall 	CollisionCutoffOnBeforeEdit		(PropItem* sender, LPVOID edit_val);
		void __fastcall 	CollisionCutoffOnDraw			(PropValue* sender, LPVOID draw_val);
		void __fastcall 	OnActionEditClick	(PropValue* sender, bool& bDataModified, bool& bSafe);
	    void __fastcall 	OnFrameResize		(PropValue* sender);
	    void __fastcall 	OnShaderChange		(PropValue* sender);
	    void __fastcall 	OnFlagChange		(PropValue* sender);
		void __fastcall 	OnControlClick		(PropValue* sender, bool& bDataModified, bool& bSafe);
		void __fastcall 	OnActionsClick		(PropValue* sender, bool& bDataModified, bool& bSafe);
        void __fastcall 	OnAfterActionNameEdit(PropItem* sender, LPVOID edit_val);
		void				FillProp		   	(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner);
		void				Copy				(const CPEDef& src);
		BOOL				Equal				(const CPEDef* pe);
		void 				Render				(const Fmatrix& parent);
		void 				Compile				();
		static PFunction*	FindCommandPrototype(LPCSTR src, LPCSTR& dest);
#endif
	};
};
//----------------------------------------------------
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
#define PED_CHUNK_TIMELIMIT2	0x0009
#define PED_CHUNK_SOURCETEXT_  	0x0020 // obsolette
#define PED_CHUNK_COLLISION	   	0x0021
#define PED_CHUNK_VEL_SCALE		0x0022
#define PED_CHUNK_OWNER			0x0023
#define PED_CHUNK_EDATA			0x0024
#define PED_CHUNK_ALIGN_TO_PATH	0x0025
//---------------------------------------------------------------------------
#endif
