#include "stdafx.h"
#include "jump_ability.h"
#include "anim_triple.h"
#include "BaseMonster/base_monster.h"
#include "../../PHMovementControl.h"
#include "../../../skeletonanimated.h"

CJumpingAbility::CJumpingAbility()
{
	m_animation_holder = xr_new<CAnimTriple>();
}

CJumpingAbility::~CJumpingAbility()
{
	xr_delete(m_animation_holder);
}

void CJumpingAbility::init_external(CBaseMonster *obj, CMotionDef *m_def1, CMotionDef *m_def2, CMotionDef *m_def3)
{
	m_animation_holder->init_external(m_def1, m_def2, m_def3);

	m_delay_after_jump	= 1000;
	m_jump_factor		= 1.5f;

	m_object			= obj;

	m_time_started		= 0;
	
	m_def_glide			= m_def2;
	m_jump_time			= 1.f;
}

void CJumpingAbility::jump(Fvector target)
{
	// получить время физ.прыжка
	float ph_time = m_object->m_PhysicMovementControl->JumpMinVelTime(target);
	// выполнить прыжок в соответствии с делителем времени
	m_object->m_PhysicMovementControl->Jump(target,ph_time/m_jump_factor);
	m_jump_time			= ph_time/m_jump_factor;
	m_time_started		= Level().timeServer();
	m_time_next_allowed	= m_time_started + m_delay_after_jump;
	
	m_object->MotionMan.TA_Activate(m_animation_holder);
	m_active			= true;

	m_blend_speed		= -1.f;
	
}

void CJumpingAbility::update_frame()
{
	if (!m_active) return;
	
	if (!m_object->MotionMan.TA_IsActive())		stop();
	//if ((m_time_started != 0) && (m_time_started + 1000 < Level().timeServer()))		{
	//	m_time_started = 0;
	//	pointbreak();
	//}
	
	if (is_landing()) {
		pointbreak		();
		m_time_started	= 0;
	}

	
	set_animation_speed();
	
	// test for hit
	//hit_test();

	//if (strike_in_jump) return;

	//const CEntity *pE = smart_cast<const CEntity *>(CJumping::GetEnemy());
	//if (!pE) return;

	//Fvector trace_from;
	//Center(trace_from);
	//setEnabled(false);
	//Collide::rq_result	l_rq;

	//if (Level().ObjectSpace.RayPick(trace_from, Direction(), trace_dist , Collide::rqtBoth, l_rq)) {
	//	if ((l_rq.O == CJumping::GetEnemy()) && (l_rq.range < trace_dist)) {
	//		SAAParam params;
	//		MotionMan.AA_GetParams(params, "jump_glide_0");
	//		HitEntity(pE, params.hit_power, params.impulse, params.impulse_dir);
	//		strike_in_jump = true;
	//	}
	//}
	//setEnabled(true);			

}
bool CJumpingAbility::is_landing()
{
	if (m_time_started == 0) return false;

	if (m_time_started + (m_jump_time*1000) < Level().timeServer()) return true;
	else return false;
}


void CJumpingAbility::set_animation_speed() 
{
	SCurrentAnimationInfo &info = m_object->MotionMan.cur_anim_info();
	if (!info.blend) return;
	
	if ((m_animation_holder->get_state() == eStateExecute) && (info.blend->motionID == m_def_glide->motion)) {
		if (m_blend_speed < 0)	m_blend_speed = info.blend->speed;
		info.speed.current = m_blend_speed * (info.blend->timeTotal / m_jump_time);
		Msg("Speed = [%f] Blend Time = [%f] Jump Time = [%f]", info.speed.current, info.blend->timeTotal, m_jump_time);
	} else {
		info.speed.current = -1.f;
	}

}


void CJumpingAbility::update_scheduled()
{

}
void CJumpingAbility::stop()
{
	m_active = false;
}

void CJumpingAbility::pointbreak()
{
	m_object->MotionMan.TA_PointBreak();
}

Fvector CJumpingAbility::get_target(CObject *obj)
{
	u16 bone_id			= smart_cast<CKinematics*>(obj->Visual())->LL_BoneID			("bip01_head");
	CBoneInstance &bone = smart_cast<CKinematics*>(obj->Visual())->LL_GetBoneInstance	(bone_id);

	Fmatrix	global_transform;
	global_transform.set	(obj->XFORM());
	global_transform.mulB	(bone.mTransform);
	
	Fvector target_pos		= global_transform.c;
	target_pos.y			-= obj->Radius() / 2;

	return target_pos;
}

