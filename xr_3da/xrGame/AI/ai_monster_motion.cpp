#include "stdafx.h"
#include "ai_monster_motion.h"
#include "ai_monster_jump.h"
#include "biting/ai_biting.h"
#include "ai_monster_utils.h"

// DEBUG purpose only
char *dbg_action_name_table[] = {
	"ACT_STAND_IDLE",
		"ACT_SIT_IDLE",
		"ACT_LIE_IDLE",
		"ACT_WALK_FWD",
		"ACT_WALK_BKWD",
		"ACT_RUN",
		"ACT_EAT",
		"ACT_SLEEP",
		"ACT_REST",
		"ACT_DRAG",
		"ACT_ATTACK",
		"ACT_STEAL",
		"ACT_LOOK_AROUND",
		"ACT_JUMP",
		"ACT_TURN",
};	


CMotionManager::CMotionManager() 
{
}

CMotionManager::~CMotionManager()
{
}


void CMotionManager::Init (CAI_Biting	*pM)
{
	pMonster				= pM;
	pJumping				= dynamic_cast<CJumping*>(pM);
	pVisual					= 0;

	prev_anim				= cur_anim	= eAnimStandIdle; 
	m_tAction				= ACT_STAND_IDLE;
	m_tpCurAnim				= 0;
	spec_params				= 0;

	time_start_stand		= 0;
	prev_action				= ACT_STAND_IDLE;

	should_play_die_anim	= true;			//этот флаг на NetSpawn должен устанавливаться в true

	Seq_Init				();

	AA_Clear				();

	b_end_transition		= false;
	saved_anim				= cur_anim;

	fx_time_last_play		= 0;
	
	b_forced_velocity		= false;
	
	accel_init				();
}

// Устанавливает текущую анимацию, которую необходимо проиграть.
// Возвращает false, если в смене анимации нет необходимости
bool CMotionManager::PrepareAnimation()
{
	if (pJumping && pJumping->IsActive())  return pJumping->PrepareAnimation(&m_tpCurAnim);
	if (0 != m_tpCurAnim) return false;

	// проверка на отыгрывание анимации смерти
	if (!pMonster->g_Alive()) 
		if (should_play_die_anim) {
			should_play_die_anim = false;  // отыграть анимацию смерти только раз
			if (_sd->m_tAnims.find(eAnimDie) != _sd->m_tAnims.end()) cur_anim = eAnimDie;
			else return false;
		} else return false;

	// получить элемент SAnimItem соответствующий cur_anim
	ANIM_ITEM_MAP_IT anim_it = _sd->m_tAnims.find(cur_anim);
	R_ASSERT(_sd->m_tAnims.end() != anim_it);

	// определить необходимый индекс
	int index;
	if (-1 != anim_it->second.spec_id) index = anim_it->second.spec_id;
	else {
		R_ASSERT(!anim_it->second.pMotionVect.empty());
		index = ::Random.randI(anim_it->second.pMotionVect.size());
	}

	// установить анимацию	
	m_tpCurAnim = anim_it->second.pMotionVect[index];
	//Msg("** Animation set = [%s]", *anim_it->second.target_name);

	// установить параметры атаки
	AA_SwitchAnimation(cur_anim, index);

	return true;
}

// проверить существует ли переход из анимации from в to
void CMotionManager::CheckTransition(EMotionAnim from, EMotionAnim to)
{
	// поиск соответствующего перехода
	bool		bActivated	= false;
	EMotionAnim cur_from = from; 
	EPState		state_from	= GetState(cur_from);
	EPState		state_to	= GetState(to);

	TRANSITION_ANIM_VECTOR_IT I = _sd->m_tTransitions.begin();
	bool bVectEmpty = _sd->m_tTransitions.empty();

	while (!bVectEmpty) {		// вход в цикл, если вектор переходов не пустой

		bool from_is_good	= ((I->from.state_used) ? (I->from.state == state_from) : (I->from.anim == cur_from));
		bool target_is_good = ((I->target.state_used) ? (I->target.state == state_to) : (I->target.anim == to));

		if (from_is_good && target_is_good) {

			// переход годится
			Seq_Add(I->anim_transition);
			bActivated	= true;	

			if (I->chain) {
				cur_from	= I->anim_transition;
				state_from	= GetState(cur_from);
				I = _sd->m_tTransitions.begin();			// начать сначала
				continue;
			} else break;
		}
		if (_sd->m_tTransitions.end() == ++I) break;
	}

	if (bActivated) {
		b_end_transition = true;		
		saved_anim = to;
		Seq_Switch();
	}
}


