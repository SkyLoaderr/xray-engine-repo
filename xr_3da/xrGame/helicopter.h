#pragma once

#include "HelicopterMovementManager.h"
#include "shootingobject.h"
#include "weaponammo.h"
#include "rocketlauncher.h"
#include "entity.h"
#include "phskeleton.h"


class CHelicopter : 
	public CEntity,
	public CShootingObject,
	public CRocketLauncher,
	public CPHSkeleton
{
	typedef CPhysicsShellHolder inherited;
public:
	enum EHeliState {
		eIdleState							= u32(0),
		eInitiatePatrolZone,				// ->eMovingByPatrolZonePath
		eMovingByPatrolZonePath,			//->eInitiatePatrolZone, eInitiateWaitBetweenPatrol
		eInitiateHunt,						// ->eMovingByAttackTraj,eInitiatePatrolZone
		eInitiateHunt2,						// ->eMovingByAttackTraj,eInitiatePatrolZone
		eMovingByAttackTraj,				// ->eInitiatePatrolZone
		eWaitForStart,						// ->eInitiatePatrolZone
		eWaitBetweenPatrol,					// ->eInitiatePatrolZone
		eInitiateWaitBetweenPatrol,			// ->eMovingToWaitPoint
		eMovingToWaitPoint,					// ->eWaitBetweenPatrol
		eInitiateGoToPoint,					// ->eMovingByPatrolZonePath
		eMovingToPoint,
		eDead
	}; 
protected:
	
	EHeliState						m_curState;


	HitTypeSVec						m_HitTypeK;

	ref_sound						m_engineSound;

	CHelicopterMovManager			m_movMngr;

	xr_map<s16,float>				m_hitBones;

	u16								m_left_rocket_bone;
	u16								m_right_rocket_bone;

	Fvector							m_fire_pos;
	Fvector							m_fire_dir;
	Fmatrix							m_fire_bone_xform;
	Fmatrix							m_i_bind_x_xform, m_i_bind_y_xform;
	Fvector2						m_lim_x_rot, m_lim_y_rot;
	float							m_tgt_x_rot, m_tgt_y_rot;
	float							m_cur_x_rot, m_cur_y_rot;
	float							m_bind_x_rot, m_bind_y_rot;
	Fvector							m_bind_x, m_bind_y;
	bool							m_allow_fire;
	bool							m_use_rocket_on_attack;
	bool							m_use_mgun_on_attack;
	u16								m_last_launched_rocket;
	float							m_min_rocket_dist;
	float							m_max_rocket_dist;

	float							m_min_mgun_dist;
	float							m_max_mgun_dist;

	u32								m_time_between_rocket_attack;
	u32								m_last_rocket_attack;
	bool							m_syncronize_rocket;

	u16								m_fire_bone;
	u16								m_rotate_x_bone;
	u16								m_rotate_y_bone;

	ref_str							m_sAmmoType;
	ref_str							m_sRocketSection;
	CCartridge						m_CurrentAmmo;

	CObject*						m_destEnemy;
	Fvector							m_destEnemyPos;



	static void __stdcall	BoneMGunCallbackX		(CBoneInstance *B);
	static void __stdcall	BoneMGunCallbackY		(CBoneInstance *B);

	typedef xr_map<s16,float>::iterator bonesIt;

	void							startRocket(u16 idx);

public:
	Fmatrix							m_left_rocket_bone_xform;
	Fmatrix							m_right_rocket_bone_xform;
	Fvector							m_stayPos;


	CHelicopter();
	virtual							~CHelicopter();
	
	CHelicopter::EHeliState			state		()		{return m_curState;};

	void							setState	(CHelicopter::EHeliState s);
	const Fvector&					lastEnemyPos()		{return m_destEnemyPos;};
	//CAI_ObjectLocation
	void							init		();
	virtual	void					reinit		();

	//CGameObject
	virtual	void			Load				(LPCSTR		section);
	virtual	void			reload				(LPCSTR		section);

	virtual BOOL			net_Spawn			(LPVOID		DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet &P);
	virtual void			net_Import			(NET_Packet &P);

	virtual void			SpawnInitPhysics	(CSE_Abstract	*D);
	virtual CPhysicsShellHolder*	PPhysicsShellHolder	()	{return PhysicsShellHolder();}
	virtual void			net_Save			(NET_Packet& P);
	virtual	BOOL			net_SaveRelevant	();					

	virtual void			renderable_Render				();
	virtual BOOL			renderable_ShadowGenerate		()	{ return FALSE;	}
	virtual BOOL			renderable_ShadowReceive		()	{ return TRUE;	}

	virtual void			OnEvent				(NET_Packet& P, u16 type);
	virtual void			UpdateCL			();
	virtual void			shedule_Update		(u32		time_delta);

	virtual	void			Hit					(float P, Fvector &dir, CObject* who, s16 element, Fvector position_in_bone_space, float impulse,  ALife::EHitType hit_type = ALife::eHitTypeWound);

	//CEntity
	virtual void			HitSignal			(float P, Fvector &local_dir,	CObject* who, s16 element){;}
	virtual void			HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir){;}
	virtual void			Die					();

protected:
	//CShootingObject
	virtual const Fmatrix&	ParticlesXFORM		()const;
	virtual IRender_Sector*	Sector				();
	virtual const Fvector&	CurrentFirePoint	();

	void					MGunFireStart		();
	void					MGunFireEnd			();
	void					MGunUpdateFire		();
	virtual	void			OnShot				();

	void					updateMGunDir		();

public:
	//for scripting
	void					doHunt				(CObject* dest);
	void					doHunt2				(CObject* dest, float dist=20.0f, float time=5.0f);
	bool			 		isOnAttack			()					{return (m_curState==eInitiateHunt || m_curState==eInitiateHunt2 || m_curState==eMovingByAttackTraj) ;}
	void					useRocket			(bool b)			{m_use_rocket_on_attack = b;};
	bool					useRocket			()const				{return m_use_rocket_on_attack;};
	void					useMGun				(bool b)			{m_use_mgun_on_attack = b;};
	bool					useMGun				()const				{return m_use_mgun_on_attack;};
	void					gotoStayPoint		(float time=0.0f, Fvector* pos = 0);
	void					goPatrol			(float time=0.0f);
	void					goToPoint			(Fvector* to, Fvector* via, float time);
	float					getLastPointTime	();

	void					goPatrolByPatrolPath (LPCSTR path_name);
};
