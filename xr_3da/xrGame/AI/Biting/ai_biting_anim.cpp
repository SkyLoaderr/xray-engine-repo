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

// Установка анимации
void CAI_Biting::SelectAnimation(const Fvector &/**_view/**/, const Fvector &/**_move/**/, float /**speed/**/)
{
	if (MotionMan.PrepareAnimation()) {
		PSkeletonAnimated(Visual())->PlayCycle(MotionMan.m_tpCurAnim,TRUE,vfPlayEndCallBack,this);
	}
}

bool CAI_Biting::AA_CheckHit()
{
	// Проверка состояния анимации (атака)
	SAttackAnimation apt_anim;
	bool was_hit = false;

	if (MotionMan.AA_CheckTime(m_dwCurrentTime,apt_anim)) {
		SetSoundOnce(SND_TYPE_ATTACK_HIT, m_dwCurrentTime);

		VisionElem ve;
		if (!GetEnemy(ve)) return false;
		CObject *obj = dynamic_cast<CObject *>(ve.obj);

		// перевод из локальных координат в мировые
		Fvector trace_from;
		XFORM().transform_tiny(trace_from, apt_anim.trace_from);
		
		Fvector trace_to;
		XFORM().transform_tiny(trace_to, apt_anim.trace_to);

		// вычисление вектора направления проверки хита
		Fvector dir;
		dir.sub(trace_to, trace_from);
		float dist = dir.magnitude();
		dir.normalize();

		// делаем поправку на pitch (fix it)
		Fvector new_dir;
		Fvector C1, C2;
		Center(C1);
		obj->Center(C2);
		new_dir.sub(C2,C1);
		new_dir.normalize();
		dir.y = new_dir.y;
		dir.normalize();

		// перевод из локальных координат в мировые вектора направления импульса
		Fvector hit_dir;
		XFORM().transform_dir(hit_dir,apt_anim.hit_dir);
		hit_dir.normalize();

		if (RayPickEnemy(obj, trace_from, dir, dist, 0.2f, 20)) {
			HitEntity(ve.obj, apt_anim.damage, hit_dir);
			was_hit = true;
		}
		
		// если жертва убита - добавить в список трупов	
		if (!ve.obj->g_Alive()) AddCorpse(ve);

		if (AS_Active()) AS_Check(was_hit);
		MotionMan.AA_UpdateLastAttack(m_dwCurrentTime);
	}
	return was_hit;
}

