////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 23.09.2003
//	Author		: Serge Zhem
//	Description : Animations for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_anim.h"


static void __stdcall vfPlayEndCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	tpBiting->MotionMan.OnAnimationEnd();
}

void CAI_Biting::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
{
	if (g_Alive() && MotionMan.PrepareAnimation()) {
		PKinematics(Visual())->PlayCycle(MotionMan.m_tpCurAnim,TRUE,vfPlayEndCallBack,this);
	}
}

void CAI_Biting::CheckAttackHit()
{
	// Проверка состояния анимации (атака)
	TTime cur_time = Level().timeServer();
	SAttackAnimation apt_anim;

	bool bGoodTime = MotionMan.AA_CheckTime(cur_time,apt_anim);

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
				MotionMan.AA_UpdateLastAttack(cur_time);
			}
		}
		this->setEnabled(true);			
		
		// если жертва убита - добавить в список трупов	
		if (!ve.obj->g_Alive()) AddCorpse(ve);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
//		NEW ANIMATION MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////
CMotionManager::CMotionManager()
{
}

void CMotionManager::Init (CAI_Biting	*pM, CKinematics *tpKin)
{
	pMonster				= pM;
	tpKinematics			= tpKin;
	
	prev_anim				= cur_anim	= eAnimStandIdle; 
	m_tAction				= ACT_STAND_IDLE;
	m_tpCurAnim				= 0;
	spec_params				= 0;

	// сейчас всё должно грузиться на NetSpawn - исправить!
	m_tAnims.clear			();
	m_tTransitions.clear	();
	m_tMotions.clear		();

	Seq_Init				();
	AA_Clear				();
}

void CMotionManager::Destroy()
{

}

// Загрузка параметров анимации
void CMotionManager::AddAnim(EMotionAnim ma, LPCTSTR tn, int s_id, float speed, float r_speed, EPState p_s)
{
	SAnimItem new_item;

	strcpy					(new_item.target_name,tn);
	new_item.spec_id		= s_id;
	new_item.speed.linear	= speed;
	new_item.speed.angular	= r_speed;
	new_item.pos_state		= p_s;

	Load					(new_item.target_name, &new_item.pMotionVect);

	m_tAnims.insert			(std::make_pair(ma, new_item));
}

void CMotionManager::AddTransition(EMotionAnim from, EMotionAnim to, EMotionAnim trans, bool chain)
{
	STransition new_item;

	new_item.ps_from_used		= false;
	new_item.anim_from			= from;

	new_item.ps_target_used		= false;
	new_item.anim_target		= to;
	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	m_tTransitions.push_back(new_item);
}


void CMotionManager::AddTransition(EMotionAnim from, EPState to, EMotionAnim trans, bool chain)
{
	STransition new_item;

	new_item.ps_from_used		= false;
	new_item.anim_from			= from;

	new_item.ps_target_used		= true;
	new_item.state_target		= to;
	
	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	m_tTransitions.push_back(new_item);
}

void CMotionManager::AddTransition(EPState from, EMotionAnim to, EMotionAnim trans, bool chain)
{
	STransition new_item;

	new_item.ps_from_used		= true;
	new_item.state_from			= from;

	new_item.ps_target_used		= false;
	new_item.anim_target		= to;
	
	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	m_tTransitions.push_back(new_item);
}

void CMotionManager::AddTransition(EPState from, EPState to, EMotionAnim trans, bool chain)
{
	STransition new_item;

	new_item.ps_from_used		= true;
	new_item.state_from			= from;

	new_item.ps_target_used		= true;
	new_item.state_target		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	m_tTransitions.push_back(new_item);
}

void CMotionManager::LinkAction(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle)
{
	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= true;
	new_item.turn.anim_left		= pmt_left;
	new_item.turn.anim_right	= pmt_right;
	new_item.turn.min_angle		= pmt_angle;

	m_tMotions.insert	(std::make_pair(act, new_item));
}

