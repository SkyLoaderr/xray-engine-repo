#pragma once
class CBaseMonster;
class CEntity;

#define HIT_STACK_SIZE	2

class CMeleeChecker {

	CBaseMonster	*m_object;

	// ltx parameters
	float			m_min_attack_distance;
	float			m_max_attack_distance;
	float			m_as_min_dist;			
	float			m_as_step;				

	bool			m_hit_stack[HIT_STACK_SIZE];

	float			m_current_min_distance;	
	
public:
			void	init_external			(CBaseMonster *obj) {m_object = obj;}
	IC		void	load					(LPCSTR section);
			
	// ���������������� ��������� �����
	IC		void	init_attack				();
			void	on_hit_attempt			(bool hit_success);

			// �������� ���������� �� fire_bone �� �����
			// ��������� RayQuery �� fire_bone � enemy.center
			float	distance_to_enemy		(const CEntity *enemy);

	IC		float	get_min_distance		();
	IC		float	get_max_distance		();

	IC		bool	can_start_melee			(const CEntity *enemy);
	IC		bool	should_stop_melee		(const CEntity *enemy);

};

#include "melee_checker_inline.h"