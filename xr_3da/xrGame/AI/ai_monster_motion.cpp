#include "stdafx.h"
#include "ai_monster_motion.h"
#include "ai_monster_jump.h"
#include "biting/ai_biting.h"

#define CHECK_SHARED_LOADED() {if (CSharedClass<_motion_shared>::IsLoaded()) return; }

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
}

// Загрузка параметров анимации. Вызывать необходимо на Monster::Load
void CMotionManager::AddAnim(EMotionAnim ma, LPCTSTR tn, int s_id, float speed, float r_speed, EPState p_s)
{
	CHECK_SHARED_LOADED();
	
	SAnimItem new_item;

	new_item.target_name	= tn;
	new_item.spec_id		= s_id;
	new_item.speed.linear	= speed;
	new_item.speed.angular	= r_speed;
	new_item.pos_state		= p_s;

	_sd->m_tAnims.insert			(mk_pair(ma, new_item));
}

// Загрузка анимаций. Необходимо вызывать на Monster::NetSpawn 
void CMotionManager::LoadVisualData()
{
	for (ANIM_ITEM_MAP_IT item_it = _sd->m_tAnims.begin(); _sd->m_tAnims.end() != item_it; ++item_it) {
		// Очистка старых анимаций
		if (!item_it->second.pMotionVect.empty()) item_it->second.pMotionVect.clear();
		// Загрузка новых
		Load(*item_it->second.target_name, &item_it->second.pMotionVect);
	}

	// Преобразование fx's на поздней стадии (когда известен визуал)
	FX_ConvertMap();

}

void CMotionManager::AddTransition(EMotionAnim from, EMotionAnim to, EMotionAnim trans, bool chain)
{
	STransition new_item;

	new_item.from.state_used	= false;
	new_item.from.anim			= from;

	new_item.target.state_used	= false;
	new_item.target.anim		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	_sd->m_tTransitions.push_back(new_item);
}


void CMotionManager::AddTransition(EMotionAnim from, EPState to, EMotionAnim trans, bool chain)
{
	CHECK_SHARED_LOADED();

	STransition new_item;

	new_item.from.state_used	= false;
	new_item.from.anim			= from;

	new_item.target.state_used	= true;
	new_item.target.state		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	_sd->m_tTransitions.push_back(new_item);
}

void CMotionManager::AddTransition(EPState from, EMotionAnim to, EMotionAnim trans, bool chain)
{
	CHECK_SHARED_LOADED();

	STransition new_item;

	new_item.from.state_used	= true;
	new_item.from.state			= from;

	new_item.target.state_used	= false;
	new_item.target.anim		= to;


	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	_sd->m_tTransitions.push_back(new_item);
}

void CMotionManager::AddTransition(EPState from, EPState to, EMotionAnim trans, bool chain)
{
	CHECK_SHARED_LOADED();
	
	STransition new_item;

	new_item.from.state_used	= true;
	new_item.from.state			= from;

	new_item.target.state_used	= true;
	new_item.target.state		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	_sd->m_tTransitions.push_back(new_item);
}

void CMotionManager::LinkAction(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle)
{
	CHECK_SHARED_LOADED();
	
	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= true;
	new_item.turn.anim_left		= pmt_left;
	new_item.turn.anim_right	= pmt_right;
	new_item.turn.min_angle		= pmt_angle;

	_sd->m_tMotions.insert	(mk_pair(act, new_item));
}

void CMotionManager::LinkAction(EAction act, EMotionAnim pmt_motion)
{
	CHECK_SHARED_LOADED();

	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= false;

	_sd->m_tMotions.insert	(mk_pair(act, new_item));
}

void CMotionManager::AddReplacedAnim(bool *b_flag, EMotionAnim pmt_cur_anim, EMotionAnim pmt_new_anim)
{
	SReplacedAnim ra;

	ra.flag		= b_flag;
	ra.cur_anim = pmt_cur_anim;
	ra.new_anim = pmt_new_anim;

	m_tReplacedAnims.push_back(ra);
}


