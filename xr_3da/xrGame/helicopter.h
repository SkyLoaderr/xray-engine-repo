#pragma once

#include "gameobject.h"
#include "HelicopterMovementManager.h"
#include "shootingobject.h"
#include "weaponammo.h"
#include "rocketlauncher.h"

#define HELI_HUNT_RADIUS 30.0f
//#define HELI_MAX_ANGULAR_VELOCITY PI
//#define HELI_MIN_ANGULAR_VELOCITY 0.0f
#define HELI_PITCH_K (-0.006f)
#define HELI_VELOCITY_ROLL_K (0.03f)

class CHelicopter : 
	public CEntity,
	public CShootingObject,
	public CRocketLauncher

{
	typedef CGameObject inherited;
public:
	enum EHeliState {
		eIdleState							= u32(0),
		eInitiatePatrolZone,				// ->eMovingByPatrolZonePath
		eMovingByPatrolZonePath,			//->eInitiatePatrolZone, eInitiateWaitBetweenPatrol
		eInitiateHunt,						// ->eMovingByAttackTraj,eInitiatePatrolZone
		eMovingByAttackTraj,				// ->eInitiatePatrolZone
		eWaitForStart,						// ->eInitiatePatrolZone
		eWaitBetweenPatrol,					// ->eInitiatePatrolZone
		eInitiateWaitBetweenPatrol,			// ->eMovingToWaitPoint
		eMovingToWaitPoint,					// ->eWaitBetweenPatrol
		eDead
	}; 
protected:
	
	EHeliState						m_curState;
	float							m_velocity;
	float							m_altitude;

	HitTypeSVec						m_HitTypeK;

	ref_sound						m_engineSound;
	CHelicopterMovementManager		m_movementMngr;

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
	BOOL							m_allow_fire;
	BOOL							m_use_rocket_on_attack;

	u16								m_fire_bone;
	u16								m_rotate_x_bone;
	u16								m_rotate_y_bone;

	ref_str							m_sAmmoType;
	ref_str							m_sRocketSection;
	CCartridge						m_CurrentAmmo;

	CObject*						m_destEnemy;
	Fvector							m_destEnemyPos;

	u16								m_last_launched_rocket;


	static void __stdcall	BoneMGunCallbackX		(CBoneInstance *B);
	static void __stdcall	BoneMGunCallbackY		(CBoneInstance *B);

	typedef xr_map<s16,float>::iterator bonesIt;
public:
	Fmatrix							m_left_rocket_bone_xform;
	Fmatrix							m_right_rocket_bone_xform;

	float							m_attack_altitude;
	float							m_min_rocket_dist;
	float							m_max_rocket_dist;
	u32								m_time_between_rocket_attack;
	u32								m_last_rocket_attack;
	BOOL							m_syncronize_rocket;

	float							m_korridor;
	float							m_x_level_bound;
	float							m_z_level_bound;
	u32								m_time_delay_before_start;
	u32								m_time_patrol_period;
	u32								m_time_delay_between_patrol;
	u32								m_time_last_patrol_start;
	u32								m_time_last_patrol_end;
	Fvector							m_stayPos;

	CHelicopter();
	virtual							~CHelicopter();
	
	CHelicopter::EHeliState			state(){return m_curState;};
	float							velocity(){return m_velocity;};
	float							altitude(){return m_altitude;};

	void							setState(CHelicopter::EHeliState s);
	Fvector&						lastEnemyPos(){return m_destEnemyPos;};
	void							startRocket(u16 idx);
	//CAI_ObjectLocation
	void					init();
	virtual	void			reinit();


	//CGameObject
	virtual	void			Load				(LPCSTR		section);
	virtual	void			reload				(LPCSTR		section);

	virtual BOOL			net_Spawn			(LPVOID		DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet &P);
	virtual void			net_Import			(NET_Packet &P);

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
	virtual const Fmatrix&	ParticlesXFORM()	 const;
	virtual IRender_Sector*	Sector();
	virtual const Fvector&	CurrentFirePoint();

	virtual	void			FireStart	();
	virtual	void			FireEnd		();
	virtual	void			UpdateFire	();
	virtual	void			OnShot		();

	void					updateMGunDir();
	void					doHunt(CObject* dest);
};
