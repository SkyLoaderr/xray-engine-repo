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

static void __stdcall vfPlayEndCallBack(CBlend* B)
{
	CBaseMonster *pBaseMonster = (CBaseMonster*)B->CallbackParam;
	pBaseMonster->MotionMan.OnAnimationEnd();
}

// ��������� ��������
void CBaseMonster::SelectAnimation(const Fvector &/**_view/**/, const Fvector &/**_move/**/, float /**speed/**/)
{
	SCurrentAnimationInfo &info = MotionMan.cur_anim_info();
	
	if (MotionMan.PrepareAnimation()) {
		info.blend		= MotionMan.m_tpCurAnim->PlayCycle(smart_cast<CSkeletonAnimated*>(Visual()), bone_part, TRUE, vfPlayEndCallBack, this);
		Msg("Time = [%u], anim_name = [%s]", Level().timeServer(), smart_cast<CSkeletonAnimated*>(Visual())->LL_MotionDefName_dbg(info.blend->motionID));
	}

	// ���������� �������� ������� ��������
	if (info.blend && (info.speed.current > 0.f)) {
		info.blend->speed = info.speed.current;
	}
}


void CBaseMonster::AA_CheckHit()
{
	SAAParam params;

	if (!EnemyMan.get_enemy()) return;
	const CObject *obj = smart_cast<const CObject *>(EnemyMan.get_enemy());

	if (!MotionMan.AA_TimeTest(params))  return;
	
	CSoundPlayer::play(MonsterSpace::eMonsterSoundAttackHit);

	bool should_hit = true;
	// ���������� ��������� �� �����
	Fvector d;
	d.sub(obj->Position(),Position());
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

	const CEntityAlive *entity = EnemyMan.get_enemy();
	if (should_hit) HitEntity(entity, params.hit_power, params.impulse, params.impulse_dir);
	
	// ���� ������ ����� - �������� � ������ ������	
	if (!entity->g_Alive()) CorpseMemory.add_corpse(entity);

	if (AS_Active()) AS_Check(should_hit);
	MotionMan.AA_UpdateLastAttack(m_dwCurrentTime);
}

