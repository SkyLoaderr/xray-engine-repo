// CustomMonster.h: interface for the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_)
#define AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_
#pragma once

#include "..\feel_vision.h"
#include "..\feel_sound.h"
#include "..\feel_touch.h"
#include "entity.h"
#include "ai_commands.h"
#include "ai_pathnodes.h"

#define MAX_FUNCTION_COUNT 64

class  CBaseFunction;
class  CPatternFunction;
class  CHealthFunction;
class  CMoraleFunction;
class  CCreatureTypeFunction;
class  CWeaponTypeFunction;
class  CDistanceFunction;

class ENGINE_API CMotionDef;
class ENGINE_API CKinematics;
class ENGINE_API CBoneInstance;
class CWeaponList;

class CCustomMonster : 
	public CEntityAlive, 
	public Feel::Vision, 
	public Feel::Sound, 
	public Feel::Touch
#ifdef DEBUG
	, public pureRender
#endif
{
private:
	typedef	CEntityAlive	inherited;
protected:
	// weapons
	CWeaponList*		Weapons;
	
	struct				SAnimState
	{
		CMotionDef		*fwd;
		CMotionDef		*back;
		CMotionDef		*ls;
		CMotionDef		*rs;
		
		void			Create(CKinematics* K, LPCSTR base);
	};
	
	static void	__stdcall TorsoSpinCallback(CBoneInstance*);
	
	typedef struct tagSDynamicObject {
		DWORD			dwTime;
		DWORD			dwUpdateCount;
		DWORD			dwNodeID;
		Fvector			tSavedPosition;
		SRotation		tOrientation;
		DWORD			dwMyNodeID;
		Fvector			tMySavedPosition;
		SRotation		tMyOrientation;
		CEntity			*tpEntity;
	} SDynamicObject;

	typedef struct tagSDynamicSound {
		ESoundTypes		eSoundType;
		DWORD			dwTime;
		float			fPower;
		DWORD			dwUpdateCount;
		Fvector			tSavedPosition;
		SRotation		tOrientation;
		Fvector			tMySavedPosition;
		SRotation		tMyOrientation;
		CEntity			*tpEntity;
	} SDynamicSound;

	typedef struct tagSSimpleSound {
		ESoundTypes		eSoundType;
		DWORD			dwTime;
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

	// Pathfinding cache
	CPathNodes			AI_Path;

	// Eyes
	Fmatrix				eye_matrix;
	int					eye_bone;
	float				eye_fov;
	float				eye_range;

	u32					eye_pp_stage;
	u32					eye_pp_timestamp;
	objSET				eye_pp_seen;

	// AI
	AI::AIC_Look		q_look;
	AI::AIC_Action		q_action;
	Fvector				tWatchDirection;
	float				m_fMinSpeed;
	float				m_fMaxSpeed;
	float				m_fCurSpeed;

	DWORD				m_dwSoundUpdate;
	float				m_fSoundPower;
	float				m_fStartPower;
	CLevel::SPath		*m_tpPath;
	CCustomMonster		*m_tpCurrentEnemy;

	virtual void		Think() = 0;

	// Rotation
	SRotation			r_current,r_target, r_torso_current, r_torso_target;
	float				r_spine_speed, r_torso_speed;

	// Motions
	DWORD				dwMoveState;
	int					m_iHealth;

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
	//float				m_fTorsoAngle;
	DWORD				m_dwLoopCount;
	int					m_iCurrentPatrolIndex;
	bool				m_bPatrolPathInverted;
	DWORD				m_dwLastUpdateTime;

	// movement
	float				m_fJumpSpeed;

	// network
//------------------------------
	struct net_update	{
		u32				dwTimeStamp;			// server(game) timestamp
		float			o_model;				// model yaw
		SRotation		o_torso;				// torso in world coords
		Fvector			p_pos;					// in world coords

		// non-exported (temporal)

		net_update()	{
			dwTimeStamp		= 0;
			o_model			= 0;
			o_torso.yaw		= 0;
			o_torso.pitch	= 0;
			p_pos.set		(0,0,0);
		}
		void	lerp	(net_update& A,net_update& B, float f);
	};
	deque<net_update>	NET;
	net_update			NET_Last;
	BOOL				NET_WasInterpolating;	// previous update was by interpolation or by extrapolation
	DWORD				NET_Time;				// server time of last update
//------------------------------

	// utils
	void				mk_orientation			( Fvector& dir, Fmatrix& mR );
	void				mk_rotation				( Fvector& dir, SRotation &R);

	// stream executors
	void				Exec_Look				( float dt );
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

	virtual void		Exec_Action				( float dt );
	virtual BOOL		net_Spawn				( LPVOID DC);
	virtual void		Death					( );
	virtual void		HitSignal				( float P,	Fvector& vLocalDir, CObject* who);
	virtual void		g_WeaponBones			( int& L,	int& R	);
	virtual void		Load					( LPCSTR	section );				
	virtual void		Update					( DWORD		DT		);
	virtual void		UpdateCL				( );

	// Team visibility
	virtual void		GetVisible				(objVisible& R);

	// Fire control
	virtual void		g_fireParams			(Fvector& P, Fvector& D);
	virtual void		g_fireStart				( );
	virtual void		g_fireEnd				( );

	// Network
	virtual void		net_Export				(NET_Packet& P);				// export to server
	virtual void		net_Import				(NET_Packet& P);				// import from server

	virtual void		SelectAnimation			( const Fvector& _view, const Fvector& _move, float speed );

	// debug
	virtual void		OnRender				( );

	// HUD
	virtual void		OnHUDDraw				(CCustomHUD* hud);
	virtual bool		bfExecMovement			(){return(false);};
	IC		void		StandUp()				{ m_cBodyState = BODY_STATE_STAND;  };
	IC		void		Squat()					{ m_cBodyState = BODY_STATE_CROUCH; };
	IC		void		Lie()					{ m_cBodyState = BODY_STATE_LIE;    };

	// miscellaneous
	IC		int			ifGetMemberIndex() 
	{
		vector<CEntity*> &tpaMembers = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Members;
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
	
public:
	//typedef BOOL (*QualifierFunction)(CObject*, void*);
	virtual objQualifier*	GetQualifier		();
	virtual	float			ffGetFov			(){return eye_fov;}	
	virtual	float			ffGetRange			(){return eye_range;}
	CWeaponList			*	tpfGetWeapons		(){return Weapons;}
	virtual	void			feel_touch_new		(CObject* O);
	virtual	void			OnEvent				(NET_Packet& P, u16 type);

// Data driven design properties
	// primary functions
	static	bool					bPatternFunctionLoaded;
	static  CBaseFunction			*fpaBaseFunctions[MAX_FUNCTION_COUNT];
	static  CHealthFunction			pfHealth;
	static  CMoraleFunction			pfMorale;
	static  CCreatureTypeFunction	pfCreatureType;
	static  CWeaponTypeFunction		pfWeaponType;
	static  CDistanceFunction		pfDistance;
	// complex functions
	static  CPatternFunction		pfEnemyStatus;
	static  CPatternFunction		pfPersonalStatus;
	static  CPatternFunction		pfWeaponEffectiveness;
	static  CPatternFunction		pfAttackSuccessProbability;
	static  CPatternFunction		pfDefendSuccessProbability;
};

namespace AI{
	const int LOST_ENEMY_REACTION_TIME	= 30000;
	const int HIT_REACTION_TIME			= 30000;
	const int SENSE_REACTION_TIME		= 30000;
	const int HIT_JUMP_TIME				= 300;
	const int SENSE_JUMP_TIME			= 00000;
}

#endif // !defined(AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_)
