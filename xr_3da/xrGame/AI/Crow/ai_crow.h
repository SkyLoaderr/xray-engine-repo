////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.h
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_CROW__
#define __XRAY_AI_CROW__

#include "..\..\entity.h"

class CAI_Crow : public CEntityAlive//GameObject
{
//	typedef	CGameObject inherited;
	typedef	CEntityAlive inherited;
	enum ECrowStates 	{
		eDeathFall = 0,
		eDeathDead,
		eFlyIdle,
		eFlyUp
	};
// animations
	struct SAnim{
		#define MAX_ANIM_COUNT 8
		typedef svector<CMotionDef*,MAX_ANIM_COUNT> MotionSVec;
		MotionSVec	m_Animations;
		CMotionDef*	GetRandom	(){return m_Animations[Random.randI(0,m_Animations.size())];}
		void		Load		(CKinematics* visual, LPCSTR prefix);
	};
	struct SSound{
		#define MAX_SND_COUNT 8
		typedef svector<sound,MAX_SND_COUNT> MotionSVec;
		MotionSVec	m_Sounds;
		sound&		GetRandom	(){return m_Sounds[Random.randI(0,m_Sounds.size())];}
		void		Load		(LPCSTR prefix);
		void		SetPosition	(const Fvector& pos);
		void		Unload		();
	};
public:
	void			OnHitEndPlaying(CBlend* B);
protected:
	struct SCrowAnimations{
		SAnim		m_idle;
		SAnim		m_fly;
		SAnim		m_death;
		SAnim		m_death_idle;
		SAnim		m_death_dead;
	};
	SCrowAnimations	m_Anims;
	struct SCrowSounds{
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
public:
					CAI_Crow();
	virtual			~CAI_Crow();
	virtual void	Load			( LPCSTR section );
	virtual BOOL	Spawn			( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
	virtual BOOL	ShadowGenerate	()			{ return FALSE;	}
	virtual BOOL	ShadowReceive	()			{ return FALSE;	}
	virtual void	Update			(DWORD DT);
	virtual void	UpdateCL		();
	virtual void	net_Export		(NET_Packet* P);
	virtual void	net_Import		(NET_Packet* P);

	virtual void	g_fireParams	(Fvector& P, Fvector& D){};
	virtual void	HitSignal		(int HitAmount, Fvector& local_dir, CEntity* who);
	virtual void	HitImpulse		(Fvector& vWorldDir, Fvector& vLocalDir, float amount){};
	virtual void	Die				(){};
	virtual	float	ffGetFov		(){return 150.f;}
	virtual	float	ffGetRange		(){return 30.f;}

	virtual BOOL	IsVisibleForHUD	(){return FALSE;}
};
		
#endif