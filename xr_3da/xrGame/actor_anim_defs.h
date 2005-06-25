#pragma once

#include "../SkeletonAnimated.h"

struct					SActorMotions
{
	struct				SActorState
	{
		struct			SAnimState
		{
			MotionID	legs_fwd;
			MotionID	legs_back;
			MotionID	legs_ls;
			MotionID	legs_rs;
			void		Create(CSkeletonAnimated* K, LPCSTR base0, LPCSTR base1);
		};
		struct			STorsoWpn{
			MotionID	aim;
			MotionID	aim_zoom;
			MotionID	holster;
			MotionID	draw;
			MotionID	drop;
			MotionID	reload;
			MotionID	reload_1;
			MotionID	reload_2;
			MotionID	attack;
			MotionID	attack_zoom;
			MotionID	fire_idle;
			MotionID	fire_end;

			//�������� ��� ����� ��� ����� ���� (����� �� ����� �� �����)
			MotionID	all_attack_0;
			MotionID	all_attack_1;
			MotionID	all_attack_2;
			void		Create(CSkeletonAnimated* K, LPCSTR base0, LPCSTR base1);
		};
		MotionID		legs_idle;
		MotionID		jump_begin;
		MotionID		jump_idle;
		MotionID		landing[2];
		MotionID		legs_turn;
		MotionID		death;
		SAnimState		m_walk;
		SAnimState		m_run;
		STorsoWpn		m_torso[10];
		MotionID		m_torso_idle;
		MotionID		m_head_idle;

		MotionID		m_damage[DAMAGE_FX_COUNT];
		void			Create(CSkeletonAnimated* K, LPCSTR base);
		void			CreateClimb(CSkeletonAnimated* K);
	};

	struct SActorSprintState 
	{
		//toroso anims
		MotionID		m_toroso[8];
		//leg anims
		MotionID		legs_fwd;
		MotionID		legs_ls;
		MotionID		legs_rs;
		void Create		(CSkeletonAnimated* K);
	};

	MotionID			m_dead_stop;

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
		static const u16 MAX_IDLES = 3		;
		u16				idles_num			;
		MotionID		idles[MAX_IDLES]	;
		MotionID		steer_left			;
		MotionID		steer_right			;
						SOneTypeCollection	();
		void			Create				(CSkeletonAnimated* K,u16 num);
	};
	SOneTypeCollection m_vehicles_type_collections[TYPES_NUMBER];
						SActorVehicleAnims	();
	void				Create				(CSkeletonAnimated* K);
};