void CMotionManager::LinkAction(EAction act, EMotionAnim pmt_motion)
{
	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= false;

	m_tMotions.insert	(std::make_pair(act, new_item));
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

// Устанавливает текущую анимацию, которую необходимо проиграть.
// Возвращает false, если в смене анимации нет необходимости
bool CMotionManager::PrepareAnimation()
{
	if (m_tpCurAnim != 0) return false;

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

	// установить анимацию
	m_tpCurAnim = anim_it->second.pMotionVect[index];

	// установить параметры атаки
	AA_SwitchAnimation(cur_anim, index);

	return true;
}

bool CMotionManager::CheckTransition(EMotionAnim from, EMotionAnim to)
{
	// поиск соответствующего перехода
	bool		bActivated	= false;
	EMotionAnim cur_from = from; 
	EPState		state_from	= GetState(cur_from);
	EPState		state_to	= GetState(to);

	TRANSITION_ANIM_VECTOR_IT I = m_tTransitions.begin();
	bool bVectEmpty = !m_tTransitions.empty();

	
	while (!bVectEmpty) {		// вход в цикл, если вектор переходов не пустой
		
		bool from_is_good	= ((I->ps_from_used) ? (I->state_from == state_from) : (I->anim_from == cur_from));
		bool target_is_good = ((I->ps_target_used) ? (I->state_target == state_to) : (I->anim_target == to));

		if (from_is_good && target_is_good) {
			
			// переход годится
			Seq_Add(I->anim_transition);
			bActivated	= true;	

			if (I->chain) {
				cur_from	= I->anim_transition;
				state_from	= GetState(cur_from);
				I = m_tTransitions.begin();			// начать сначала
			} else break;
		}
		if ((++I) == m_tTransitions.end()) break;
	}

	if (bActivated) Seq_Switch();
	return bActivated;
}

// В соответствии текущему действию m_tAction, назначить соответсвующию анимацию и параметры движения
// выполняется на каждом шед.апдейте, после выполнения состояния
void CMotionManager::ProcessAction()
{
	// Если последовательность активирована
	if (seq_playing) {
		cur_anim = *seq_it;
	} else {
		
		// преобразовать Action в Motion и получить новую анимацию
		SMotionItem MI = m_tMotions[m_tAction];
		cur_anim = MI.anim;

		// установить target.yaw
		if (!pMonster->AI_Path.TravelPath.empty()) pMonster->SetDirectionLook( ((spec_params & ASP_MOVE_BKWD) == ASP_MOVE_BKWD) );

		// проверить необходимость поворота
		float &cur_yaw		= pMonster->r_torso_current.yaw;
		float &target_yaw	= pMonster->r_torso_target.yaw;
		if (MI.is_turn_params)
			if (!getAI().bfTooSmallAngle(cur_yaw, target_yaw, MI.turn.min_angle)) {
				// повернуться
				// необходим поворот влево или вправо
				if (angle_normalize_signed(target_yaw - cur_yaw) > 0) {
					// вправо
					cur_anim = MI.turn.anim_right;
				} else {
					// влево
					cur_anim = MI.turn.anim_left;
				}
			}
			// Проверить ASP
			if (CheckSpecParams()) return;
			
			// если новая анимация не совпадает с предыдущей, проверить переход
			if (prev_anim != cur_anim) {
				if (CheckTransition	(prev_anim, cur_anim)) return;
			}
	} // sequence playing
	
	ApplyParams();

	// если установленная анимация отличается от предыдущей - установить новую анимацию
	if (cur_anim != prev_anim) {
		FORCE_ANIMATION_SELECT();		
	}

	prev_anim	= cur_anim;
	spec_params = 0;
}

// возвращает true, если после выполнения этой функции необходимо прервать обработку
bool CMotionManager::CheckSpecParams()
{
	if ((spec_params & ASP_DRAG_CORPSE) == 	ASP_DRAG_CORPSE) {
		cur_anim = eAnimDragCorpse;
	}

	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		Seq_Add(eAnimCheckCorpse);
		Seq_Switch();
		return true;
	}
	
	return (pMonster->CheckSpecParams(spec_params));
}


