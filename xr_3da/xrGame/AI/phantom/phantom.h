#pragma once

#include "../../entity.h"
#include "../../fastdelegate.h"
#include "../../../SkeletonAnimated.h"

class CParticlesObject;

class CPhantom : public CEntity {
	
	typedef	CEntity inherited;
private:
	enum EState{
		stIdle			= -1,
		stBirth			= 0,
		stFly			= 1,
		stDeath			= 2,
		stCount
	};
	EState				m_State;

	void				SwitchToState				(EState new_state);
	void __stdcall		OnBirthState				();
	void __stdcall		OnFlyState					();
	void __stdcall		OnDeathState				();

	fastdelegate::FastDelegate0						UpdateEvent;
private:
	struct SStateData{
		shared_str		particles;
		ref_sound		sound;
		MotionID		motion;
	};
	SStateData			m_state_data[stCount];
private:
	CParticlesObject*	m_fly_particles;
	ref_sound			m_fly_sound;
private:
	CObject*			m_enemy;

	float				fDHeading;
	float				fSpeed;	
	float				fASpeed;
	Fvector				vHPB;
	
	Fvector				vCurrentDir;
	Fvector				vGoalDir;
	Fvector				vVarGoal;
	float				fGoalChangeTime;
	float				fGoalChangeDelta;

	Fmatrix				XFORM_center				();

	CParticlesObject*	PlayParticles				(const shared_str& name, BOOL bAutoRemove);
//	ref_sound			PlaySound					(const shared_str& name);
//	void				PlayMotion					(MotionID);

	void				UpdatePosition				(const Fvector& tgt_pos);

	void				PsyHit						(const CObject *object, float value);
public:
						CPhantom					();
	virtual				~CPhantom					();
	
	virtual void		Load						( LPCSTR section );
	virtual BOOL		net_Spawn					( CSE_Abstract* DC );
	virtual void		net_Destroy					();
	
	virtual void		net_Export					(NET_Packet& P);
	virtual void		net_Import					(NET_Packet& P);

	virtual void		shedule_Update				(u32 DT); 
	virtual void		UpdateCL					();

	virtual void		HitSignal					(float	HitAmount,	Fvector& local_dir, CObject* who, s16 element){}
	virtual void		HitImpulse					(float	amount,		Fvector& vWorldDir, Fvector& vLocalDir){}
	virtual void		Hit							(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type);
	virtual void		Die							(CObject* who);

	virtual BOOL		IsVisibleForHUD				() {return false;}
	virtual bool		IsVisibleForZones			() {return false;}

	virtual BOOL		UsedAI_Locations			() {return false;}

	virtual CEntity*	cast_entity					() {return this;}
};