// Установка линейной и угловой скоростей для cur_anim
void CMotionManager::ApplyParams()
{
	ANIM_ITEM_MAP_IT	item_it = _sd->m_tAnims.find(cur_anim);
	R_ASSERT(_sd->m_tAnims.end() != item_it);

	//pMonster->m_fCurSpeed		= item_it->second.speed.linear;
	pMonster->m_velocity_linear.target	= item_it->second.velocity->velocity.linear;
	if (!b_forced_velocity) pMonster->m_velocity_angular.target = item_it->second.velocity->velocity.angular;

}

// Callback на завершение анимации
void CMotionManager::OnAnimationEnd() 
{ 
	m_tpCurAnim = 0;
	if (seq_playing) Seq_Switch();

	if (pJumping && pJumping->IsActive()) pJumping->OnAnimationEnd();
}

// если монстр стоит на месте и играет анимацию движения - force stand idle
void CMotionManager::FixBadState()
{	
//	if (!pMonster->MotionStats->is_good_motion(3) || (IsMoving() && !pMonster->IsMovingOnPath())) {
//		cur_anim = eAnimStandIdle;
//	}
}

void CMotionManager::CheckReplacedAnim()
{
	for (REPLACED_ANIM_IT it=m_tReplacedAnims.begin(); m_tReplacedAnims.end()!=it ;++it) 
		if ((cur_anim == it->cur_anim) && (*(it->flag) == true)) { 
			cur_anim = it->new_anim;
			return;
		}
}

