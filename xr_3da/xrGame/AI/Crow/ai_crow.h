////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.h
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_CROW__
#define __XRAY_AI_CROW__

#include "../../entity_alive.h"
class CAI_Crow : public CEntityAlive
{
	typedef	CEntityAlive inherited;
	enum ECrowStates 	{
		eUndef		= -1,
		eDeathFall	= 0,
		eDeathDead,
		eFlyIdle,
		eFlyUp
	};

	// constants and types
	enum			{ MAX_ANIM_COUNT = 8	};
	enum			{ MAX_SND_COUNT = 8		};

	// animations
	struct SAnim
	{
		typedef			svector<CMotionDef*,MAX_ANIM_COUNT> MotionSVec;
		MotionSVec		m_Animations;
		CMotionDef*		GetRandom	(){return m_Animations[Random.randI(0,m_Animations.size())];}
		void			Load		(CSkeletonAnimated* visual, LPCSTR prefix);
	};

	struct SSound
	{
		typedef			svector<ref_sound,MAX_SND_COUNT>		MotionSVec;
		MotionSVec		m_Sounds;
		ref_sound&		GetRandom	(){return m_Sounds[Random.randI(0,m_Sounds.size())];}
		void			Load		(LPCSTR prefix);
		void			SetPosition	(const Fvector& pos);
		void			Unload		();
	};
public:
	void				OnHitEndPlaying(CBlend* B);
protected:
	
	struct SCrowAnimations
	{
		SAnim		m_idle;
		SAnim		m_fly;
		SAnim		m_death;
		SAnim		m_death_idle;
		SAnim		m_death_dead;
	};
	SCrowAnimations	m_Anims;
	struct SCrowSounds
	{
		SSound		m_idle;
	};
	SCrowSounds		m_Sounds;

	Fvector			vOldPosition;
	ECrowStates		st_current, st_target;
	// parameters block
	Fvector			vGoalDir;
	Fvector			vCurrentDir;
	Fvector			vHPB;
	float			fDHeading;

	// constants
	float			fGoalChangeDelta;
	float			fSpeed;
	float			fASpeed;
	float			fMinHeight;
	Fvector			vVarGoal;
	float			fIdleSoundDelta;

	// variables
	float			fGoalChangeTime;
	float			fIdleSoundTime;

	void			switch2_FlyUp	();
	void			switch2_FlyIdle	();
	void			switch2_DeathFall();
	void			switch2_DeathDead();

	void			state_DeathFall	();
	void			state_Flying	();

	void			CreateSkeleton();
	
public:
					CAI_Crow();
	virtual			~CAI_Crow();
	virtual void	Load			( LPCSTR section );
			void	Init			();
	virtual BOOL	net_Spawn		( LPVOID DC );
	virtual void	net_Destroy		();
	virtual BOOL	renderable_ShadowGenerate	()			{ return FALSE;	}
	virtual BOOL	renderable_ShadowReceive	()			{ return FALSE;	}
	virtual void	shedule_Update	(u32 DT);
	virtual void	UpdateCL		();

	virtual void	net_Export		(NET_Packet& P);
	virtual void	net_Import		(NET_Packet& P);

	virtual void	g_fireParams	(const CHudItem* /**pHudItem/**/, Fvector& /**P/**/, Fvector& /**D/**/)	{};
	virtual void	g_WeaponBones	(int &/**L/**/, int &/**R1/**/, int &/**R2/**/)	{};

	virtual void	HitSignal		(float	HitAmount,	Fvector& local_dir, CObject* who, s16 element);
	virtual void	HitImpulse		(float	amount,		Fvector& vWorldDir, Fvector& vLocalDir);
	virtual void	Hit				(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type);
	virtual void	Die				(){inherited::Die();CreateSkeleton();};
	virtual	float	ffGetFov		()const {return 150.f;}
	virtual	float	ffGetRange		()const {return 30.f;}

	virtual BOOL	IsVisibleForHUD	()	{ return FALSE;		}
	virtual bool	IsVisibleForZones() { return false;		}
	virtual BOOL	UsedAI_Locations();
	virtual void	create_physic_shell	();
};

#endif