////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 23.09.2003
//	Author		: Serge Zhem
//	Description : Animations for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../ai_monster_utils.h"

static void __stdcall vfPlayEndCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	tpBiting->MotionMan.OnAnimationEnd();
}

// ��������� ��������
void CAI_Biting::SelectAnimation(const Fvector &/**_view/**/, const Fvector &/**_move/**/, float /**speed/**/)
{
	if (MotionMan.PrepareAnimation()) {
		cur_blend = PSkeletonAnimated(Visual())->PlayCycle(MotionMan.m_tpCurAnim,TRUE,vfPlayEndCallBack,this);
	}

	if (anim_speed > 0.f && cur_blend) cur_blend->speed = anim_speed;
}


bool CAI_Biting::AA_CheckHit()
{
	SAAParam params;

	VisionElem ve;
	if (!GetEnemy(ve)) return false;
	const CObject *obj = dynamic_cast<const CObject *>(ve.obj);

	if (MotionMan.AA_TimeTest(params)) {
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

		if (should_hit) HitEntity(ve.obj, params.hit_power, params.impulse, params.impulse_dir);
		
		// ���� ������ ����� - �������� � ������ ������	
		if (!ve.obj->g_Alive()) AddCorpse(ve);

		if (AS_Active()) AS_Check(should_hit);
		MotionMan.AA_UpdateLastAttack(m_dwCurrentTime);

		if (should_hit) return true;
	}

	return false;
}

