#pragma once

#include "HelicopterMovementManager.h"
#include "shootingobject.h"
#include "weaponammo.h"
#include "rocketlauncher.h"
#include "entity.h"
#include "phskeleton.h"

#include "script_export_space.h"
class CScriptGameObject;
class CLAItem;
struct SHeliShared{
	CObject*						m_destEnemy;
	Fvector							m_destEnemyPos; //lastEnemyPos
	Fvector							m_stayPoint;

	float							m_pitch_k;
	float							m_bank_k;
	float							m_time_delay_between_patrol;
	float							m_time_patrol_period;
	float							m_alt_korridor;
	float							m_baseAltitude;
	float							m_attackAltitude;
	float							m_basePatrolSpeed;
	float							m_baseAttackSpeed;
	float							m_hunt_dist;
	float							m_hunt_time;
	Fvector							m_to_point;//tmp, fix it
	Fvector							m_via_point;//tmp, fix it

	float							m_wait_in_point;//tmp, fix it
	float							m_maxKeyDist;
	float							m_endAttackTime;
	Fvector							m_startDir;
	float							m_time_last_patrol_end;
	float							m_time_last_patrol_start;
	float							m_time_delay_before_start;

	Fmatrix							m_desiredXFORM;
	Fvector							m_desiredP;
	Fvector							m_desiredR;


	float							m_wrk_altitude;

	int								m_patrol_begin_idx;
	ref_str							m_patrol_path_name;
	const CPatrolPath*				m_currPatrolPath;
	const CPatrolPath::CVertex*		m_currPatrolVertex;
};

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
		eInitiatePatrolByPath,
		eMovingByPatrolPath,
		eDead,
		eForce = u32(-1)
	}; 
	enum EHeliEvents{
	EV_ON_POINT,
	EV_ON_HIT,
	EV_FORCE32 = u32(-1)
	};
	SHeliShared						m_data;

	bool							m_use_rocket_on_attack;
	bool							m_use_mgun_on_attack;
	float							m_min_rocket_dist;
	float							m_max_rocket_dist;
	float							m_min_mgun_dist;
	float							m_max_mgun_dist;
	u32								m_time_between_rocket_attack;
	bool							m_syncronize_rocket;
	float							m_on_point_range_dist;

protected:
	float							m_maxLinearSpeed;

	float							m_curLinearSpeed;//m/s
	float							m_curLinearAcc;
	float							m_LinearAcc_fw;
	float							m_LinearAcc_bk;
	float							m_angularSpeedPitch;
	float							m_angularSpeedHeading;

	float							m_model_angSpeedBank;
	float							m_model_angSpeedPitch;

	Fvector							m_currP;
	Fvector							m_currR;
	float							m_currPathH;
	float							m_currPathP;

	float							m_currBodyH;
	float							m_currBodyP;
	float							m_currBodyB;

	Fvector							m_death_ang_vel;
	float							m_death_lin_vel_k;
//////////////////////////////////////////////////
	EHeliState						m_curState;


	HitTypeSVec						m_HitTypeK;

	ref_sound						m_engineSound;
	ref_sound						m_explodeSound;
	IRender_Light*					m_light_render;
	CLAItem*						m_lanim;
	u16								m_light_bone;
	float							m_light_brightness;

	CHelicopterMovManager			m_movMngr;

	xr_map<s16,float>				m_hitBones;

	u16								m_left_rocket_bone;
	u16								m_right_rocket_bone;
	u16								m_fire_bone;
	u16								m_rotate_x_bone;
	u16								m_rotate_y_bone;

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
	u16								m_last_launched_rocket;
	u32								m_last_rocket_attack;


	ref_str							m_sAmmoType;
	ref_str							m_sRocketSection;
	CCartridge						m_CurrentAmmo;

	ref_str							m_smoke_particle;
	ref_str							m_explode_particle;
	CParticlesObject*				m_pParticle;
	Fmatrix							m_particleXFORM;
	u16								m_smoke_bone;

	ref_str							m_death_bones_to_hide;
	static void __stdcall	BoneMGunCallbackX		(CBoneInstance *B);
	static void __stdcall	BoneMGunCallbackY		(CBoneInstance *B);

	typedef xr_map<s16,float>::iterator bonesIt;

	void							startRocket(u16 idx);

public:
	Fmatrix							m_left_rocket_bone_xform;
	Fmatrix							m_right_rocket_bone_xform;


	CHelicopter();
	virtual							~CHelicopter();
	
	CHelicopter::EHeliState			state		()		{return m_curState;};
	u32								state_script()		{return m_curState;};

	void							setState	(CHelicopter::EHeliState s);
	void							setState_script	(u32 s){setState((CHelicopter::EHeliState)s);};
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
	
			void			PrepareDie			();
			void			UpdateHeliParticles	();
			void			Explode				();

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

	void					goPatrolByPatrolPath (LPCSTR path_name,int start_idx);
	
	float					GetDistanceToDestPosition();
	void					SetDestPosition (Fvector* pos);
	float					GetCurrAltitude();
	void					SetCurrAltitude(float a);
	float					GetCurrVelocity();
	void					SetCurrVelocity(float v);
	void					SetEnemy(CScriptGameObject* e);
	float					GetRealAltitude();

	virtual float GetfHealth() const;
	virtual float SetfHealth(float value);
			void   TurnLighting(bool bOn);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CHelicopter)
#undef script_type_list
#define script_type_list save_type_list(CHelicopter)