#include "stdafx.h"
#include "control_animation_base.h"
#include "ai_monster_jump.h"
#include "BaseMonster/base_monster.h"
#include "../../PHMovementControl.h"
#include "anim_triple.h"
#include "../../../skeletonanimated.h"
#include "../../detail_path_manager.h"
#include "monster_velocity_space.h"
#include "critical_action_info.h"
#include "monster_event_manager.h"
#include "control_jump.h"

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
		"ACT_JUMP"
};	

void CControlAnimationBase::reinit()
{
	inherited::reinit		();

	prev_action				= m_tAction	= ACT_STAND_IDLE;
	spec_params				= 0;

	fx_time_last_play		= 0;

	accel_init				();

	aa_time_last_attack		= 0;

	// обновить количество анимаций
	UpdateAnimCount			();

	// инициализация информации о текущей анимации
	m_cur_anim.motion			= eAnimStandIdle;
	m_cur_anim.index			= 0;
	m_cur_anim.time_started		= 0;
	m_cur_anim.speed.current	= -1.f;
	m_cur_anim.speed.target		= -1.f;
	m_cur_anim.blend			= 0;
	m_cur_anim.speed_change_vel	= 1.f;

	prev_motion					= cur_anim_info().motion; 

	m_prev_character_velocity	= 0.01f;

	m_anim_motion_map.clear		();

	spec_anim					= eAnimUndefined;

	// test
	m_man->capture				(this, ControlCom::eControlAnimation);

}

void CControlAnimationBase::on_start_control(ControlCom::EContolType type)
{
	switch (type) {
	case ControlCom::eControlAnimation: 
		m_man->subscribe	(this, ControlCom::eventAnimationEnd);	
		select_animation	();
		break;
	}
}

void CControlAnimationBase::on_stop_control	(ControlCom::EContolType type)
{
	switch (type) {
	case ControlCom::eControlAnimation: m_man->unsubscribe	(this, ControlCom::eventAnimationEnd);	break;
	}
}

void CControlAnimationBase::on_event(ControlCom::EEventType type, ControlCom::IEventData *data)
{
	switch (type) {
	case ControlCom::eventAnimationEnd: select_animation();	break;
	}
}

void CControlAnimationBase::select_animation()
{
	// start new animation
	SControlAnimationData		*ctrl_data = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	if (!ctrl_data) return;

	// перекрыть все определения и установть анимацию
	m_object->ForceFinalAnimation();

	// получить элемент SAnimItem, соответствующий текущей анимации
	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != anim_it);

	// определить необходимый индекс
	int index;
	if (-1 != anim_it->second.spec_id) index = anim_it->second.spec_id;
	else {
		VERIFY(anim_it->second.count != 0);
		index = ::Random.randI(anim_it->second.count);
	}

	// установить анимацию	
	string128	s1,s2;
	MotionID	cur_anim		= smart_cast<CSkeletonAnimated*>(m_object->Visual())->ID_Cycle_Safe(strconcat(s2,*anim_it->second.target_name,itoa(index,s1,10)));

	// Setup Com
	ctrl_data->global.motion	= cur_anim;
	ctrl_data->global.actual	= false;
	ctrl_data->speed			= m_cur_anim.speed.target;

	// Заполнить текущую анимацию
	string64	st,tmp;
	strconcat	(st,*anim_it->second.target_name,itoa(index,tmp,10));
	//	sprintf		(st, "%s%d", *anim_it->second.target_name, index);
	m_cur_anim.name				= st; 
	m_cur_anim.index			= u8(index);
	m_cur_anim.time_started		= Device.dwTimeGlobal;
	m_cur_anim.speed.current	= -1.f;
	m_cur_anim.speed.target		= -1.f;
}

// проверить существует ли переход из анимации from в to
void CControlAnimationBase::CheckTransition(EMotionAnim from, EMotionAnim to)
{
	if (!m_man->check_start_conditions(ControlCom::eControlSequencer)) return;

	// поиск соответствующего перехода
	bool		b_activated	= false;
	EMotionAnim cur_from = from; 
	EPState		state_from	= GetState(cur_from);
	EPState		state_to	= GetState(to);

	TRANSITION_ANIM_VECTOR_IT I = get_sd()->m_tTransitions.begin();
	bool bVectEmpty = get_sd()->m_tTransitions.empty();

	while (!bVectEmpty) {		// вход в цикл, если вектор переходов не пустой

		bool from_is_good	= ((I->from.state_used) ? (I->from.state == state_from) : (I->from.anim == cur_from));
		bool target_is_good = ((I->target.state_used) ? (I->target.state == state_to) : (I->target.anim == to));

		if (from_is_good && target_is_good) {

			//if (I->skip_if_aggressive && m_object->m_bAggressive) return;

			// переход годится
			if (!b_activated) {
				m_object->com_man().seq_init();
			}

			m_object->com_man().seq_add(get_motion_id(I->anim_transition));
			b_activated	= true;	


			if (I->chain) {
				cur_from	= I->anim_transition;
				state_from	= GetState(cur_from);
				I = get_sd()->m_tTransitions.begin();			// начать сначала
				continue;
			} else break;
		}
		if (get_sd()->m_tTransitions.end() == ++I) break;
	}

	if (b_activated) {
		m_object->com_man().seq_switch		();
	}
}

