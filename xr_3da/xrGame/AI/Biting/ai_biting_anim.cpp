////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 23.09.2003
//	Author		: Serge Zhem
//	Description : Animations for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

static void __stdcall vfPlayEndCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	tpBiting->MotionMan.OnAnimationEnd();
}

// ��������� ��������
void CAI_Biting::SelectAnimation(const Fvector &/**_view/**/, const Fvector &/**_move/**/, float /**speed/**/)
{
	if (MotionMan.PrepareAnimation()) {
		PSkeletonAnimated(Visual())->PlayCycle(MotionMan.m_tpCurAnim,TRUE,vfPlayEndCallBack,this);
	}
}

bool CAI_Biting::AA_CheckHit()
{
	// �������� ��������� �������� (�����)
	SAttackAnimation apt_anim;
	bool was_hit = false;

	if (MotionMan.AA_CheckTime(m_dwCurrentTime,apt_anim)) {
		SetSoundOnce(SND_TYPE_ATTACK_HIT, m_dwCurrentTime);

		VisionElem ve;
		if (!GetEnemy(ve)) return false;
		const CObject *obj = dynamic_cast<const CObject *>(ve.obj);

		// ������� �� ��������� ��������� � �������
		Fvector trace_from;
		XFORM().transform_tiny(trace_from, apt_anim.trace_from);
		
		Fvector trace_to;
		XFORM().transform_tiny(trace_to, apt_anim.trace_to);

		// ���������� ������� ����������� �������� ����
		Fvector dir;
		dir.sub(trace_to, trace_from);
		float dist = dir.magnitude();
		dir.normalize_safe();

		// ������ �������� �� pitch (fix it)
		Fvector new_dir;
		Fvector C1, C2;
		Center(C1);
		obj->Center(C2);
		new_dir.sub(C2,C1);
		new_dir.normalize();
		dir.y = new_dir.y;
		dir.normalize_safe();

		// ������� �� ��������� ��������� � ������� ������� ����������� ��������
		Fvector hit_dir;
		XFORM().transform_dir(hit_dir,apt_anim.hit_dir);
		hit_dir.normalize();

#ifndef SIMPLE_ENEMY_HIT_TEST
		if (RayPickEnemy(obj, trace_from, dir, dist, 0.2f, 20)) {
			HitEntity(ve.obj, apt_anim.damage, hit_dir);
			was_hit = true;
		}
#else
		dist = 20.f;
		
		bool should_hit = true;
		Fvector d;
		d.sub(C2,C1);
		if (d.magnitude() > apt_anim.dist) should_hit = false;
		
		float my_h,my_p;
		float h,p;

		Direction().getHP(my_h,my_p);
		d.getHP(h,p);
		
		float from	= angle_normalize(my_h + apt_anim.yaw_from);
		float to	= angle_normalize(my_h + apt_anim.yaw_to);
		
		if (!is_angle_between(h, from, to)) should_hit = false;
		
		from	= angle_normalize(my_p + apt_anim.pitch_from);
		to		= angle_normalize(my_p + apt_anim.pitch_to);

		if (!is_angle_between(p, from, to)) should_hit = false;

		if (should_hit) {
			HitEntity(ve.obj, apt_anim.damage, hit_dir);
			was_hit = true;
		}
#endif
		
		// ���� ������ ����� - �������� � ������ ������	
		if (!ve.obj->g_Alive()) AddCorpse(ve);

		if (AS_Active()) AS_Check(was_hit);
		MotionMan.AA_UpdateLastAttack(m_dwCurrentTime);
	}
	return was_hit;
}