bool CMotionManager::IsMoving()
{
	return ((ACT_WALK_FWD == m_tAction) || (ACT_WALK_BKWD == m_tAction) || (ACT_RUN == m_tAction) || 
		(ACT_DRAG == m_tAction) || (ACT_STEAL == m_tAction));
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
		++seq_it; 
		if (seq_states.end() == seq_it) {
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

	if (b_end_transition) { 
		prev_anim = cur_anim = saved_anim;
		b_end_transition = false;
	} else {
		prev_anim = cur_anim = _sd->m_tMotions[m_tAction].anim;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attack Animation
void CMotionManager::AA_Clear()
{ 
	aa_time_started		= 0;
	aa_time_last_attack	= 0;
	aa_stack.clear		(); 
}


void CMotionManager::AA_SwitchAnimation(EMotionAnim anim, u32 i3)
{
	aa_time_started = Level().timeServer();
	aa_stack.clear();

	// найти в m_all анимации с параметрами (anim,i3) и заполнить m_stack
	ATTACK_ANIM_IT I = _sd->aa_all.begin();
	ATTACK_ANIM_IT E = _sd->aa_all.end();

	for (;I!=E; ++I) {
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

	for (;I!=E; ++I) if ((aa_time_started + I->time_from <= cur_time) && (cur_time <= aa_time_started + I->time_to)) {
		anim = (*I);
		return true;
	}
	return false;
}


EPState	CMotionManager::GetState (EMotionAnim a)
{
	// найти анимацию 
	ANIM_ITEM_MAP_IT  item_it = _sd->m_tAnims.find(a);
	R_ASSERT(_sd->m_tAnims.end() != item_it);

	return item_it->second.pos_state;
}


void CMotionManager::PrepareSharing()
{
	CSharedClass<_motion_shared>::Prepare(pMonster->SUB_CLS_ID);
}

void CMotionManager::NotifyShareLoaded() 
{
	CSharedClass<_motion_shared>::Finish();
}

void CMotionManager::ForceAnimSelect() 
{
	m_tpCurAnim = 0; 
	pMonster->SelectAnimation(pMonster->Direction(),pMonster->Direction(),0);
}

void CMotionManager::UpdateVisual()
{
	pVisual = pMonster->Visual();
	LoadVisualData();
}	

#define FX_CAN_PLAY_MIN_INTERVAL	50

void CMotionManager::FX_Play(EHitSide side, float amount)
{
	if (fx_time_last_play + FX_CAN_PLAY_MIN_INTERVAL > pMonster->m_dwCurrentTime) return;

	ANIM_ITEM_MAP_IT anim_it = _sd->m_tAnims.find(cur_anim);
	R_ASSERT(_sd->m_tAnims.end() != anim_it);
	
	clamp(amount,0.f,1.f);

	ref_str	*p_str = 0;
	switch (side) {
		case eSideFront:	p_str = &anim_it->second.fxs.front;	break;
		case eSideBack:		p_str = &anim_it->second.fxs.back;	break;
		case eSideLeft:		p_str = &anim_it->second.fxs.left;	break;
		case eSideRight:	p_str = &anim_it->second.fxs.right;	break;
	}
	
	if (p_str && p_str->size()) PSkeletonAnimated(pVisual)->PlayFX(*(*p_str), amount);

	fx_time_last_play = pMonster->m_dwCurrentTime;
}


float CMotionManager::GetAnimTime(EMotionAnim anim, u32 index)
{
	// получить элемент SAnimItem соответствующий anim
	ANIM_ITEM_MAP_IT anim_it = _sd->m_tAnims.find(anim);
	R_ASSERT(_sd->m_tAnims.end() != anim_it);

	CMotionDef			*def = anim_it->second.pMotionVect[index];
	CBoneData			&bone_data = PKinematics(pVisual)->LL_GetData(0);
	CBoneDataAnimated	*bone_anim = dynamic_cast<CBoneDataAnimated *>(&bone_data);

	return  bone_anim->Motions[def->motion].GetLength();
}

float CMotionManager::GetAnimSpeed(EMotionAnim anim)
{
	ANIM_ITEM_MAP_IT anim_it = _sd->m_tAnims.find(anim);
	R_ASSERT(_sd->m_tAnims.end() != anim_it);

	CMotionDef *def = anim_it->second.pMotionVect[0];

	return def->Dequantize(def->speed);
}


void CMotionManager::ForceAngularSpeed(float vel)
{
	pMonster->m_velocity_angular.current = pMonster->m_velocity_angular.target = vel;
	b_forced_velocity = true;
}

bool CMotionManager::IsStandCurAnim()
{
	ANIM_ITEM_MAP_IT	item_it = _sd->m_tAnims.find(cur_anim);
	R_ASSERT(_sd->m_tAnims.end() != item_it);

	if (fis_zero(item_it->second.velocity->velocity.linear)) return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////
// m_tAction processing
//////////////////////////////////////////////////////////////////////////

void CMotionManager::Update()
{
	if (seq_playing) return;
	
	// Установка Yaw
	if (pMonster->IsMovingOnPath()) pMonster->SetDirectionLook( ((spec_params & ASP_MOVE_BKWD) == ASP_MOVE_BKWD) );
	
	b_forced_velocity	= false;

	SelectAnimation		();
	SelectVelocities	();

	spec_params		= 0;
}


//////////////////////////////////////////////////////////////////////////
// SelectAnimation
// In:	path, target_yaw, m_tAction
// Out:	установить cur_anim
void CMotionManager::SelectAnimation()
{
	EAction							action = m_tAction;
	if (pMonster->IsMovingOnPath()) action = GetActionFromPath();

	cur_anim						= _sd->m_tMotions[action].anim;
	
	pMonster->CheckSpecParams		(spec_params);	
	if (Seq_Active()) return;

	if (prev_anim != cur_anim)		CheckTransition(prev_anim, cur_anim);
	if (Seq_Active()) return;

	CheckReplacedAnim				();
	
	pMonster->ProcessTurn			();
}


//////////////////////////////////////////////////////////////////////////
// SelectVelocities
// In:	path, target_yaw, анимация
// Out:	установить linear и angular velocities, 
//		по скорости движения выбрать финальную анимацию из Velocity_Chain
void CMotionManager::SelectVelocities()
{
	// получить скорости движения по пути
	bool		b_moving = pMonster->IsMovingOnPath();
	SMotionVel	path_vel;	path_vel.set(0.f,0.f);
	SMotionVel	anim_vel;	anim_vel.set(0.f,0.f);

	if (b_moving) {
		
		u32 cur_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index()].velocity;

		u32 next_point_velocity_index = u32(-1);
		if (pMonster->CDetailPathManager::path().size() > pMonster->curr_travel_point_index() + 1) 
			next_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index() + 1].velocity;

		// если сейчас стоит на месте и есть след точка (т.е. должен быть в движении),
		// то реализовать поворот на месте, а дальше форсировать скорость со следующей точки
		if ((cur_point_velocity_index == pMonster->eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
			if (angle_difference(pMonster->m_body.current.yaw, pMonster->m_body.target.yaw) < deg(1)) 
				cur_point_velocity_index = next_point_velocity_index;
		} 
		
		xr_map<u32,CDetailPathManager::STravelParams>::const_iterator it = pMonster->m_movement_params.find(cur_point_velocity_index);
		R_ASSERT(it != pMonster->m_movement_params.end());

		path_vel.set(_abs((*it).second.linear_velocity), (*it).second.angular_velocity);
	}

	ANIM_ITEM_MAP_IT	item_it = _sd->m_tAnims.find(cur_anim);
	R_ASSERT(_sd->m_tAnims.end() != item_it);

	// получить скорости движения по анимации
	anim_vel.set(item_it->second.velocity->velocity.linear, item_it->second.velocity->velocity.angular);

//	// проверить на совпадение
//	R_ASSERT(fsimilar(path_vel.linear,	anim_vel.linear));
//	R_ASSERT(fsimilar(path_vel.angular,	anim_vel.angular));


	// установка линейной скорости	
	// - проверить на возможность торможения
	if (!accel_check_braking(0.f)) {
		pMonster->m_velocity_linear.target	= _abs(anim_vel.linear);
		if (fis_zero(pMonster->m_velocity_linear.target)) pMonster->m_velocity_linear.current = 0.f;
	} else {
		pMonster->m_velocity_linear.target	= 0.1f;
	}

	// финальная корректировка анимации по физической скорости
	float  real_speed = pMonster->m_fCurSpeed;
	EMotionAnim new_anim;
	float		a_speed;

	if (accel_chain_get(real_speed, cur_anim,new_anim, a_speed)) {
		cur_anim = new_anim;
		pMonster->SetAnimSpeed(a_speed);
	} else pMonster->SetAnimSpeed(-1.f);

	// установка угловой скорости
	if (!b_forced_velocity) {
		item_it = _sd->m_tAnims.find(cur_anim);
		R_ASSERT(_sd->m_tAnims.end() != item_it);

		pMonster->m_velocity_angular.target	= pMonster->m_velocity_angular.current = item_it->second.velocity->velocity.angular;
	}
	
	// применить 
	// если установленная анимация отличается от предыдущей - установить новую анимацию
	if (cur_anim != prev_anim) ForceAnimSelect();		

	prev_anim	= cur_anim;
}

EAction CMotionManager::VelocityIndex2Action(u32 velocity_index)
{
	switch (velocity_index) {
		case pMonster->eVelocityParameterStand:			return ACT_STAND_IDLE;
		case pMonster->eVelocityParameterWalkNormal:	return ACT_WALK_FWD;
		case pMonster->eVelocityParameterRunNormal:		return ACT_RUN;
		case pMonster->eVelocityParameterWalkDamaged:	return ACT_WALK_FWD;
		case pMonster->eVelocityParameterRunDamaged:	return ACT_RUN;
		case pMonster->eVelocityParameterSteal:			return ACT_STEAL;
		case pMonster->eVelocityParameterDrag:			return ACT_DRAG;
		default:										NODEFAULT;
	}

	return ACT_STAND_IDLE;
}

EAction CMotionManager::GetActionFromPath()
{
	EAction action;
	
	u32 cur_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index()].velocity;
	action = VelocityIndex2Action(cur_point_velocity_index);

	u32 next_point_velocity_index = u32(-1);
	if (pMonster->CDetailPathManager::path().size() > pMonster->curr_travel_point_index() + 1) 
		next_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index() + 1].velocity;

	if ((cur_point_velocity_index == pMonster->eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
		if (angle_difference(pMonster->m_body.current.yaw, pMonster->m_body.target.yaw) < deg(1)) 
			action = VelocityIndex2Action(next_point_velocity_index);
	}

	return action;
}



//////////////////////////////////////////////////////////////////////////
// Debug

LPCSTR CMotionManager::GetAnimationName(EMotionAnim anim)
{
	ANIM_ITEM_MAP_IT	item_it = _sd->m_tAnims.find(anim);
	R_ASSERT(_sd->m_tAnims.end() != item_it);

	return *item_it->second.target_name;
}

LPCSTR CMotionManager::GetActionName(EAction action)
{
	return dbg_action_name_table[action];
}

// End Debug
//////////////////////////////////////////////////////////////////////////

