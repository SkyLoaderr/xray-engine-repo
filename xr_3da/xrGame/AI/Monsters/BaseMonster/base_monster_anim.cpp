////////////////////////////////////////////////////////////////////////////
//	Module 		: base_monster_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 23.09.2003
//	Author		: Serge Zhem
//	Description : Animations for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "base_monster.h"
#include "../ai_monster_utils.h"
#include "../../../../skeletonanimated.h"
#include "../../../sound_player.h"
#include "../../../ai_monster_space.h"

static void __stdcall vfPlayEndCallBack(CBlend* B)
{
	CBaseMonster *pBaseMonster = (CBaseMonster*)B->CallbackParam;
	pBaseMonster->MotionMan.OnAnimationEnd();
}

// Установка анимации
void CBaseMonster::SelectAnimation(const Fvector &/**_view/**/, const Fvector &/**_move/**/, float /**speed/**/)
{
	
	SCurrentAnimationInfo &info = MotionMan.cur_anim_info();
	
	if (MotionMan.PrepareAnimation()) {
		info.blend							= MotionMan.m_tpCurAnim->PlayCycle(smart_cast<CSkeletonAnimated*>(Visual()), m_default_bone_part, TRUE, vfPlayEndCallBack, this);
		CStepManager::on_animation_start	(info.name, info.blend);
	}

	// установить скорость текущей анимации
	if (info.blend && (info.speed.current > 0.f)) {
		info.blend->speed = info.speed.current;
	}
}


void CBaseMonster::AA_CheckHit()
{
	if (!EnemyMan.get_enemy()) return;
	const CEntityAlive *enemy = EnemyMan.get_enemy();

	SAAParam params;
	if (!MotionMan.AA_TimeTest(params))  return;
	
	sound().play(MonsterSpace::eMonsterSoundAttackHit);

	bool should_hit = true;
	// определить дистанцию до врага
	Fvector d;
	d.sub(enemy->Position(),Position());
	if (d.magnitude() > params.dist) should_hit = false;
	
	// проверка на  Field-Of-Hit
	float my_h,my_p;
	float h,p;

	Direction().getHP(my_h,my_p);
	d.getHP(h,p);
	
	float from	= angle_normalize(my_h + params.foh.from_yaw);
	float to	= angle_normalize(my_h + params.foh.to_yaw);
	
	if (!is_angle_between(h, from, to)) should_hit = false;

	from	= angle_normalize(my_p + params.foh.from_pitch);
	to		= angle_normalize(my_p + params.foh.to_pitch);

	if (!is_angle_between(p, from, to)) should_hit = false;

	if (should_hit) HitEntity(enemy, params.hit_power, params.impulse, params.impulse_dir);
	
	MeleeChecker.on_hit_attempt		(should_hit);
	MotionMan.AA_UpdateLastAttack	(Device.dwTimeGlobal);
}

