#include "stdafx.h"
#include "jump_ability.h"
#include "anim_triple.h"

CJumpingAbility::CJumpingAbility()
{
	m_animation_holder = xr_new<CAnimTriple>();
}

CJumpingAbility::~CJumpingAbility()
{
	xr_delete(m_animation_holder);
}

void CJumpingAbility::init_external(CMotionDef *m_def1, CMotionDef *m_def2, CMotionDef *m_def3)
{
	m_animation_holder->init_external(m_def1, m_def2, m_def3);
}

void CJumpingAbility::jump(Fvector target)
{
	//// получить время физ.прыжка
	//float ph_time = m_object->m_PhysicMovementControl->JumpMinVelTime(target);
	//// выполнить прыжок в соответствии с делителем времени
	//m_object->m_PhysicMovementControl->Jump(target,ph_time/m_jump_factor);

	//m_time_started		= Level().timeServer();
	//m_time_next_allowed	= m_time_started + m_delay_after_jump;
}

void CJumpingAbility::update_frame()
{

}

void CJumpingAbility::update_scheduled()
{

}
void CJumpingAbility::pointbreak()
{
	m_animation_holder->pointbreak();
}

Fvector CJumpingAbility::get_target(CObject *obj)
{
	//u16 bone_id			= smart_cast<CKinematics*>(obj->Visual())->LL_BoneID			("bip01_head");
	//CBoneInstance &bone = smart_cast<CKinematics*>(obj->Visual())->LL_GetBoneInstance	(bone_id);

	//Fmatrix	global_transform;
	//global_transform.set	(obj->XFORM());
	//global_transform.mulB	(bone.mTransform);
	//
	//Fvector target_pos		= global_transform.c;
	//target_pos.y			-= obj->Radius() / 2;

	//return target_pos;

	return Fvector().set(0.f,0.f,0.f);
}

