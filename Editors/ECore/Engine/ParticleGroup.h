//---------------------------------------------------------------------------
#ifndef ParticleGroupH
#define ParticleGroupH

#include "..\ParticleCustom.h"

namespace PS
{
	class CParticleEffect;

	class CPGDef
	{
	public:
		ref_str				m_Name;
		Flags32				m_Flags;
		float				m_fTimeLimit;
		struct SEffect{
			enum{
				flDefferedStop	= (1<<0),
				flChild			= (1<<1),
				flEnabled		= (1<<2),
			};
			Flags32			m_Flags;
			ref_str			m_EffectName;  
			ref_str			m_ChildEffectName;
			float			m_Time0;
			float			m_Time1;
							SEffect				(){m_Flags.set(flEnabled);m_Time0=0;m_Time1=0;}
#ifdef _EDITOR
            BOOL			Equal				(const SEffect&);
#endif
		};
		DEFINE_VECTOR(SEffect,EffectVec,EffectIt);
		EffectVec			m_Effects;
#ifdef _EDITOR
        ref_str				m_OwnerName;
        ref_str				m_ModifName;
        time_t				m_CreateTime;
        time_t				m_ModifTime;
// change Equal if variables changed
		void __fastcall 	OnEffectsEditClick	(PropValue* sender, bool& bDataModified, bool& bSafe);
		void __fastcall 	OnEffectTypeChange	(PropValue* sender);
		void __fastcall 	OnEffectEditClick	(PropValue* sender, bool& bDataModified, bool& bSafe);
		void __fastcall 	OnControlClick	(PropValue* sender, bool& bDataModified, bool& bSafe);
		void __fastcall 	OnParamsChange	(PropValue* sender);
		void				FillProp	   	(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner);
		BOOL				Equal			(const CPGDef* pe);
#endif
	public:
		CPGDef		  	();
		~CPGDef		  	();
		void				SetName		  	(LPCSTR name);

		void 				Save		  	(IWriter& F);
		BOOL 				Load		 	(IReader& F);
	};
	DEFINE_VECTOR(CPGDef*,PGDVec,PGDIt);

	class CParticleGroup: public IParticleCustom
	{
		const CPGDef*		m_Def;
		float				m_CurrentTime;
		Fvector				m_InitialPosition;
	public:
    	DEFINE_VECTOR(IRender_Visual*,VisualVec,VisualVecIt);
    	class SItem{
        	IRender_Visual*	effect;
            VisualVec		children;
            VisualVec		children_stopped;
        public:
        	void			Set				(IRender_Visual* e);
            void			Clear			();

            IC u32			GetVisuals		(xr_vector<IRender_Visual*>& visuals)
            {
            	visuals.reserve(children.size()+children_stopped.size()+1);
                visuals.push_back(effect);
			    VisualVecIt it;
                for (it=children.begin(); it!=children.end(); it++) 				visuals.push_back(*it);
                for (it=children_stopped.begin(); it!=children_stopped.end(); it++)	visuals.push_back(*it);
                return visuals.size();
            }
            
            void			OnDeviceCreate	();
            void			OnDeviceDestroy	();

            void			AppendChild		(LPCSTR eff_name);
            void			RemoveChild		(u32 idx);

            void 			UpdateParent	(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM);
            void			OnFrame			(u32 u_dt, Fbox& box, bool& bPlaying);

            u32				ParticlesCount	();
            BOOL			IsPlaying		();
            void			Play			();
            void			Stop			(BOOL def_stop);
        };
        DEFINE_VECTOR(SItem,SItemVec,SItemVecIt)
		SItemVec			items;
	public:
		enum{
			flRT_Playing		= (1<<0),
			flRT_DefferedStop	= (1<<1),
		};
		Flags8				m_RT_Flags;
	public:
		CParticleGroup	();
		virtual				~CParticleGroup	();
		virtual void	 	OnFrame			(u32 dt);

		virtual void		Copy			(IRender_Visual* pFrom) {Debug.fatal("Can't duplicate particle system - NOT IMPLEMENTED");}

		virtual void 		OnDeviceCreate	();
		virtual void 		OnDeviceDestroy	();

		virtual void		UpdateParent	(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM);

		BOOL				Compile			(CPGDef* def);

		const CPGDef*		GetDefinition	(){return m_Def;}

		virtual void		Play			();
		virtual void		Stop			(BOOL bDefferedStop=TRUE);
		virtual BOOL		IsPlaying		(){return m_RT_Flags.is(flRT_Playing);}

		virtual float		GetTimeLimit	(){VERIFY(m_Def); return m_Def->m_fTimeLimit;}

		virtual const ref_str	Name		(){VERIFY(m_Def); return m_Def->m_Name;}

        virtual u32 		ParticlesCount	();
	};

}
#define PGD_VERSION				0x0003
//----------------------------------------------------
#define PGD_CHUNK_VERSION		0x0001
#define PGD_CHUNK_NAME			0x0002
#define PGD_CHUNK_FLAGS			0x0003
#define PGD_CHUNK_EFFECTS		0x0004 // obsolette
#define PGD_CHUNK_TIME_LIMIT	0x0005
#define PGD_CHUNK_OWNER			0x0006
#define PGD_CHUNK_EFFECTS2		0x0007

//---------------------------------------------------------------------------
#endif
