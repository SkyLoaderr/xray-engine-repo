// Actor.h: interface for the CActor class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
#define AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_
#pragma once

#include "entity_alive.h"
#include "actor_flags.h"
#include "../feel_touch.h"
#include "PHMovementControl.h"
#include "PhysicsShell.h"
#include "InventoryOwner.h"
#include "ActorCondition.h"
#include "damage_manager.h"
#include "material_manager.h"

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CBoneInstance;
class ENGINE_API CMotionDef;
class ENGINE_API CBlend;
class CWeaponList;
class CEffectorBobbing;
class CTargetCS;
class CCar;

class CActor: 
	public CEntityAlive, 
	public Feel::Touch,
	public CInventoryOwner,
	public CActorCondition,
	public CDamageManager,
	public CMaterialManager
#ifdef DEBUG
	,public pureRender
#endif
{
private:
	typedef CEntityAlive	inherited;
protected:
	enum ESoundCcount {
		SND_HIT_COUNT=4,
		SND_DIE_COUNT=4
	};
	enum EActorCameras {
		eacFirstEye		= 0,
		eacLookAt,
		eacFreeLook,
		eacMaxCam
	};
	enum EDamages {DAMAGE_FX_COUNT = 12};
public:
	
	//CPHMovementControl m_PhysicMovementControl;

	enum EMoveCommand
	{
		mcFwd		= (1ul<<0ul),
		mcBack		= (1ul<<1ul),
		mcLStrafe	= (1ul<<2ul),
		mcRStrafe	= (1ul<<3ul),
		mcCrouch	= (1ul<<4ul),
		mcAccel		= (1ul<<5ul),
		mcTurn		= (1ul<<6ul),
		mcJump		= (1ul<<7ul),
		mcFall		= (1ul<<8ul),
		mcLanding	= (1ul<<9ul),
		mcLanding2	= (1ul<<10ul),
		mcClimb		= (1ul<<11ul),

		mcAnyMove	= (mcFwd|mcBack|mcLStrafe|mcRStrafe)
	};
protected:
	// Weapons and Items
	//CWeaponList*			Weapons;
	CTargetCS*				m_pArtifact;

	// Death
	float					hit_slowmo;
	float					hit_factor;
	bool					bDeathInit;
//	u16						self_gmtl_id;
//	u16						last_gmtl_id;
	//
	//Death physics			
	///CPhysicsShell*			m_phSkeleton;

	//vehicle (not luxury)
	CCar*					m_vehicle;

	// media
	BOOL					bStep;
	ref_sound					sndLanding;
	ref_sound					sndZoneHeart;
	ref_sound					sndZoneDetector;
	ref_sound					sndHit[SND_HIT_COUNT];
	ref_sound					sndDie[SND_DIE_COUNT];

	struct					SActorMotions
	{
		struct				SActorState
		{
			struct			SAnimState
			{
				CMotionDef*	legs_fwd;
				CMotionDef*	legs_back;
				CMotionDef*	legs_ls;
				CMotionDef*	legs_rs;
				void		Create(CSkeletonAnimated* K, LPCSTR base0, LPCSTR base1);
			};
			struct			STorsoWpn{
				CMotionDef*	aim;
				CMotionDef*	holster;
				CMotionDef*	draw;
				CMotionDef*	drop;
				CMotionDef*	reload;
				CMotionDef*	attack;
				CMotionDef*	fire_idle;
				CMotionDef*	fire_end;
				void		Create(CSkeletonAnimated* K, LPCSTR base0, LPCSTR base1);
			};

			CMotionDef*		legs_idle;
			CMotionDef*		jump_begin;
			CMotionDef*		jump_idle;
			CMotionDef*		landing[2];
			CMotionDef*		legs_turn;
			CMotionDef*		death;
			SAnimState		m_walk;
			SAnimState		m_run;
			STorsoWpn		m_torso[7];
			CMotionDef*		m_torso_idle;

			CMotionDef*		m_damage[DAMAGE_FX_COUNT];

			void			Create(CSkeletonAnimated* K, LPCSTR base);
		};
		CMotionDef*			m_steering_torso_left;
		CMotionDef*			m_steering_torso_right;
		CMotionDef*			m_steering_torso_idle;
		CMotionDef*			m_steering_legs_idle;
		CMotionDef*			m_dead_stop;

		SActorState			m_normal;
		SActorState			m_crouch;
		SActorState			m_climb;
		void				Create(CSkeletonAnimated* K);
	};
	BOOL					m_bAnimTorsoPlayed;
	static void				AnimTorsoPlayCallBack(CBlend* B)
	{
		CActor* actor		= (CActor*)B->CallbackParam;
		actor->m_bAnimTorsoPlayed = FALSE;
	}
public:
	// animation
	SActorMotions			m_anims;

	CBlend*					m_current_legs_blend;
	CBlend*					m_current_torso_blend;
	CBlend*					m_current_jump_blend;
	CMotionDef*				m_current_legs;
	CMotionDef*				m_current_torso;

protected:
	// skeleton
	static	float			skel_density_factor;
	static	float			skel_airr_lin_factor;
	static	float			skel_airr_ang_factor;
	static	float			hinge_force_factor;
	static	float			hinge_force_factor1;
	static	float			hinge_force_factor2;
	static	float			hinge_vel;
	static	float			skel_fatal_impulse_factor;
#ifdef DEBUG
	friend class CLevelGraph;
#endif
protected:
	Fvector					m_saved_dir;
	Fvector					m_saved_position;
	float					m_saved_impulse;
	s16						m_saved_element;
	// Rotation
	SRotation				r_torso;
	float					r_model_yaw_dest;
	float					r_model_yaw;			// orientation of model
	float					r_model_yaw_delta;		// effect on multiple "strafe"+"something"

	float					m_fLandingTime;
	float					m_fJumpTime;
	float					m_fFallTime;

	u32						patch_frame;
	Fvector					patch_position;
	float					m_fCamHeightFactor;
	
	int						skel_ddelay;
	
	///////////////////////////////////////////////////
	static void	__stdcall	SpinCallback	(CBoneInstance*);
	static void	__stdcall	ShoulderCallback(CBoneInstance*);
	static void	__stdcall	HeadCallback	(CBoneInstance*);
	static void __stdcall	CarHeadCallback	(CBoneInstance*);
private:
	// Motions
	u32						mstate_wishful;	
	u32						mstate_real;	

	BOOL					m_bJumpKeyPressed;

	float					m_fWalkAccel;
	float					m_fJumpSpeed;
	float					m_fRunFactor;
	float					m_fCrouchFactor;
	
	float					m_fTimeToStep;

	int						m_r_hand;
	int						m_l_finger1;
	int						m_r_finger2;

	// Dropping
	BOOL					b_DropActivated;
	float					f_DropPower;

	// Cameras
	CCameraBase*			cameras[eacMaxCam];
	EActorCameras			cam_active;
	float					cam_gray;				// for zone-effects
	float					cam_shift;				// for zone-effects
	float					fPrevCamPos;
	CEffectorBobbing*		pCamBobbing;


	////////////////////////////////////////////
	// ��� �������������� � ������� ����������� 
	// ��� ����������

	// Person we're looking at
	CInventoryOwner*		m_pPersonWeLookingAt;
	// Car or lorry we're looking at
	CCar*					m_pCarWeLookingAt;
	//����� ���������� ���������
	bool					m_bPickupMode;

	void					PickupModeOn();
	void					PickupModeOff();
	void					PickupModeUpdate();
	void					PickupInfoDraw		(CObject* object);
		
	//------------------------------
	struct				net_update 		
	{
		u32					dwTimeStamp;			// server(game) timestamp
		float				o_model;				// model yaw
		SRotation			o_torso;				// torso in world coords
		Fvector				p_pos;					// in world coords
		Fvector				p_accel;				// in world coords
		Fvector				p_velocity;				// in world coords
		u32					mstate;
		int					weapon;
		float				fHealth;
		float				fArmor;

		net_update()	{
			dwTimeStamp		= 0;
			p_pos.set		(0,0,0);
			p_accel.set		(0,0,0);
			p_velocity.set	(0,0,0);
		}
		void	lerp		(net_update& A,net_update& B, float f);
	};
	xr_deque<net_update>	NET;
	Fvector					NET_SavedAccel;
	net_update				NET_Last;
	BOOL					NET_WasInterpolating;	// previous update was by interpolation or by extrapolation
	u32						NET_Time;				// server time of last update

	void					NetUpdate_Apply			( net_update &NetUpdate, float dt);
	//------------------------------
	struct					net_input
	{
		u32					m_dwTimeStamp;

		u32					mstate_wishful;	

		u8					cam_mode;
		float				cam_yaw;
		float				cam_pitch;
	};
	xr_deque<net_input>		NET_I;
	BOOL					NET_I_NeedReculc;
	void					NetInput_Save			( );
	//------------------------------
	void					g_cl_CheckControls		(u32 mstate_wf, Fvector &vControlAccel, float &Jump, float dt);
	void					g_cl_ValidateMState		(float dt, u32 mstate_wf);
	void					g_cl_Orientate			(u32 mstate_rl, float dt);
	void					g_sv_Orientate			(u32 mstate_rl, float dt);
	void					g_Orientate				(u32 mstate_rl, float dt);
	void					g_Physics				(Fvector& accel, float jump, float dt);
	void					g_SetAnimation			(u32 mstate_rl);

	virtual void			feel_touch_new			(CObject* O);
	virtual void			feel_touch_delete		(CObject* O);
	virtual BOOL			feel_touch_contact		(CObject* O);

	void					cam_Set					(EActorCameras style);
	void					cam_Update				(float dt, float fFOV);
	void					create_Skeleton			();
	void					create_Skeleton1		();
	void					attach_Vehicle			(CCar* vehicle);
	bool					use_Vehicle				(CGameObject* object);
	CGameObject*			pick_Object				(u16& element);
	void					ActorUse				();


public:
	void					detach_Vehicle			();
	void					steer_Vehicle			(float angle);
public:
							CActor					( );
	virtual					~CActor					( );


	IC static BOOL			isAccelerated		(u32 mstate)	
	{
		if (mstate&mcAccel)	return psActorFlags.test(AF_ALWAYSRUN)?FALSE:TRUE ;
		else				return psActorFlags.test(AF_ALWAYSRUN)?TRUE :FALSE;
	}
	IC BOOL					HUDview				( )const 
	{ 
		return IsFocused()&&(cam_active==eacFirstEye)&&(!m_vehicle); 
	}

	IC CCameraBase*						cam_Active			() {return cameras[cam_active];}

	// Network
	virtual void						Load				( LPCSTR section );
	virtual BOOL						net_Spawn			( LPVOID DC);
	virtual void						net_Export			( NET_Packet& P);				// export to server
	virtual void						net_Import			( NET_Packet& P);				// import from server
	virtual void						net_ImportInput		( NET_Packet& P);				// import input from remote client
	virtual BOOL						net_Relevant		()	{ return getSVU() | getLocal(); };		// relevant for export to server
	virtual	void						net_Relcase			( CObject* O );					//
	virtual void						net_Destroy			();

	virtual void						Die					( );
	virtual	void						Hit					(float P, Fvector &dir,			CObject* who, s16 element, float impulse,  ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual	void						Hit					(float P, Fvector &dir,			CObject* who, s16 element, Fvector position_in_bone_space, float impulse,  ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void						HitSignal			(float P, Fvector &vLocalDir,	CObject* who, s16 element);
	virtual	float						HitScale			(int element);

	virtual	float						ffGetFov			()	const	{ return 90.f;		}	
	virtual	float						ffGetRange			()	const	{ return 500.f;		}

	// misc
	//virtual CWeaponList*				GetItemList			(){return Weapons;}

	// Fire control
	virtual void						g_fireParams		(Fvector& P, Fvector& D);
	virtual void						g_fireStart			( );
	virtual void						g_fireEnd			( );
	virtual void						g_fire2Start		( );
	virtual void						g_fire2End			( );
	virtual BOOL						g_State				(SEntityState& state)
	{
		state.bJump			= !!(mstate_real&mcJump);
		state.bCrouch		= !!(mstate_real&mcCrouch);
		state.fVelocity		= m_PhysicMovementControl->GetVelocityActual();
		return TRUE;
	}
	virtual BOOL						renderable_ShadowGenerate	( ) {
															if(m_vehicle)return FALSE;
															return inherited::renderable_ShadowGenerate();
															}
	virtual void						g_PerformDrop		( );
	virtual void						g_WeaponBones		(int &L, int &R1, int &R2);
	
	virtual const SRotation				Orientation()		const { return r_torso; };
	SRotation							&Orientation()		{ return r_torso; };

	virtual void						IR_OnMouseMove		(int x, int y);
	//virtual void						IR_OnMousePress		(int dik);
	virtual void						IR_OnKeyboardPress	(int dik);
	virtual void						IR_OnKeyboardRelease(int dik);
	virtual void						IR_OnKeyboardHold	(int dik);

	virtual void						shedule_Update		( u32 T ); 
	virtual void						renderable_Render	( );
	virtual void						UpdateCL			( );
	virtual void						OnEvent				( NET_Packet& P, u16 type		);
	virtual void						ForceTransform		(const Fmatrix &m);
	void								ZoneEffect			(float z_amount);
	void								Statistic			( );
	void								SetPhPosition		(const Fmatrix& pos);
	// HUD
	virtual void						OnHUDDraw			(CCustomHUD* hud);
	//CWeaponList*						tpfGetWeapons		()	{return Weapons;}
	virtual f32 GetMass() { return g_Alive()?m_PhysicMovementControl->GetMass():m_pPhysicsShell?m_pPhysicsShell->getMass():0; }
	virtual float						Radius				() const;

#ifdef DEBUG
	virtual void						OnRender			();
#endif


	//by Dandy
	//Actor condition
	virtual void ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element = 0);
	virtual void UpdateCondition();

	//information receive
	virtual void OnReceiveInfo(int info_index);
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index);

	virtual void reinit	();
	virtual void reload	(LPCSTR section);
	virtual bool		use_bolts				() const;
};

#endif // !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
