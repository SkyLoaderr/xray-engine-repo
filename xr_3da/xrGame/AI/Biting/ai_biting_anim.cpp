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
void CAI_Biting::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
{
	if (MotionMan.PrepareAnimation()) {
		PSkeletonAnimated(Visual())->PlayCycle(MotionMan.m_tpCurAnim,TRUE,vfPlayEndCallBack,this);
		Msg("Animation Selected!!!");
	}
}

bool CAI_Biting::AA_CheckHit()
{
	// Проверка состояния анимации (атака)
	TTime cur_time = Level().timeServer();
	SAttackAnimation apt_anim;
	bool was_hit = false;

	if (MotionMan.AA_CheckTime(cur_time,apt_anim)) {
		SetSoundOnce(SND_TYPE_ATTACK_HIT, cur_time);

		VisionElem ve;
		if (!GetEnemy(ve)) return false;
		CObject *obj = dynamic_cast<CObject *>(ve.obj);

		// определить точку, откуда будем пускать луч 
		Fvector trace_from;
		Center(trace_from);
		trace_from.add(apt_anim.trace_offset);

		this->setEnabled(false);
		Collide::ray_query	l_rq;

		if (Level().ObjectSpace.RayPick(trace_from, Direction(), apt_anim.dist, l_rq)) {
			if ((l_rq.O == obj) && (l_rq.range < apt_anim.dist)) {
				DoDamage(ve.obj, apt_anim.damage,apt_anim.dir_yaw,apt_anim.dir_pitch);
				was_hit = true;
			}
		}
		this->setEnabled(true);			
		
		// если жертва убита - добавить в список трупов	
		if (!ve.obj->g_Alive()) AddCorpse(ve);

		if (AS_Active()) {
			AS_Check(was_hit);
		}
		MotionMan.AA_UpdateLastAttack(cur_time);
	}
	return was_hit;
}

