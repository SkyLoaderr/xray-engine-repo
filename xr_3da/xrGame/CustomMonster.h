// CustomMonster.h: interface for the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_)
#define AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_
#pragma once

#include "..\portal.h"
//#include "..\lightPPA.h"
#include "entity.h"
#include "custommonster_vistrack.h"
#include "ai_commands.h"
#include "ai_pathnodes.h"

class ENGINE_API CMotionDef;
class ENGINE_API CKinematics;
class ENGINE_API CBoneInstance;
class CWeaponList;

class CCustomMonster : public CEntity, public pureRender  
{
private:
	typedef	CEntity		inherited;
protected:
	// weapons
	CWeaponList*		Weapons;
	
	struct				SRotation
	{
		float yaw,pitch;
		SRotation() { yaw=pitch=0; }
	};
	struct				SAnimState
	{
		CMotionDef		*fwd;
		CMotionDef		*back;
		CMotionDef		*ls;
		CMotionDef		*rs;
		
		void			Create(CKinematics* K, LPCSTR base);
	};
	static void	__stdcall SpinCallback(CBoneInstance*);
public:
	// Lighting and effects
//	CLightPPA			TEST;

	// Pathfinding cache
	CPathNodes			AI_Path;

	// Eyes
	AI::VisiTrack		ai_Track;
	Fmatrix				eye_matrix;
	int					eye_bone;
	float				eye_fov;
	float				eye_range;

	// AI
	AI::AIC_Look		q_look;
	AI::AIC_Action		q_action;
	Fvector				tWatchDirection;
	float				m_fMinSpeed;
	float				m_fMaxSpeed;
	float				m_fCurSpeed;

	virtual void		Think() = 0;

	// Rotation
	SRotation			r_current,r_target;

	// Motions
	DWORD				dwMoveState;
	CMotionDef*			m_current;
	CMotionDef*			m_death;
	CMotionDef*			m_idle;
	SAnimState			m_walk;
	SAnimState			m_run;

	// movement
	float				m_fWalkAccel;
	float				m_fJumpSpeed;
	float				m_fRunCoef;

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
	void				Exec_Movement			( float dt );
	void				Exec_Action				( float dt );
	void				Exec_Visibility			( float dt );

	void				BuildCamera				( );
public:
						CCustomMonster			( );
	virtual				~CCustomMonster			( );

	virtual BOOL		Spawn					( BOOL bLocal, int sid, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
	virtual void		Death					( );
	virtual void		HitSignal				( int iHitAmount, Fvector& vLocalDir, CEntity* who);
	virtual void		SenseSignal				( int iSenseAmount, Fvector& vLocalDir, CEntity* who);
	virtual void		Load					( CInifile* ini, const char* section );
	virtual void		Update					( DWORD DT );
	virtual void		UpdateCL				( );

	virtual BOOL		TakeItem				( DWORD CID );

	// Team visibility
	virtual void		GetVisible				(objVisible& R);

	// Fire control
	virtual void		g_fireParams			(Fvector& P, Fvector& D);
	virtual void		g_cl_fireStart			( );
	virtual void		g_fireEnd				( );

	// Network
	virtual void		net_Export				(NET_Packet* P);				// export to server
	virtual void		net_Import				(NET_Packet* P);				// import from server

	virtual void		SelectAnimation			( const Fvector& _view, const Fvector& _move, float speed );

	// debug
	virtual void		OnRender				( );

	// HUD
	virtual void		OnHUDDraw				(CCustomHUD* hud);
};

namespace AI{
	#define LOST_ENEMY_REACTION_TIME	30000
	#define HIT_REACTION_TIME			30000
	#define SENSE_REACTION_TIME			30000
	#define HIT_JUMP_TIME				3000
	#define SENSE_JUMP_TIME				3000
}

#endif // !defined(AFX_CUSTOMMONSTER_H__D44439C3_D752_41AE_AD49_C68E5DE3045F__INCLUDED_)