void CControlAnimationBase::CheckReplacedAnim()
{
	for (REPLACED_ANIM_IT it=m_tReplacedAnims.begin(); m_tReplacedAnims.end()!=it ;++it) 
		if ((cur_anim_info().motion == it->cur_anim) && (*(it->flag) == true)) { 
			cur_anim_info().motion = it->new_anim;
			return;
		}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attack Animation

#define TIME_OFFSET 10
#define TIME_DELTA	200

// Сравнивает тек. время с временами хитов в стеке
bool CControlAnimationBase::AA_TimeTest(SAAParam &params)
{

	// Частота хитов не может быть больше 'TIME_DELTA'
	TTime cur_time	= Device.dwTimeGlobal;

	if (aa_time_last_attack + TIME_DELTA > cur_time) return false;

	// искать текущую анимацию в AA_MAP

	AA_MAP_IT it = get_sd()->aa_map.find(m_cur_anim.name);
	if (it == get_sd()->aa_map.end()) return false;

	// вычислить смещённое время хита в соответствии с параметрами анимации атаки
	TTime offset_time = m_cur_anim.time_started + u32(1000 * GetCurAnimTime() * it->second.time);

	if ((offset_time >= (cur_time - TIME_OFFSET)) && (offset_time <= (cur_time + TIME_OFFSET)) ){
		params = it->second;
		return true;
	}

	return false;
}

void CControlAnimationBase::AA_GetParams(SAAParam &params, LPCSTR anim_name)
{
	shared_str st = anim_name;

	// искать текущую анимацию в AA_MAP
	AA_MAP_IT it = get_sd()->aa_map.find(st);
	if (it == get_sd()->aa_map.end()) return;

	params = it->second;
}
//////////////////////////////////////////////////////////////////////////


EPState	CControlAnimationBase::GetState (EMotionAnim a)
{
	// найти анимацию 
	ANIM_ITEM_MAP_IT  item_it = get_sd()->m_tAnims.find(a);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	return item_it->second.pos_state;
}

#define FX_CAN_PLAY_MIN_INTERVAL	50

void CControlAnimationBase::FX_Play(EHitSide side, float amount)
{
	if (fx_time_last_play + FX_CAN_PLAY_MIN_INTERVAL > m_object->m_dwCurrentTime) return;

	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != anim_it);

	clamp(amount,0.f,1.f);

	shared_str	*p_str = 0;
	switch (side) {
		case eSideFront:	p_str = &anim_it->second.fxs.front;	break;
		case eSideBack:		p_str = &anim_it->second.fxs.back;	break;
		case eSideLeft:		p_str = &anim_it->second.fxs.left;	break;
		case eSideRight:	p_str = &anim_it->second.fxs.right;	break;
	}

	if (p_str && p_str->size()) smart_cast<CSkeletonAnimated*>(m_object->Visual())->PlayFX(*(*p_str), amount);

	fx_time_last_play = m_object->m_dwCurrentTime;
}


float CControlAnimationBase::GetCurAnimTime()
{
	return 1.f;

	//VERIFY(m_cur_anim.blend);

	//return (m_cur_anim.blend->timeTotal / m_cur_anim.blend->speed);
}

float CControlAnimationBase::GetAnimSpeed(EMotionAnim anim)
{
	ANIM_ITEM_MAP_IT	anim_it = get_sd()->m_tAnims.find(anim);
	VERIFY				(get_sd()->m_tAnims.end() != anim_it);

	CMotionDef			*def = get_motion_def(anim_it, 0);

	return				(def->Dequantize(def->speed));
}


bool CControlAnimationBase::IsStandCurAnim()
{
	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	if (fis_zero(item_it->second.velocity.velocity.linear)) return true;
	return false;
}


EAction CControlAnimationBase::VelocityIndex2Action(u32 velocity_index)
{
	switch (velocity_index) {
		case MonsterMovement::eVelocityParameterStand:			return ACT_STAND_IDLE;
		case MonsterMovement::eVelocityParameterWalkNormal:		return ACT_WALK_FWD;
		case MonsterMovement::eVelocityParameterRunNormal:		return ACT_RUN;
		case MonsterMovement::eVelocityParameterWalkDamaged:	return ACT_WALK_FWD;
		case MonsterMovement::eVelocityParameterRunDamaged:		return ACT_RUN;
		case MonsterMovement::eVelocityParameterSteal:			return ACT_STEAL;
		case MonsterMovement::eVelocityParameterDrag:			return ACT_DRAG;
		case MonsterMovement::eVelocityParameterInvisible:		return ACT_RUN;
	}

	return m_object->CustomVelocityIndex2Action(velocity_index);
}

