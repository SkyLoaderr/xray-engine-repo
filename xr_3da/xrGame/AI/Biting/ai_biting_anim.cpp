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
	"idle_",				// 0			// 0	 
	"idle_rs_",				// 1			// 1	
	"idle_ls_",				// 20			// 2
											
	"walk_fwd_",			// 2			// 3
	"walk_bkwd_",			// 3			// 4
	"walk_ls_",				// 4			// 5
	"walk_rs_",				// 5			// 6
											
	"run_",					// 6			// 7
	"run_ls_",				// 7			// 8
	"run_rs_",				// 8			// 9
											
	"attack_",				// 9			// 10
	"attack2_",				// 10			// 11
	"attack_jump_",			// 19			// 12
	
	"fast_turn_ls_",		// 11			// 13
										
	"eat_",					// 12			// 14
											
	"damaged_",				// 13			// 15
	"scared_",				// 14			// 16
	"die_",					// 15			// 17	
											
	"sit_down_",			// 23			// 18
	"to_sleep_",			// 24			// 19
	"lie_down_",			// 16			// 20
	"stand_up_",			// 17			// 21
	"liedown_eat_",			// 18			// 22

	"to_sleep_",			// 21			// 23 
	"sleep_",				// 22			// 24 
	0
};

static void __stdcall vfPlayCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	tpBiting->OnAnimationEnd();
}

void CAI_Biting::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
{
	// преобразование названия анимации в индексы глобальной анимации
	int i1,i2,i3;

	if (bShowDeath)	{
		MotionToAnim(eAnimDie,i1,i2,i3);
		m_tpCurAnim = m_tAnimations.A[i1].A[i2].A[i3];
		PKinematics(Visual())->PlayCycle(m_tpCurAnim,TRUE,vfPlayCallBack,this);
		bShowDeath  = false;
		return;
	}
	
	TTime cur_time = Level().timeServer();

	if (g_Alive())
		if (!m_tpCurAnim && !IsAnimLocked(cur_time)) {
			MotionToAnim(m_tAnim,i1,i2,i3);
			m_tpCurAnim = m_tAnimations.A[i1].A[i2].A[i3];
			PKinematics(Visual())->PlayCycle(m_tpCurAnim,TRUE,vfPlayCallBack,this);
			m_tAttackAnim.SwitchAnimation(cur_time,i1,i2,i3);

			m_tAnimPlaying	= m_tAnim;
			m_dwAnimStarted = cur_time;
			anim_i3			= i3;
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
		// Если нет пути и есть анимация движения, то играть анимацию стоять на месте
		if (AI_Path.TravelPath.empty() || ((AI_Path.TravelPath.size() - 1) <= AI_Path.TravelStart)) {
			if ((m_tAnim == eAnimWalkFwd) || (m_tAnim == eAnimRun)) {
				m_tAnim = eAnimStandIdle;
			}
		}

		// если стоит на месте и пытается бежать...
		int i = ps_Size();		
		if (i > 1) {
			CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
			if (tCurrentPosition.vPosition.similar(tPreviousPosition.vPosition)) {
				if ((m_tAnim == eAnimWalkFwd) || (m_tAnim == eAnimRun)) {
					m_tAnim = eAnimStandIdle;
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
	if ((m_tAnim != eAnimStandTurnLeft) && (m_tAnim != eAnimWalkFwd) && (m_tAnim != eAnimWalkBkwd) && 
		(m_tAnim != eAnimWalkTurnLeft) && (m_tAnim != eAnimWalkTurnRight) && (m_tAnim != eAnimRun) && 
		(m_tAnim != eAnimRunTurnLeft) && (m_tAnim != eAnimRunTurnRight) && (m_tAnim != eAnimFastTurn)) {
		return false;
	}
	return true;
}

void CAI_Biting::CheckAttackHit()
{
	// Проверка состояния анимации (атака)
	TTime cur_time = Level().timeServer();
	SAttackAnimation apt_anim;

	bool bGoodTime = m_tAttackAnim.CheckTime(cur_time,apt_anim);

	if (bGoodTime) {
		VisionElem ve;
		if (!GetEnemy(ve)) return;
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
				m_tAttackAnim.UpdateLastAttack(cur_time);
			}
		}
		this->setEnabled(true);			

		if (!ve.obj->g_Alive()) AddCorpse(ve);
	}
}

void CAI_Biting::LockAnim(EMotionAnim anim, int i3, TTime from, TTime to)
{
	SLockAnim S;  
	
	S.anim	= anim;
	S.i3	= i3;
	S.from	= from; 
	S.to	= to;
	
	m_tLockedAnims.push_back(S); 
}


bool CAI_Biting::IsAnimLocked(TTime cur_time)
{
	for (LOCK_ANIM_IT I = m_tLockedAnims.begin(); I != m_tLockedAnims.end(); I++) 
		if (I->anim == m_tAnimPlaying && I->i3 == anim_i3) {
			if ((I->from + m_dwAnimStarted < cur_time) && (cur_time < I->to + m_dwAnimStarted)) return true;
			else return false;
		}
	return false;
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

void CAttackAnim::PushAttackAnim(u32 i1, u32 i2, u32 i3, TTime from, TTime to, Fvector &ray, float dist, float damage, float yaw, float pitch, u32 flags)
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
	anim.dir_yaw		= yaw;
	anim.dir_pitch		= pitch;
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

