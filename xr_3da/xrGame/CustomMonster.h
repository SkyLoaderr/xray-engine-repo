// CustomMonster.h: interface for the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_)
#define AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_
#pragma once

#define NO_PHYSICS_IN_AI_MOVE

#include "..\feel_vision.h"
#include "..\feel_sound.h"
#include "..\feel_touch.h"
#include "entity.h"
#include "phmovementcontrol.h"
#include "ai_space.h"
#include "ai_commands.h"
#include "ai_pathnodes.h"
#include "AI\\ai_monster_state.h"


//#define IGNORE_ACTOR

class CCustomMonster : 
	public CEntityAlive, 
	public Feel::Vision, 
	public Feel::Sound, 
	public Feel::Touch

{
private:
	typedef	CEntityAlive	inherited;
protected:
	// weapons
	struct				SAnimState
	{
		CMotionDef		*fwd;
		CMotionDef		*back;
		CMotionDef		*ls;
		CMotionDef		*rs;
		
		void			Create(CKinematics* K, LPCSTR base);
	};
	
//	static void	__stdcall TorsoSpinCallback(CBoneInstance*);
	
	typedef struct tagSDynamicObject {
		u32				dwTime;
		u32				dwUpdateCount;
		u32				dwNodeID;
		Fvector			tSavedPosition;
		SRotation		tOrientation;
		u32				dwMyNodeID;
		Fvector			tMySavedPosition;
		SRotation		tMyOrientation;
		CEntity			*tpEntity;
	} SDynamicObject;

	typedef struct tagSDynamicSound {
		ESoundTypes		eSoundType;
		u32				dwTime;
		float			fPower;
		u32				dwUpdateCount;
		Fvector			tSavedPosition;
		u32				dwNodeID;
		SRotation		tOrientation;
		Fvector			tMySavedPosition;
		u32				dwMyNodeID;
		SRotation		tMyOrientation;
		CEntity			*tpEntity;
	} SDynamicSound;

	typedef struct tagSSimpleSound {
		ESoundTypes		eSoundType;
		u32				dwTime;
		float			fPower;
		Fvector			tSavedPosition;
		CEntity			*tpEntity;
	} SSimpleSound;

public:
	enum EBodyStates {
		BODY_STATE_STAND=0,
		BODY_STATE_CROUCH,
		BODY_STATE_LIE,
	};
	
	//Movement
	CPHMovementControl	Movement;

	// Pathfinding cache
	AI::CPathNodes		AI_Path;
	AI::DWORD_VECTOR	m_tpaGraphPath;

	// Eyes
	Fmatrix				eye_matrix;
	int					eye_bone;
	float				eye_fov;
	float				eye_range;

	u32					eye_pp_stage;
	u32					eye_pp_timestamp;
	Fvector				m_tEyeShift;
	BOOL				NET_WasExtrapolating;

	// AI
	AI::AIC_Look		q_look;
	AI::AIC_Action		q_action;
	Fvector				tWatchDirection;
	float				m_fMinSpeed;
	float				m_fMaxSpeed;
	float				m_fCurSpeed;

	u32					m_dwSoundUpdate;
	float				m_fSoundPower;
	float				m_fStartPower;
	CLevel::SPath		*m_tpPath;
	//CWeaponList			*Weapons;


	virtual void		Think() = 0;

	// Rotation
	SRotation			r_current,r_target, r_torso_current, r_torso_target;
	float				r_spine_speed, r_torso_speed;

	// Motions
	u32					dwMoveState;

	CMotionDef*			m_current;
	
	CMotionDef*			m_death;
	CMotionDef*			m_idle;
	SAnimState			m_walk;
	SAnimState			m_run;
	CMotionDef*			m_turn;
	
	CMotionDef* 		m_crouch_death;
	CMotionDef* 		m_crouch_idle;
	CMotionDef* 		m_crouch_turn;
	SAnimState 			m_crouch_walk;
	SAnimState 			m_crouch_run;

	char				m_cBodyState;
	
	float				m_fTimeUpdateDelta;
	u32					m_dwLoopCount;
	int					m_iCurrentPatrolIndex;
	bool				m_bPatrolPathInverted;
	u32					m_dwLastUpdateTime;

	// movement
	float				m_fJumpSpeed;

	// Biting/Himera motion & states management
	u32					m_dwCurrentUpdate;
	EMotionAnim			m_tAnim;
	void				SetDirectionLook();


	// network
//------------------------------
	struct net_update	{
		u32				dwTimeStamp;			// server(game) timestamp
		float			o_model;				// model yaw
		SRotation		o_torso;				// torso in world coords
		Fvector			p_pos;					// in world coords
		float			fHealth;

		// non-exported (temporal)

		net_update()	{
			dwTimeStamp		= 0;
			o_model			= 0;
			o_torso.yaw		= 0;
			o_torso.pitch	= 0;
			p_pos.set		(0,0,0);
			fHealth			= 0.f;
		}
		void	lerp	(net_update& A,net_update& B, float f);
		
//		IC bool angle_lerp_bounds(float &a, float b, float c)
//		{
//			float fDifference;
//			if ((fDifference = _abs(a - b)) > PI - EPS_L)
//				fDifference = PI_MUL_2 - fDifference;
//
//			if (c >= fDifference) {
//				a = b;
//				return(true);
//			}
//			
//			angle_lerp(a,b,c);
//
//			return(false);
//		}
//
	};
	xr_deque<net_update>	NET;
	net_update				NET_Last;
	BOOL					NET_WasInterpolating;	// previous update was by interpolation or by extrapolation
	u32						NET_Time;				// server time of last update
//------------------------------

	// utils
	void				mk_orientation			( Fvector& dir, Fmatrix& mR );
	void				mk_rotation				( Fvector& dir, SRotation &R);

	// stream executors
	virtual void		Exec_Action				( float dt );
	virtual void		Exec_Look				( float dt );
	virtual void		Exec_Movement			( float dt );
	void				Exec_Physics			( float dt );
	void				Exec_Visibility			( );
	void				eye_pp_s0				( );
	void				eye_pp_s1				( );
	void				eye_pp_s2				( );

	void				BuildCamera				( );
public:
						CCustomMonster			( );
	virtual				~CCustomMonster			( );

	virtual BOOL		net_Spawn				( LPVOID DC);
	virtual void		Die						( );

	virtual void		HitSignal				( float P,	Fvector& vLocalDir, CObject* who);
	virtual void		g_WeaponBones			(int &L, int &R1, int &R2) {};
	virtual void		Load					( LPCSTR	section );				
	virtual void		shedule_Update					( u32		DT		);
	virtual void		UpdateCL				( );

	// Team visibility
	virtual void		GetVisible				(objVisible& R);

	// Network
	virtual void		net_Export				(NET_Packet& P);				// export to server
	virtual void		net_Import				(NET_Packet& P);				// import from server

	virtual void		SelectAnimation			( const Fvector& _view, const Fvector& _move, float speed );

	// debug
#ifdef DEBUG
	virtual void		OnRender				( );
#endif

	// HUD
	virtual void		OnHUDDraw				(CCustomHUD* hud);
	virtual bool		bfExecMovement			(){return(false);};
	IC		void		StandUp()				{ m_cBodyState = BODY_STATE_STAND;  };
	IC		void		Squat()					{ m_cBodyState = BODY_STATE_CROUCH; };
	IC		void		Lie()					{ m_cBodyState = BODY_STATE_LIE;    };

	// miscellaneous
	IC		int			ifGetMemberIndex() 
	{
		xr_vector<CEntity*> &tpaMembers = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Members;
		int iCount = (int)tpaMembers.size();
		for (int i=0; i<iCount; i++)
			if (this == tpaMembers[i])
				return(i);
		return(-1);
	};

	IC		void		vfResetPatrolData()
	{
		m_dwLoopCount = 0;
		m_iCurrentPatrolIndex = -1;
		m_bPatrolPathInverted = false;
	};

	IC		bool		angle_lerp_bounds(float &a, float b, float c, float d)
	{
		float fDifference;
		if ((fDifference = _abs(a - b)) > PI - EPS_L)
			fDifference = PI_MUL_2 - fDifference;

		if (c*d >= fDifference) {
			a = b;
			return(true);
		}
		
		angle_lerp(a,b,c,d);

		return(false);
	};
	
	IC  CGroup *getGroup() {return(&(Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()]));};

public:
	virtual	float			ffGetFov				()				{return eye_fov;}	
	virtual	float			ffGetRange				()				{return eye_range;}
	virtual	void			feel_touch_new			(CObject* O);
	virtual BOOL			feel_visible_isRelevant	(CObject* O);
};

namespace AI{
	const int LOST_ENEMY_REACTION_TIME	= 30000;
	const int HIT_REACTION_TIME			= 30000;
	const int SENSE_REACTION_TIME		= 30000;
	const int HIT_JUMP_TIME				= 300;
	const int SENSE_JUMP_TIME			= 00000;
}

#endif // !defined(AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_)
