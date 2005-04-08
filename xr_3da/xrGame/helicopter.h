#pragma once

#include "shootingobject.h"
#include "weaponammo.h"
#include "rocketlauncher.h"
#include "entity.h"
#include "phskeleton.h"
#include "hit_immunity.h"
#include "script_export_space.h"
#include "memory_manager.h"
#include "HudSound.h"

#include "patrol_path.h"
#include "intrusive_ptr.h"

class CScriptGameObject;
class CLAItem;
class CHelicopterMovManager;
class CHelicopter;

enum EHeliHuntState{eEnemyNone,eEnemyPoint,eEnemyEntity};
struct SHeliEnemy{
	EHeliHuntState					type;
	Fvector							destEnemyPos;
	u32								destEnemyID;
	void Update						();
};

enum EHeliBodyState{eBodyByPath,eBodyToPoint};
struct SHeliBodyState{
	EHeliBodyState					type;
	//settings, constants
	float							model_pitch_k;
	float							model_bank_k;
	float							model_angSpeedBank;
	float							model_angSpeedPitch;

	//runtime params
	float							currBodyH;
	float							currBodyP;
	float							currBodyB;

	bool							b_looking_at_point;
	Fvector							looking_point;
	void		LookAtPoint			(Fvector point, bool do_it);
};

enum EHeilMovementState{eMovNone,eMovToPoint,eMovPatrolPath,eMovLanding,eMovTakeOff};
struct SHeliMovementState{
	CHelicopter*					parent;
	EHeilMovementState				type;
	//specified path
	const CPatrolPath*				currPatrolPath;
	const CPatrolPath::CVertex*		currPatrolVertex;
	
	int								patrol_begin_idx;
	shared_str						patrol_path_name;
	bool							need_to_del_path;
	bool							need_to_call_callback;

	float							maxLinearSpeed;
	float							LinearAcc_fw;
	float							LinearAcc_bk;
	float							angularSpeedPitch;
	float							angularSpeedHeading;
	float							speedInDestPoint;
//runtime values
	Fvector							desiredPoint;

	float							curLinearSpeed;
	float							curLinearAcc;

	Fvector							currP;
	float							currPathH;
	float							currPathP;


	Fvector							round_center;
	float							round_radius;
	bool							round_reverse;

	float							onPointRangeDist;

	void	Update						();
	void	UpdateMovToPoint			();
	void	UpdatePatrolPath			();
	bool	AlreadyOnPoint				();
	void	goByRoundPath				(Fvector center, float radius, bool clockwise);
	float	GetDistanceToDestPosition	();
	void	getPathAltitude				(Fvector& point, float base_altitude);
	void	SetDestPosition				(Fvector* pos);
	void	goPatrolByPatrolPath		(LPCSTR path_name,int start_idx);
};

class CHelicopter : 	public CEntity,
						public CShootingObject,
						public CRocketLauncher,
						public CPHSkeleton,
						public CHitImmunity
					#ifdef DEBUG
						,public pureRender
					#endif

