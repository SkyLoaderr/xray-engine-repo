// Actor.h: interface for the CActor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
#define AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_
#pragma once

#include "entity.h"
#include "actor_flags.h"

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CBoneInstance;
class ENGINE_API CMotionDef;
class CWeaponList;
class ENGINE_API CKinematics;
class ENGINE_API CBlend;
class CEffectorBobbing;

class CActor: public CEntity, public pureRender
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
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
		mcFwd		= 0x0001,
		mcBack		= 0x0002,
		mcLStrafe	= 0x0004,
		mcRStrafe	= 0x0008,
		mcJump		= 0x0010,
		mcCrouch	= 0x0020,
		mcAccel		= 0x0040,
		mcTurn		= 0x0080,

		mcAnyMove	= (mcFwd|mcBack|mcLStrafe|mcRStrafe)
	};
protected:
	// weapons
	CWeaponList*		Weapons;

	// media
	int					sndStep[2];
	int					sndAmbient;
	BOOL				bSndAmbientPlaying;
	BOOL				bStep;
	int					sndLanding;
	sound3D				sndWeaponChange;
	sound3D				sndRespawn;
	sound3D				sndHit[SND_HIT_COUNT];
	sound3D				sndDie[SND_DIE_COUNT];

	struct				SActorState
	{
		struct			SAnimState
		{
			CMotionDef*	legs_fwd;
			CMotionDef*	legs_back;
			CMotionDef*	legs_ls;
			CMotionDef*	legs_rs;
			
			void		Create(CKinematics* K, LPCSTR base0, LPCSTR base1);
		};
		CMotionDef*		torso_aim;
		CMotionDef*		idle;
		CMotionDef*		jump_begin;
		CMotionDef*		jump_idle;
		CMotionDef*		legs_turn;
		CMotionDef*		death;
		SAnimState		m_walk;
		SAnimState		m_run;
		void			Create(CKinematics* K, LPCSTR base);
	};
public:
	// animation
	CBlend*				m_current_legs_blend;
	CBlend*				m_current_jump_blend;
	CMotionDef*			m_current_legs;
	CMotionDef*			m_current_torso;
	SActorState			m_normal;
	SActorState			m_crouch;

protected:
	// Rotation
	SRotation			r_torso;
	float				r_model_yaw_dest;
	float				r_model_yaw;			// orientation of model
	float				r_model_yaw_delta;		// effect on multiple "strafe"+"something"

	static void	__stdcall SpinCallback(CBoneInstance*);
private:
	typedef CEntity		inherited;
	BOOL				bAlive;

	// Motions
	DWORD				mstate_wishful;	
	DWORD				mstate_real;	

	BOOL				m_bJumpKeyPressed, m_bJumpInProgress;

	float				m_fWalkAccel;
	float				m_fJumpSpeed;
	float				m_fRunFactor;
	float				m_fCrouchFactor;
	
	float				m_fTimeToStep;

	// Cameras
	CCameraBase*		cameras[eacMaxCam];
	EActorCameras		cam_active;
	float				fPrevCamPos;
	CEffectorBobbing*	pCamBobbing;

//------------------------------
	struct				net_update 		
	{
		DWORD			dwTimeStamp;			// server(game) timestamp
		float			o_model;				// model yaw
		SRotation		o_torso;				// torso in world coords
		Fvector			p_pos;					// in world coords
		Fvector			p_accel;				// in world coords
		Fvector			p_velocity;				// in world coords
		Fvector			f_pos;
		Fvector			f_dir;
		DWORD			mstate;
		int				weapon;

		net_update()	{
			dwTimeStamp		= 0;
			p_pos.set		(0,0,0);
			p_accel.set		(0,0,0);
			p_velocity.set	(0,0,0);
		}
		void	lerp	(net_update& A,net_update& B, float f);
	};
	deque<net_update>	NET;
	Fvector				NET_SavedAccel;
	net_update			NET_Last;
	BOOL				NET_WasInterpolating;	// previous update was by interpolation or by extrapolation
	DWORD				NET_Time;				// server time of last update
//------------------------------
	void				g_cl_CheckControls		(DWORD mstate_wf, Fvector &vControlAccel, float &Jump, float dt);
	void				g_cl_ValidateMState		(DWORD mstate_wf);
	void				g_cl_Orientate			(DWORD mstate_rl, float dt);
	void				g_sv_Orientate			(DWORD mstate_rl, float dt);
	void				g_Orientate				(DWORD mstate_rl, float dt);
	void				g_Physics				(Fvector& accel, float jump, float dt);
	void				g_SetAnimation			(DWORD mstate_rl);

	void				cam_Set					(EActorCameras style);
	void				cam_Update				(float dt, BOOL bZoom);

	bool				HUDview					( ) { return IsFocused()&&(cam_active==eacFirstEye); }
public:
						CActor					( );
	virtual				~CActor					( );


	IC static BOOL		isAccelerated			(DWORD mstate)	
	{
		if (mstate&mcAccel)	return (psActorFlags&AF_ALWAYSRUN)?FALSE:TRUE ;
		else				return (psActorFlags&AF_ALWAYSRUN)?TRUE :FALSE;
	}

	virtual void		Load					( CInifile* ini, const char *section );

	virtual void		Die						( );
	virtual	BOOL		Spawn					(BOOL bLocal, int sid, int team, int squad, int group, Fvector4& o_pos );
	virtual	BOOL		Hit						(int iLost, Fvector &dir, CEntity* who);
	virtual void		HitSignal				(int HitAmount, Fvector& vLocalDir, CEntity* who);

	// Fire control
	virtual void		g_fireParams			(Fvector& P, Fvector& D);
	virtual void		g_cl_fireStart			( );
	virtual void		g_sv_fireStart			(NET_Packet* P);
	virtual void		g_fireEnd				( );

	// Network
	virtual void		net_Export				(NET_Packet* P);				// export to server
	virtual void		net_Import				(NET_Packet* P);				// import from server
	virtual BOOL		net_Relevant			()	{ return net_Local; };		// relevant for export to server
	
	virtual BOOL		TakeItem				( DWORD CID );

	virtual void		OnMouseMove				(int x, int y);
	virtual void		OnKeyboardPress			(int dik);
	virtual void		OnKeyboardRelease		(int dik);
	virtual void		OnKeyboardHold			(int dik);

	virtual void		Update					( DWORD T ); 
	virtual void		OnMoveVisible			( ); 

	void				Statistic				( );
	virtual void		OnRender				( );

	// HUD
	virtual void		OnHUDDraw				(CCustomHUD* hud);
};

#endif // !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
