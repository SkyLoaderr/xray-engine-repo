#include "stdafx.h"
#include "ai_monster_jump.h"
#include "..\\custommonster.h"

CJumping::CJumping()
{
	active	= false;
	state	= JS_NONE;
}

void CJumping::Init(CCustomMonster *pM)
{
	pMonster	= pM;
	active		= false;
	state		= JS_NONE;
}

void CJumping::Load(LPCSTR section)
{
	m_fJumpFactor					= pSettings->r_float(section,"jump_factor");
	m_fJumpMinDist					= pSettings->r_float(section,"jump_min_dist");		
	m_fJumpMaxDist					= pSettings->r_float(section,"jump_max_dist");		
	m_fJumpMaxAngle					= pSettings->r_float(section,"jump_max_angle");
	m_dwDelayAfterJump				= pSettings->r_u32	(section,"jump_delay");
}


// Проверка на возможность прыжка
bool CJumping::CheckJump(Fvector from_pos, Fvector to_pos, CObject *pO)
{
	if (CanJump() || active || (time_next_allowed > pMonster->m_dwCurrentTime)) return false;

	// растояние до цели	
	float dist = from_pos.distance_to(to_pos);	

	// получить вектор направления и его мир угол
	Fvector dir;
	float  dest_yaw, pitch;
	dir.sub(to_pos, from_pos);
	dir.getHP(dest_yaw, pitch);
	dest_yaw *= -1;
	dest_yaw = angle_normalize(dest_yaw);

	// проверка на max_angle и на dist
	if (!getAI().bfTooSmallAngle(pMonster->r_torso_current.yaw, dest_yaw, m_fJumpMaxAngle) || !(m_fJumpMinDist <=dist && dist <= m_fJumpMaxDist)) return false;

	// можно прыгать; инициализировать параметры прыжка
	active			= true;

	target_yaw		= dest_yaw;
	this->from_pos	= from_pos;
	target_pos		= to_pos;
	entity			= pO;
	ph_time			= 0.f;

	pMonster->r_torso_target.yaw = target_yaw;

	return true;
}

void CJumping::SwitchJumpState()
{
	if (!active) return;

	if (state == JS_NONE) state = (((states_used & JUMP_PREPARE_USED) == JUMP_PREPARE_USED) ?	JS_PREPARE : JS_JUMP);
	else if (state == JS_PREPARE) state = JS_JUMP;
	else if (state == JS_JUMP) state = (((states_used & JUMP_FINISH_USED) == JUMP_FINISH_USED) ? JS_FINISH : JS_NONE);

	switch (state) {
	case JS_PREPARE:
		PrepareJump();
		break;
	case JS_JUMP:
		ExecuteJump();
		break;
	case JS_FINISH:
		FinishJump();
		break;
	case JS_NONE:
		active = false;
		break;
	}
} 

void CJumping::PrepareJump()
{
}

void CJumping::FinishJump()
{
}

void CJumping::ExecuteJump()
{
	if (entity) {
		// put target up to the head
		u16 bone_id = PKinematics(entity->Visual())->LL_BoneID("bip01_head");
		CBoneInstance &bone = PKinematics(entity->Visual())->LL_GetBoneInstance(bone_id);

		Fmatrix global_transform;
		global_transform.set(entity->XFORM());
		global_transform.mulB(bone.mTransform);
		target_pos = global_transform.c;
		target_pos.y /= 1.5f;
	}
	
	// get time of jump
	ph_time = pMonster->Movement.JumpMinVelTime(target_pos);
	pMonster->Movement.Jump(target_pos,ph_time/m_fJumpFactor);
	
	time_started		= pMonster->m_dwCurrentTime;
	time_next_allowed	= time_started + m_dwDelayAfterJump;
}