// Установка линейной и угловой скоростей для cur_anim
void CMotionManager::ApplyParams()
{
	ANIM_ITEM_MAP_IT	item_it = m_tAnims.find(cur_anim);
	R_ASSERT(item_it != m_tAnims.end());
	
	pMonster->m_fCurSpeed		= item_it->second.speed.linear;
	pMonster->r_torso_speed		= item_it->second.speed.angular;
}

// Callback на завершение анимации
void CMotionManager::OnAnimationEnd() 
{ 
	m_tpCurAnim = 0;
	if (seq_playing) Seq_Switch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Работа с последовательностями
void CMotionManager::Seq_Init()
{
	seq_states.clear	();
	seq_it				= seq_states.end();
	seq_playing			= false;	
}

void CMotionManager::Seq_Add(EMotionAnim a)
{
	seq_states.push_back(a);
}

void CMotionManager::Seq_Switch()
{
	if (!seq_playing) {
		// активация последовательностей
		seq_it = seq_states.begin();
	} else {
		seq_it++; 
		if (seq_it == seq_states.end()) {
			Seq_Finish();
			return;
		}
	}

	seq_playing = true;
	
	// установить параметры
	cur_anim	= *seq_it;
	ApplyParams ();
}

void CMotionManager::Seq_Finish()
{
	Seq_Init(); 
	ProcessAction();	// выполнить текущие установки
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attack Animation
void CMotionManager::AA_Clear()
{ 
	aa_time_started		= 0;
	aa_time_last_attack	= 0;
	aa_all.clear		(); 
	aa_stack.clear		(); 
}

void CMotionManager::AA_PushAttackAnim(SAttackAnimation AttackAnim)
{
	aa_all.push_back(AttackAnim);
}

void CMotionManager::AA_PushAttackAnim(EMotionAnim a, u32 i3, TTime from, TTime to, Fvector &ray, float dist, float damage, float yaw, float pitch, u32 flags)
{
	SAttackAnimation anim;
	anim.anim			= a;
	anim.anim_i3		= i3;

	anim.time_from		= from;
	anim.time_to		= to;

	anim.trace_offset	= ray;
	anim.dist			= dist;

	anim.damage			= damage;
	anim.dir_yaw		= yaw;
	anim.dir_pitch		= pitch;
	anim.flags			= flags;

	AA_PushAttackAnim	(anim);
}

void CMotionManager::AA_SwitchAnimation(EMotionAnim anim, u32 i3)
{
	aa_time_started = Level().timeServer();
	aa_stack.clear();

	// найти в m_all анимации с параметрами (anim,i3) и заполнить m_stack
	ATTACK_ANIM_IT I = aa_all.begin();
	ATTACK_ANIM_IT E = aa_all.end();

	for (;I!=E; I++) {
		if ((I->anim == anim) && (I->anim_i3 == i3)) {
			aa_stack.push_back(*I);
		}
	}
}

// Сравнивает тек. время с временами хитов в стеке
bool CMotionManager::AA_CheckTime(TTime cur_time, SAttackAnimation &anim)
{
	// Частота хитов не может быть больше 'time_delta'
	TTime time_delta = 1000;

	if (aa_stack.empty()) return false;
	if (aa_time_last_attack + time_delta > cur_time) return false;

	ATTACK_ANIM_IT I = aa_stack.begin();
	ATTACK_ANIM_IT E = aa_stack.end();

	for (;I!=E; I++) if ((aa_time_started + I->time_from <= cur_time) && (cur_time <= aa_time_started + I->time_to)) {
		anim = (*I);
		return true;
	}
	return false;
}

EPState	CMotionManager::GetState (EMotionAnim a)
{
	// найти анимацию 
	ANIM_ITEM_MAP_IT  item_it = m_tAnims.find(a);
	R_ASSERT(item_it != m_tAnims.end());

	return item_it->second.pos_state;
}
