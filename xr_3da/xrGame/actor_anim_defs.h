#pragma once

class CMotionDef;
class CSkeletonAnimated;


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

			//анимации для атаки для всего тела (когда мы стоим на месте)
			CMotionDef*	all_attack_0;
			CMotionDef*	all_attack_1;
			CMotionDef*	all_attack_2;
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
		STorsoWpn		m_torso[8];
		CMotionDef*		m_torso_idle;
		CMotionDef*		m_head_idle;

		CMotionDef*		m_damage[DAMAGE_FX_COUNT];
		void			Create(CSkeletonAnimated* K, LPCSTR base);
		void			CreateClimb(CSkeletonAnimated* K);
	};

	struct SActorSprintState 
	{
		//toroso anims
		CMotionDef*		m_toroso[8];
		//leg anims
		CMotionDef*		legs_fwd;
		CMotionDef*		legs_ls;
		CMotionDef*		legs_rs;
		void Create		(CSkeletonAnimated* K);
	};

	CMotionDef*			m_dead_stop;

	SActorState			m_normal;
	SActorState			m_crouch;
	SActorState			m_climb;
	SActorSprintState	m_sprint;
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


