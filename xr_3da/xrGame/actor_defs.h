#include "PHSynchronize.h"
#include "xrserver_space.h"

#pragma once


#define ACTOR_HEIGHT 1.75f

namespace ACTOR_DEFS 
{

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
	mcCreep		= (1ul<<12ul),
	mcAnyMove	= (mcFwd|mcBack|mcLStrafe|mcRStrafe)
};

// enum дл€ определени€ действи€ над вещью на которую наведен в текущее врем€ прицел.
// »спользуетс€ дл€ показа всплывающих динамических подсказок
enum EActorAction
{
	eaaNoAction			= 0,
	eaaPickup,
	eaaTalk,
	eaaOpenDoor,
	eaaSearchCorpse,
};

//результат функции GoSleep у актера
enum EActorSleep
{
	easCanSleep			= 0,
	easNotSolidGround,
	easEnemies		
};

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
			CMotionDef*	aim_zoom;
			CMotionDef*	holster;
			CMotionDef*	draw;
			CMotionDef*	drop;
			CMotionDef*	reload;
			CMotionDef*	attack;
			CMotionDef*	attack_zoom;
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
		CMotionDef*		m_head_idle;

		CMotionDef*		m_damage[DAMAGE_FX_COUNT];

		void			Create(CSkeletonAnimated* K, LPCSTR base);
		void			CreateClimb(CSkeletonAnimated* K);
	};
	//CMotionDef*			m_steering_torso_left;
	//CMotionDef*			m_steering_torso_right;
	//CMotionDef*			m_steering_torso_idle;
	//CMotionDef*			m_steering_legs_idle;
	

	CMotionDef*			m_dead_stop;

	SActorState			m_normal;
	SActorState			m_crouch;
	SActorState			m_climb;
	void				Create(CSkeletonAnimated* K);
};

//
struct SActorVehicleAnims
{
			static const TYPES_NUMBER=2;
			struct	SOneTypeCollection
				{
						static const u16		MAX_IDLES = 3				;
									 u16		idles_num					;
							CMotionDef*			idles[MAX_IDLES]	;
							CMotionDef*			steer_left					;
							CMotionDef*			steer_right					;
							SOneTypeCollection	()							;
					void	Create				(CSkeletonAnimated* K,u16 num);
				};
			SOneTypeCollection m_vehicles_type_collections[TYPES_NUMBER]	;
			SActorVehicleAnims	()											;
	void	Create				(CSkeletonAnimated* K)						;
};

//---------------------------------------------
// ввод с клавиатуры и мыши
struct					net_input
{
	u32					m_dwTimeStamp;

	u32					mstate_wishful;	

	u8					cam_mode;
	float				cam_yaw;
	float				cam_pitch;

	bool operator < (const u32 Time)
	{
		return m_dwTimeStamp < Time;
	};
};

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

///////////////////////////////////////////////////////
// апдайт с данными физики
struct					net_update_A
{
	u32					dwTimeStamp;
//	u32					dwTime0;
//	u32					dwTime1;
	SPHNetState			State;
};

///////////////////////////////////////////////////////
// данные дл€ интерпол€ции
struct					InterpData
{
	Fvector				Pos;
	float				o_model;				// model yaw
	SRotation			o_torso;				// torso in world coords
};


};