EAction CControlAnimationBase::GetActionFromPath()
{
	EAction action;

	u32 cur_point_velocity_index = m_object->movement().detail().path()[m_object->movement().detail().curr_travel_point_index()].velocity;
	action = VelocityIndex2Action(cur_point_velocity_index);

	u32 next_point_velocity_index = u32(-1);
	if (m_object->movement().detail().path().size() > m_object->movement().detail().curr_travel_point_index() + 1) 
		next_point_velocity_index = m_object->movement().detail().path()[m_object->movement().detail().curr_travel_point_index() + 1].velocity;

	if ((cur_point_velocity_index == MonsterMovement::eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
		if (!m_object->control().direction().is_turning()) 
			action = VelocityIndex2Action(next_point_velocity_index);
	}

	return action;
}



//////////////////////////////////////////////////////////////////////////
// Debug

LPCSTR CControlAnimationBase::GetAnimationName(EMotionAnim anim)
{
	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(anim);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	return *item_it->second.target_name;
}

LPCSTR CControlAnimationBase::GetActionName(EAction action)
{
	return dbg_action_name_table[action];
}

///////////////////////////////////////////////////////////////////////////////////////

void CControlAnimationBase::ValidateAnimation()
{

	ANIM_ITEM_MAP_IT item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	bool is_moving_anim		= !fis_zero(item_it->second.velocity.velocity.linear);
	bool is_moving_on_path	= m_object->control().path_builder().is_moving_on_path();

	if (is_moving_on_path && is_moving_anim) {
		m_object->dir().use_path_direction(item_it->first == eAnimDragCorpse);
		return;
	}

	if (!is_moving_on_path && is_moving_anim) {
		cur_anim_info().motion				= eAnimStandIdle;
		m_object->move().stop				();
		return;
	}

	if (is_moving_on_path && !is_moving_anim) {
		m_object->move().stop				();
		return;
	}

	if (!m_object->control().direction().is_turning() && ((item_it->first == eAnimStandTurnLeft) || (item_it->first == eAnimStandTurnRight))) {
		cur_anim_info().motion		= eAnimStandIdle;
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void CControlAnimationBase::UpdateAnimCount()
{
	for (ANIM_ITEM_MAP_IT it = get_sd()->m_tAnims.begin(); it != get_sd()->m_tAnims.end(); it++)	{

		// проверить, были ли уже загружены данные
		if (it->second.count != 0) return;

		string128	s, s_temp; 
		u8 count = 0;

		for (int i=0; ; ++i) {
			if (smart_cast<CSkeletonAnimated*>(m_object->Visual())->ID_Cycle_Safe(strconcat(s_temp, *it->second.target_name,itoa(i,s,10))))  count++;
			else break;
		}

		if (count != 0) it->second.count = count;
		else {
			sprintf(s, "Error! No animation: %s for monster %s", *it->second.target_name, *m_object->cName());
			R_ASSERT2(count != 0, s);
		} 
	}
}

CMotionDef *CControlAnimationBase::get_motion_def(ANIM_ITEM_MAP_IT &it, u32 index)
{
	string128			s1,s2;
	CSkeletonAnimated	*skeleton_animated = smart_cast<CSkeletonAnimated*>(m_object->Visual());
	const MotionID		&motion_id = skeleton_animated->ID_Cycle_Safe(strconcat(s2,*it->second.target_name,itoa(index,s1,10)));
	return				(skeleton_animated->LL_GetMotionDef(motion_id));
}

void CControlAnimationBase::AddAnimTranslation(const MotionID &motion, LPCSTR str)
{
	m_anim_motion_map.insert(mk_pair(motion, str));	
}
shared_str CControlAnimationBase::GetAnimTranslation(const MotionID &motion)
{
	shared_str				ret_value;

	ANIM_TO_MOTION_MAP_IT	anim_it = m_anim_motion_map.find(motion);
	if (anim_it != m_anim_motion_map.end()) ret_value = anim_it->second;

	return ret_value;
}

MotionID CControlAnimationBase::get_motion_id(EMotionAnim a, u32 index)
{
	// получить элемент SAnimItem, соответствующий текущей анимации
	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(a);
	VERIFY(get_sd()->m_tAnims.end() != anim_it);

	// определить необходимый индекс
	if (index == u32(-1)) {
		if (-1 != anim_it->second.spec_id) index = anim_it->second.spec_id;
		else {
			VERIFY(anim_it->second.count != 0);
			index = ::Random.randI(anim_it->second.count);
		}
	}

	string128			s1,s2;
	return				(smart_cast<CSkeletonAnimated*>(m_object->Visual())->ID_Cycle_Safe(strconcat(s2,*anim_it->second.target_name,itoa(index,s1,10))));
}

void CControlAnimationBase::stop_now()
{
	m_object->move().stop			();
//	m_object->path().disable_path	();
}

void CControlAnimationBase::set_animation_speed()
{
	// Setup Com
	SControlAnimationData		*ctrl_data = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	if (!ctrl_data) return;
	ctrl_data->speed			= m_cur_anim.speed.target;
}
