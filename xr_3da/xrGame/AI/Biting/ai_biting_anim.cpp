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
	if (g_Alive()) {
		PKinematics(Visual())->PlayCycle(m_tpCurAnim,TRUE,vfPlayCallBack,this);

	}
}

void CAI_Biting::OnAnimationEnd()
{
	m_tpCurAnim = 0;
	Motion.m_tSeq.OnAnimEnd();
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







///////////////////////////////////////////////////////////////////////////////////////////
//
//		NEW ANIMATION MANAGMENT
//
///////////////////////////////////////////////////////////////////////////////////////////
CMotionManager::CMotionManager()
{
}

void CMotionManager::Init (CKinematics *tpKin)
{
	tpKinematics	= tpKin;

	prev_anim		= cur_anim	= eAnimStandIdle; 
	m_tAction		= ACT_STAND_IDLE;
	m_tpCurAnim		= 0;
}

void CMotionManager::Destroy()
{

}

// Загрузка параметров анимации
void CMotionManager::AddAnim(EMotionAnim ma, LPCTSTR tn, int s_id, float speed, float r_speed)
{
	SAnimItem new_item;

	strcpy					(new_item.target_name,tn);
	new_item.spec_id		= s_id;
	new_item.speed.linear	= speed;
	new_item.speed.angular	= r_speed;

	Load					(new_item.target_name, &new_item.pMotionVect);

	m_tAnims.insert			(std::make_pair(ma, new_item));
}

void CMotionManager::AddTransition(EMotionAnim from, EMotionAnim to, EMotionAnim trans, bool chain)
{
	STransition new_item;

	new_item.anim_from			= from;
	new_item.anim_target		= to;
	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	m_tTransitions.push_back(new_item);
}

// загрузка анимаций из модели начинающиеся с pmt_name в вектор pMotionVect
void CMotionManager::Load(LPCTSTR pmt_name, ANIM_VECTOR	*pMotionVect)
{
	anim_string	S1, S2;
	CMotionDef	*tpMotionDef;
	for (int i=0; ; i++) {
		if (0 != (tpMotionDef = tpKinematics->ID_Cycle_Safe(strconcat(S1,pmt_name,itoa(i,S2,10)))))  pMotionVect->push_back(tpMotionDef);
		else if (0 != (tpMotionDef = tpKinematics->ID_FX_Safe(strconcat(S1,pmt_name,itoa(i,S2,10))))) pMotionVect->push_back(tpMotionDef);
		else break;
	}
}

// Играть текущую анимацию определённую в cur_anim
void CMotionManager::Play()
{
	if (m_tpCurAnim) return;

	// получить элемент SAnimItem соответствующий cur_anim
	ANIM_ITEM_MAP_IT anim_it = m_tAnims.find(cur_anim);
	R_ASSERT(anim_it != m_tAnims.end());

	// определить необходимый индекс
	int index;
	if (anim_it->second.spec_id != -1) index = anim_it->second.spec_id;
	else {
		R_ASSERT(!anim_it->second.pMotionVect.empty());
		index = ::Random.randI(anim_it->second.pMotionVect.size());
	}

	// получить анимацию
	CMotionDef	*m_tpCurAnim = anim_it->second.pMotionVect[index];

	// проиграть 
	tpKinematics->PlayCycle(m_tpCurAnim,TRUE,vfPlayCallBack,this);
}


bool CMotionManager::CheckTransition(EMotionAnim from, EMotionAnim to)
{
	// поиск соответствующего перехода
	bool bActivated = false;
	EMotionAnim cur_from = from, cur_to = to;
//	float	speed, r_speed;

	for (u32 i=0; i<m_tTransitions.size(); i++) {
		if ((m_tTransitions[i].anim_from == cur_from) && (m_tTransitions[i].anim_target == cur_to)) {
			cur_from = m_tTransitions[i].anim_transition;

			//GetSpeedParams(cur_from, speed, r_speed);
			//m_tSeq.Add(cur_from,speed,r_speed,0,0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
			bActivated = true;	
		}
	}
	
	if (bActivated) {
		//m_tSeq.Switch();
		return true;
	}
	return false;
}

//void CMotionManager::ApplyParams(CAI_Biting *pM)
//{
//	ANIM_ITEM_MAP_IT	item_it = m_tAnims.find(cur_anim);
//	R_ASSERT(item_it != m_tAnims.end());
//	
//	pM->m_fCurSpeed		= item_it->second.speed.linear;
//	pM->r_torso_speed	= item_it->second.speed.angular;
//}

void CMotionManager::AddMotion(EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle)
{
	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= true;
	new_item.turn.anim_left		= pmt_left;
	new_item.turn.anim_right	= pmt_right;
	new_item.turn.min_angle		= pmt_angle;

	m_tMotions.push_back(new_item);
}

void CMotionManager::AddMotion(EMotionAnim pmt_motion)
{
	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= false;

	m_tMotions.push_back(new_item);
}

void CMotionManager::ProcessAction()
{

//	// преобразовать Action в Motion и получить новую анимацию
//	SMotionItem MI = m_tMotions[m_tAction];
//	cur_anim = MI.anim;
//
//	// если новая анимация не совпадает с предыдущей, проверить переход
//	if (prev_anim != cur_anim) {
//		if (CheckTransition	(prev_anim, cur_anim)) return;
//	}
//
//	if (MI.is_turn_params)
//		// проверить необходимость поворота
//		if (!getAI().bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw, MI.turn.min_angle)) {
//			// повернуться
//			// необходим поворот влево или вправо
//			if (angle_normalize_signed(r_torso_target.yaw - r_torso_current.yaw) > 0) {
//				// вправо
//				cur_anim = MI.turn.anim_right;
//			} else {
//				// влево
//				cur_anim = MI.turn.anim_left;
//			}
//		}
//
//		ApplyParams(this);					
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