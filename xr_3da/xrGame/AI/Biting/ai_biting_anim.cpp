////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"
#include "ai_biting_anim.h"

LPCSTR caBitingStateNames			[] = {
	"stand_",				// 0
	"sit_",					// 1
	"lie_",					// 2
	0
};

LPCSTR caBitingGlobalNames		[] = {
	"idle_",				// 0
	"idle_rs_",				// 1	
	"walk_fwd_",			// 2
	"walk_bkwd_",			// 3
	"walk_ls_",				// 4
	"walk_rs_",				// 5
	"run_",					// 6
	"run_ls_",				// 7
	"run_rs_",				// 8
	"attack_",				// 9
	"attack2_",				// 10
	"attack_ls_",			// 11
	"eat_",					// 12
	"damaged_",				// 13
	"scared_",				// 14
	"die_",					// 15	
	"lie_down_",			// 16
	"stand_up_",			// 17
	"liedown_eat_",			// 18
	"attack_jump_",			// 19
	0
};

static void __stdcall vfPlayCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	tpBiting->OnAnimationEnd();
}

void CAI_Biting::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
{
	
	// TODO: no magic numbers!!!
	if (bShowDeath)	{
		m_tpCurAnim = m_tAnimations.A[0].A[15].A[::Random.randI((int)m_tAnimations.A[0].A[15].A.size())];
		PKinematics(Visual())->PlayCycle(m_tpCurAnim,TRUE,vfPlayCallBack,this);
		bShowDeath  = false;
		return;
	}
	
	if (g_Alive())
		if (!m_tpCurAnim) {

			// преобразование названия анимации в индексы глобальной анимации
			int i1, i2, i3;
			i1 = i2 = i3 = 0;			// bug protection ;) todo: find out the reason
			MotionToAnim(m_tAnim,i1,i2,i3);
			if (i3 == -1) {
				i3 = ::Random.randI((int)m_tAnimations.A[i1].A[i2].A.size());
			}
		
			m_tpCurAnim = m_tAnimations.A[i1].A[i2].A[i3];
			PKinematics(Visual())->PlayCycle(m_tpCurAnim,TRUE,vfPlayCallBack,this);

			m_tAttackAnim.SwitchAnimation(m_dwCurrentUpdate,i1,i2,i3);
		}
}

void CAI_Biting::OnAnimationEnd()
{
	m_tpCurAnim = 0;
	Motion.m_tSeq.OnAnimEnd();
}

void CAI_Biting::ControlAnimation()
{
	if (!Motion.m_tSeq.Playing) {

		// __START: Bug protection 
		// Если нет пути и есть анимация движения, то играть анимацию отдыха
		if (AI_Path.TravelPath.empty() || ((AI_Path.TravelPath.size() - 1) <= AI_Path.TravelStart)) {
			if ((m_tAnim == eMotionWalkFwd) || (m_tAnim == eMotionRun)) {
				m_tAnim = eMotionStandIdle;
			}
		}

		// если стоит на месте и пытается бежать...
		int i = ps_Size();		
		if (i > 1) {
			CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
			if (tCurrentPosition.vPosition.similar(tPreviousPosition.vPosition)) {
				if ((m_tAnim == eMotionWalkFwd) || (m_tAnim == eMotionRun)) {
					m_tAnim = eMotionStandIdle;
				}
			}
		}
		// __END

		// если анимация изменилась, переназначить анимацию
		if (m_tAnimPrevFrame != m_tAnim) {
			FORCE_ANIMATION_SELECT();
		}	
	}
	//--------------------------------------

	// Сохранение предыдущей анимации
	m_tAnimPrevFrame = m_tAnim;
}




bool CAI_Biting::IsInMotion()
{
	if ((m_tAnim != eMotionStandTurnLeft) && (m_tAnim != eMotionWalkFwd) && (m_tAnim != eMotionWalkBkwd) && 
		(m_tAnim != eMotionWalkTurnLeft) && (m_tAnim != eMotionWalkTurnRight) && (m_tAnim != eMotionRun) && 
		(m_tAnim != eMotionRunTurnLeft) && (m_tAnim != eMotionRunTurnRight) && (m_tAnim != eMotionFastTurnLeft)) {
		return false;
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attack Animation
////////////////////////////////////////////////////////////////////////////////////////////////////////
void CAttackAnim::Clear()
{ 
	time_started		= 0;
	time_last_attack	= 0;
	m_all.clear			(); 
	m_stack.clear		(); 
}

void CAttackAnim::PushAttackAnim(SAttackAnimation AttackAnim)
{
	m_all.push_back(AttackAnim);
}

void CAttackAnim::PushAttackAnim(u32 i1, u32 i2, u32 i3, TTime from, TTime to, Fvector &ray, float dist, float damage, u32 flags)
{
	SAttackAnimation anim;

	anim.anim_i1		= i1;
	anim.anim_i2		= i2;
	anim.anim_i3		= i3;

	anim.time_from		= from;
	anim.time_to		= to;

	anim.trace_offset	= ray;
	anim.dist			= dist;

	anim.damage			= damage;
	anim.flags			= flags;

	PushAttackAnim		(anim);
}

void CAttackAnim::SwitchAnimation(TTime cur_time, u32 i1, u32 i2, u32 i3)
{
	time_started = cur_time;

	m_stack.clear();

	// найти в m_all анимации с индексами (i1,i2,i3) и заполнить m_stack
	ATTACK_ANIM_IT I = m_all.begin();
	ATTACK_ANIM_IT E = m_all.end();

	for (;I!=E; I++) {
		if ((I->anim_i1 == i1) && (I->anim_i2 == i2) && (I->anim_i3 == i3)) {
			m_stack.push_back(*I);
		}
	}
}

// Сравнивает тек. время с временами хитов в стеке
bool CAttackAnim::CheckTime(TTime cur_time, SAttackAnimation &anim)
{
	// Частота хитов не может быть больше 'time_delta'
	TTime time_delta = 1000;
	
	if (m_stack.empty()) return false;
	if (time_last_attack + time_delta > cur_time) return false;

	ATTACK_ANIM_IT I = m_stack.begin();
	ATTACK_ANIM_IT E = m_stack.end();

	for (;I!=E; I++) if ((time_started + I->time_from <= cur_time) && (cur_time <= time_started + I->time_to)) {
		anim = (*I);
		return true;
	}
	return false;
}