{
	typedef CPhysicsShellHolder inherited;
public:
	enum EHeliState {
		eAlive							= u32(0),
		eDead,
		eForce = u32(-1)
	}; 


// heli weapons
	bool							m_use_rocket_on_attack;
	bool							m_use_mgun_on_attack;
	float							m_min_rocket_dist,m_max_rocket_dist;
	float							m_min_mgun_dist, m_max_mgun_dist;
	u32								m_time_between_rocket_attack;
	bool							m_syncronize_rocket;

	HUD_SOUND						m_sndShot;
	HUD_SOUND						m_sndShotRocket;

	Fvector							m_fire_dir,m_fire_pos;

	u16								m_left_rocket_bone, m_right_rocket_bone, m_fire_bone, m_rotate_x_bone, m_rotate_y_bone;

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

	shared_str						m_sAmmoType, m_sRocketSection;
	CCartridge						m_CurrentAmmo;

	Fmatrix							m_left_rocket_bone_xform, m_right_rocket_bone_xform;

	static void __stdcall			BoneMGunCallbackX		(CBoneInstance *B);
	static void __stdcall			BoneMGunCallbackY		(CBoneInstance *B);
	void							startRocket(u16 idx);

	//CShootingObject
	virtual const Fmatrix&			ParticlesXFORM		()const					{return m_fire_bone_xform;};

	virtual const Fvector&			CurrentFirePoint	()						{return m_fire_pos;};

	void							MGunFireStart		();
	void							MGunFireEnd			();
	void							MGunUpdateFire		();
	virtual	void					OnShot				();

	void							UpdateMGunDir		();
	void							UpdateWeapons		();

protected:
	SHeliEnemy						m_enemy;
	SHeliBodyState					m_body;
	SHeliMovementState				m_movement;

// on death...
	Fvector							m_death_ang_vel;
	float							m_death_lin_vel_k;
	shared_str						m_death_bones_to_hide;

//////////////////////////////////////////////////

// sound, light, particles...
	ref_sound						m_engineSound;
	ref_sound						m_explodeSound;
	ref_light						m_light_render;
	CLAItem*						m_lanim;
	u16								m_light_bone, m_smoke_bone;
	float							m_light_range, m_light_brightness;
	Fcolor							m_light_color;
	shared_str						m_smoke_particle, m_explode_particle;
	CParticlesObject*				m_pParticle;
	Fmatrix							m_particleXFORM;

	void							StartFlame					();
	void							UpdateHeliParticles			();
	void							Explode						();
	void							DieHelicopter				();
	void							TurnLighting				(bool bOn);
	void							TurnEngineSound				(bool bOn);


//general
	EHeliState						m_curState;

	xr_map<s16,float>				m_hitBones;
	typedef xr_map<s16,float>::iterator bonesIt;
	float							m_stepRemains;

	void	UpdateState					();
public:



	CHelicopter();
	virtual							~CHelicopter();
	
	CHelicopter::EHeliState			state		()		{return m_curState;};
	int								state_script()		{return m_curState;};

	void							setState	(CHelicopter::EHeliState s);
	void							setState_script	(u32 s)					{setState((CHelicopter::EHeliState)s);};

	void							init		();
	virtual	void					reinit		()							{inherited::reinit	();};

	virtual	void					Load				(LPCSTR		section);
	virtual	void					reload				(LPCSTR		section);

	virtual BOOL					net_Spawn			(CSE_Abstract*		DC);
	virtual void					net_Destroy			();
	virtual void					net_Export			(NET_Packet &P){};
	virtual void					net_Import			(NET_Packet &P){};

	virtual void					SpawnInitPhysics	(CSE_Abstract	*D);
	virtual CPhysicsShellHolder*	PPhysicsShellHolder	()						{return PhysicsShellHolder();}
	virtual void					net_Save			(NET_Packet& P);
	virtual	BOOL					net_SaveRelevant	()						{return (inherited::net_SaveRelevant() || BOOL(PPhysicsShell()!=NULL));};					

	virtual void					renderable_Render				()			{ inherited::renderable_Render();};
	virtual BOOL					renderable_ShadowGenerate		()			{ return FALSE;	}
	virtual BOOL					renderable_ShadowReceive		()			{ return TRUE;	}

	virtual void					OnEvent				(NET_Packet& P, u16 type);
	virtual void					UpdateCL			();
	virtual void					shedule_Update		(u32		time_delta);
			void					MoveStep			();

	virtual	void					Hit					(float P, Fvector &dir, CObject* who, s16 element, Fvector position_in_bone_space, float impulse,  ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void					PHHit				(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type);
	//CEntity
	virtual void					HitSignal			(float P, Fvector &local_dir,	CObject* who, s16 element){;}
	virtual void					HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir){;}
	
	virtual const Fmatrix&			get_ParticlesXFORM	();
	virtual const Fvector&			get_CurrentFirePoint();



public:
	//for scripting
	bool					isVisible						(CScriptGameObject* O);
	bool					isObjectVisible					(CObject* O);
	bool			 		isOnAttack						()				{return m_enemy.type!=eEnemyNone;}

	void					goPatrolByPatrolPath			(LPCSTR path_name,int start_idx);
	void					goByRoundPath					(Fvector center, float radius, bool clockwise);
	void					LookAtPoint						(Fvector point, bool do_it);
	void					SetDestPosition					(Fvector* pos);
	float					GetDistanceToDestPosition		();
	
	void					SetSpeedInDestPoint				(float sp);
	float					GetSpeedInDestPoint				(float sp);
	
	void					SetOnPointRangeDist				(float d);
	float					GetOnPointRangeDist				();

	float					GetCurrVelocity					();
	float					GetMaxVelocity					();
	void					SetMaxVelocity					(float v);

	void					SetEnemy						(CScriptGameObject* e);
	void					SetEnemy						(Fvector* pos);
	void					UnSetEnemy						();

	float					GetRealAltitude					();

	int						GetMovementState				();
	int						GetHuntState					();
	int						GetBodyState					();

//	virtual float			GetfHealth						() const;
//	virtual float			SetfHealth						(float value);

#ifdef DEBUG
public:
	virtual void			OnRender						();
#endif

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CHelicopter)
#undef script_type_list
#define script_type_list save_type_list(CHelicopter)