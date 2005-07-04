////////////////////////////////////////////////////////////////////////////
//	Module 		: base_monster_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 23.09.2003
//	Author		: Serge Zhem
//	Description : Animations for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "base_monster.h"
#include "../../../../skeletonanimated.h"
#include "../../../sound_player.h"
#include "../../../ai_monster_space.h"
#include "../control_animation_base.h"

// ��������� ��������
void CBaseMonster::SelectAnimation(const Fvector &/**_view/**/, const Fvector &/**_move/**/, float /**speed/**/)
{
	control().animation().update_frame();
}


void CBaseMonster::AA_CheckHit()
{
	if (!EnemyMan.get_enemy()) return;
	const CEntityAlive *enemy = EnemyMan.get_enemy();

	SAAParam params;
	if (!anim().AA_TimeTest(params))  return;
	
	sound().play(MonsterSpace::eMonsterSoundAttackHit);

	bool should_hit = true;
	// ���������� ��������� �� �����
	Fvector d;
	d.sub(enemy->Position(),Position());
	if (d.magnitude() > params.dist) should_hit = false;
	
	// �������� ��  Field-Of-Hit
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
	anim().AA_UpdateLastAttack	(Device.dwTimeGlobal);
}

