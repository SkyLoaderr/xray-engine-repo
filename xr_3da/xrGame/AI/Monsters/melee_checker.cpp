#include "stdafx.h"
#include "melee_checker.h"
#include "basemonster/base_monster.h"
#include "../../../skeletonanimated.h"
#include "../../../xr_collide_form.h"

#define MAX_TRACE_ENEMY_RANGE	6.f

float CMeleeChecker::distance_to_enemy(const CEntity *enemy)
{
	float dist = enemy->Position().distance_to	(m_object->Position());
	if (dist > MAX_TRACE_ENEMY_RANGE)			return dist;

	Fvector					enemy_center;
	enemy->Center			(enemy_center);

	Fmatrix					global_transform;
	global_transform.set	(m_object->XFORM());

	CKinematics *p_visual	= smart_cast<CKinematics*>(m_object->Visual());

	global_transform.mulB	(p_visual->LL_GetBoneInstance(p_visual->LL_BoneID("bip01_head")).mTransform);

	Fvector					dir; 
	dir.sub					(enemy_center, global_transform.c);

	collide::ray_defs		r_query	(global_transform.c, dir, MAX_TRACE_ENEMY_RANGE, CDB::OPT_CULL | CDB::OPT_ONLYNEAREST, collide::rqtObject);
	collide::rq_results		r_res;

	m_object->setEnabled	(false);
	
	if (enemy->CFORM()->_RayQuery(r_query, r_res)) 
		if (r_res.r_begin()->O) dist = r_res.r_begin()->range;
	
	m_object->setEnabled	(true);		

	return dist;
}

void CMeleeChecker::on_hit_attempt(bool hit_success) 
{
	// добавить новый элемент в стек
	for (u32 i=HIT_STACK_SIZE-1; i > 0; i--) m_hit_stack[i] = m_hit_stack[i-1];
	m_hit_stack[0] = hit_success;

	// проверить однородность стека
	bool stack_similar = true;	
	for (u32 i=1; i < HIT_STACK_SIZE; i++) if (m_hit_stack[i] != hit_success) {stack_similar = false; break;}

	if (!stack_similar) return;

	// обновить m_current_min_distance
	if (hit_success && (m_current_min_distance + m_as_step < m_min_attack_distance))	m_current_min_distance += m_as_step;
	if (!hit_success && (m_current_min_distance > m_as_min_dist + m_as_step)) 			m_current_min_distance -= m_as_step;
}