// загрузка анимаций из модели начинающиеся с pmt_name в вектор pMotionVect
void CMotionManager::Load(LPCTSTR pmt_name, ANIM_VECTOR	*pMotionVect)
{
	
	string256	S1, S2; 
	CMotionDef	*tpMotionDef;
	for (int i=0; ; ++i) {
		if (0 != (tpMotionDef = PSkeletonAnimated(pVisual)->ID_Cycle_Safe(strconcat(S1,pmt_name,itoa(i,S2,10)))))  pMotionVect->push_back(tpMotionDef);
		else if (0 != (tpMotionDef = PSkeletonAnimated(pVisual)->ID_FX_Safe(strconcat(S1,pmt_name,itoa(i,S2,10))))) pMotionVect->push_back(tpMotionDef);
		else break;
	}
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
	
	LOG_EX2("ANIM: Name = [%s], Time =[%u]", *"*/ *anim_it->second.target_name, pMonster->m_dwCurrentTime /*"*);

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

// В соответствии текущему действию m_tAction, назначить соответсвующию анимацию и параметры движения
// выполняется на каждом шед.апдейте, после выполнения состояния
void CMotionManager::ProcessAction()
{
	if (pJumping && pJumping->IsActive()) return;

	if (!seq_playing) {

		// преобразовать Action в Motion и получить новую анимацию
		SMotionItem MI = _sd->m_tMotions[m_tAction];
		cur_anim = MI.anim;

		// установить target.yaw
		if (pMonster->valid()) pMonster->SetDirectionLook( ((spec_params & ASP_MOVE_BKWD) == ASP_MOVE_BKWD) );

		// если новая анимация не совпадает с предыдущей, проверить переход
		if (prev_anim != cur_anim) CheckTransition(prev_anim, cur_anim);
		
		if (!seq_playing) {
			// проверить необходимость установки анимации поворота
			float &cur_yaw		= pMonster->m_body.current.yaw;
			float &target_yaw	= pMonster->m_body.target.yaw;
			if (MI.is_turn_params) {
				if (angle_difference(cur_yaw, target_yaw) > MI.turn.min_angle) {
					// необходим поворот влево или вправо
					if (angle_normalize_signed(target_yaw - cur_yaw) > 0) 	cur_anim = MI.turn.anim_right;	// вправо
					else													cur_anim = MI.turn.anim_left; 	// влево

					// Проверить, является ли это хорошим методом
					Seq_Add(cur_anim);
					Seq_Switch();
				}
			}	

			pMonster->ProcessTurn();
		
			// Проверить ASP и доп. параметры анимации (kinda damaged)
			pMonster->CheckSpecParams(spec_params); 

			if (!seq_playing) {
				// проверить подмену анимаций
				CheckReplacedAnim();

				// если монстр стоит на месте и играет анимацию движения, принять stand_idle
				FixBadState();
			}
		}
	} else if (seq_playing) {
		cur_anim = *seq_it;
		pMonster->enable_movement(false);
	}

	ApplyParams();

	// если установленная анимация отличается от предыдущей - установить новую анимацию
	if (cur_anim != prev_anim) ForceAnimSelect();		

	prev_anim	= cur_anim;
	spec_params = 0;
}


// Установка линейной и угловой скоростей для cur_anim
void CMotionManager::ApplyParams()
{
	ANIM_ITEM_MAP_IT	item_it = _sd->m_tAnims.find(cur_anim);
	R_ASSERT(_sd->m_tAnims.end() != item_it);

	pMonster->m_fCurSpeed		= item_it->second.speed.linear;
	pMonster->m_body.speed		= item_it->second.speed.angular;
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
//	bool is_moving_action = IsMoving();
//	bool is_action_changed = prev_action != m_tAction;
//	TTime cur_time = Level().timeServer();
//
//	// если конец пути и монстр идёт - исправить
//	if (!pMonster->valid() && is_moving_action) {
//		cur_anim = eAnimStandIdle;
//		return;
//	}
//
//	// исправления сосояния 'бега на месте'
//	if (is_action_changed) {
//		time_start_stand = 0;
//	}
//
//	// если только начинаем стоять, проигрывая анимацию движения
//	if (is_moving_action && (0 == time_start_stand)) {
//		time_start_stand	= cur_time;
//	}
//
//	if (is_moving_action && !is_action_changed && (time_start_stand + CRITICAL_STAND_TIME < cur_time) && pMonster->IsStanding(CRITICAL_STAND_TIME)) {
//		cur_anim = eAnimStandIdle;	
//
//		if (time_start_stand + CRITICAL_STAND_TIME + TIME_STAND_RECHECK < cur_time) time_start_stand = 0;
//	}
//
//	prev_action = m_tAction;
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

void CMotionManager::AA_PushAttackAnim(SAttackAnimation AttackAnim)
{	
	_sd->aa_all.push_back(AttackAnim);
}

void CMotionManager::AA_PushAttackAnim(EMotionAnim a, u32 i3, TTime from, TTime to, Fvector &ray, float dist, float damage, float yaw, float pitch, u32 flags)
{
	CHECK_SHARED_LOADED();	

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



/////////////////////////////////////////////////////////////////////////////////////////////
// Загружает из LTX временную карту соответствий
// название боны -> индекс1, индекс2
void CMotionManager::FX_LoadMap(LPCSTR section)
{
	CHECK_SHARED_LOADED();

	string32 sect;
	strconcat(sect,section,"_fx");

	if (!pSettings->section_exist(sect)) return;

	_sd->default_fx_indexes.front	= -1;
	_sd->default_fx_indexes.back	= -1;

	LPCSTR		bone_name,val;
	string16	first,second; 
	t_fx_index	index;

	for (u32 i=0; pSettings->r_line(sect,i,&bone_name,&val); ++i) {
		
		_GetItem(val,0,first);
		_GetItem(val,1,second);

		if (0 == strcmp(bone_name, "default")) {
			_sd->default_fx_indexes.front	= s8(atoi(first));
			_sd->default_fx_indexes.back	= s8(atoi(second));
		} else {
			index.front	=	s8(atoi(first));
			index.back	=	s8(atoi(second));

			_sd->fx_map_string.insert(mk_pair(bone_name, index));
		}
	}

	_sd->map_converted = false;
}

// Call on NetSpawn
void CMotionManager::FX_ConvertMap()
{
	if (_sd->map_converted) return;
	_sd->map_converted = true;

	// Преобразовать названия бон в их u16-индексы
	for (FX_MAP_STRING_IT item_it = _sd->fx_map_string.begin(); item_it != _sd->fx_map_string.end(); ++item_it) {
		u16 bone_id = PKinematics(pVisual)->LL_BoneID(*item_it->first);
		_sd->fx_map_u16.insert(mk_pair(bone_id,item_it->second)); 
	}
}

#define FX_CAN_PLAY_MIN_INTERVAL	500

void CMotionManager::FX_Play(u16 bone, bool is_front, float amount) 
{
	if (_sd->fx_map_u16.empty()) return;
	if (fx_time_last_play + FX_CAN_PLAY_MIN_INTERVAL > pMonster->m_dwCurrentTime) return;

	FX_MAP_U16_IT	fx_it = _sd->fx_map_u16.find(bone);
	
	t_fx_index cur_fx = cur_fx = fx_it->second;
	if (_sd->fx_map_u16.end() == fx_it) {
		
		// Найти минимальное расстояние до боны на которой есть fx
		CBoneInstance *target_bone = &PKinematics(pVisual)->LL_GetBoneInstance(bone);
		
		float	best_dist = flt_max;

		for (FX_MAP_U16_IT it = _sd->fx_map_u16.begin(); it != _sd->fx_map_u16.end(); ++it) {
			CBoneInstance *cur_bone = &PKinematics(pVisual)->LL_GetBoneInstance(it->first);
			
			float cur_dist = target_bone->mTransform.c.distance_to(cur_bone->mTransform.c);
			if (best_dist > cur_dist) {
				best_dist = cur_dist;
				cur_fx = it->second;
			}
		}
	}

	s8 fx_index;
	fx_index = ((is_front)? cur_fx.front : cur_fx.back);
	
	// Если нет соответстующего fx'a - загрузить default
	if (fx_index < 0) {
		fx_index = ((is_front)? _sd->default_fx_indexes.front :  _sd->default_fx_indexes.back);

		if (fx_index < 0) return;
	}

	string16 temp;
	itoa(fx_index,temp,10);

	string32 fx_name;
	strconcat(fx_name,"fx_damage_",temp);
	
	clamp(amount,0.f,1.f);
	PSkeletonAnimated(pVisual)->PlayFX(fx_name, amount);

	fx_time_last_play = pMonster->m_dwCurrentTime;
}






