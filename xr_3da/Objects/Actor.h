// Actor.h: interface for the CActor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
#define AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_
#pragma once

#include "entity.h"
#include "actor_flags.h"
#include "..\feel_touch.h"
#include "PHMovementControl.h"
// refs
class ENGINE_API CCameraBase;
class ENGINE_API CBoneInstance;
class ENGINE_API CMotionDef;
class ENGINE_API CKinematics;
class ENGINE_API CBlend;
class CWeaponList;
class CEffectorBobbing;
class CTargetCS;

class CActor: 
	public CEntityAlive, 
	public Feel::Touch
{
private:
	typedef CEntityAlive	inherited;
protected:
	CPHMovementControl ph_Movement;
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
public:
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

		mcAnyMove	= (mcFwd|mcBack|mcLStrafe|mcRStrafe)
	};
protected:
	// Weapons and Items
	CWeaponList*			Weapons;
	CTargetCS*				m_pArtifact;

	// Death
	float					hit_slowmo;
	float					hit_factor;

	// media
	BOOL					bStep;
	sound					sndStep[2];
	sound					sndLanding;
	sound					sndZoneHeart;
	sound					sndZoneDetector;
	sound					sndHit[SND_HIT_COUNT];
	sound					sndDie[SND_DIE_COUNT];

	struct					SActorState
	{
		struct				SAnimState
		{
			CMotionDef*		legs_fwd;
			CMotionDef*		legs_back;
			CMotionDef*		legs_ls;
			CMotionDef*		legs_rs;
			void			Create(CKinematics* K, LPCSTR base0, LPCSTR base1);
		};
		struct				STorsoWpn{
			CMotionDef*		aim;
			CMotionDef*		holster;
			CMotionDef*		draw;
			CMotionDef*		drop;
			CMotionDef*		reload;
			CMotionDef*		attack;
			void			Create(CKinematics* K, LPCSTR base0, LPCSTR base1);
		};

		CMotionDef*			legs_idle;
		CMotionDef*			jump_begin;
		CMotionDef*			jump_idle;
		CMotionDef*			landing[2];
		CMotionDef*			legs_turn;
		CMotionDef*			death;
		SAnimState			m_walk;
		SAnimState			m_run;
		STorsoWpn			m_torso[2];
		CMotionDef*			m_torso_idle;
		void				Create(CKinematics* K, LPCSTR base);
	};
	BOOL					m_bAnimTorsoPlayed;
	static void				AnimTorsoPlayCallBack(CBlend* B)
	{
		CActor* actor		= (CActor*)B->CallbackParam;
		actor->m_bAnimTorsoPlayed = FALSE;
	}
public:
	// animation
	CBlend*					m_current_legs_blend;
	CBlend*					m_current_jump_blend;
	CMotionDef*				m_current_legs;
	CMotionDef*				m_current_torso;
	SActorState				m_normal;
	SActorState				m_crouch;
protected:
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

	static void	__stdcall	SpinCallback	(CBoneInstance*);
	static void	__stdcall	ShoulderCallback(CBoneInstance*);
	static void	__stdcall	HeadCallback	(CBoneInstance*);
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

	// 
	vector<Fvector4>		zone_areas;

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
	deque<net_update>		NET;
	Fvector					NET_SavedAccel;
	net_update				NET_Last;
	BOOL					NET_WasInterpolating;	// previous update was by interpolation or by extrapolation
	u32						NET_Time;				// server time of last update

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

	void					cam_Set					(EActorCameras style);
	void					cam_Update				(float dt, float fFOV);

public:
							CActor					( );
	virtual					~CActor					( );


	IC static BOOL			isAccelerated		(u32 mstate)	
	{
		if (mstate&mcAccel)	return (psActorFlags&AF_ALWAYSRUN)?FALSE:TRUE ;
		else				return (psActorFlags&AF_ALWAYSRUN)?TRUE :FALSE;
	}
	IC BOOL					HUDview				( ) 
	{ 
		return IsFocused()&&(cam_active==eacFirstEye); 
	}

	IC CCameraBase*						cam_Active			() {return cameras[cam_active];}

	// Network
	virtual void						Load				( LPCSTR section );
	virtual BOOL						net_Spawn			( LPVOID DC);
	virtual void						net_Export			( NET_Packet& P);				// export to server
	virtual void						net_Import			( NET_Packet& P);				// import from server
	virtual BOOL						net_Relevant		()	{ return getLocal(); };		// relevant for export to server
	virtual	void						net_Relcase			( CObject* O );					//
	virtual void						net_Destroy			();

	virtual void						Die					( );
	virtual	void						Hit					(float P, Fvector &dir,			CObject* who, s16 element);
	virtual void						HitSignal			(float P, Fvector &vLocalDir,	CObject* who, s16 element);
	virtual	float						HitScale			(int element);

	virtual	float						ffGetFov			()			{ return 90.f;		}	
	virtual	float						ffGetRange			()			{ return 500.f;		}

	// misc
	virtual CWeaponList*				GetItemList			(){return Weapons;}

	// Fire control
	virtual void						g_fireParams		(Fvector& P, Fvector& D);
	virtual void						g_fireStart			( );
	virtual void						g_fireEnd			( );
	virtual BOOL						g_State				(SEntityState& state)
	{
		state.bJump			= !!(mstate_real&mcJump);
		state.bCrouch		= !!(mstate_real&mcCrouch);
		state.fVelocity		= Movement.GetVelocityActual();
		return TRUE;
	}
	virtual void						g_PerformDrop		( );
	virtual void						g_WeaponBones		( int& L, int& R);
	
	SRotation							Orientation()		{ return r_torso; };

	virtual void						OnMouseMove			(int x, int y);
	virtual void						OnKeyboardPress		(int dik);
	virtual void						OnKeyboardRelease	(int dik);
	virtual void						OnKeyboardHold		(int dik);

	virtual void						Update				( u32 T ); 
	virtual void						UpdateCL			( );
	virtual void						OnVisible			( );
	virtual void						OnEvent				( NET_Packet& P, u16 type		);
	virtual void						OnDeviceCreate		( );

	void								ZoneEffect			(float z_amount);
	void								Statistic			( );

	// HUD
	virtual void						OnHUDDraw			(CCustomHUD* hud);
	CWeaponList*						tpfGetWeapons		()	{return Weapons;}
	virtual	void						PhHit				(Fvector point,float power,int boneid){};

#ifdef DEBUG
	virtual void						OnRender			();
#endif
};

#endif // !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
