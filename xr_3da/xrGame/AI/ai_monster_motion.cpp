#include "stdafx.h"
#include "ai_monster_motion.h"
#include "ai_monster_jump.h"
#include "biting/ai_biting.h"
#include "ai_monster_utils.h"
#include "../PHMovementControl.h"
#include "anim_triple.h"
#include "ai_monster_debug.h"

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
}

void CMotionManager::reinit()
{
	prev_anim				= cur_anim	= eAnimStandIdle; 
	prev_action				= m_tAction	= ACT_STAND_IDLE;
	m_tpCurAnim				= 0;
	spec_params				= 0;

	should_play_die_anim	= true;			//���� ���� �� NetSpawn ������ ��������������� � true
	
	Seq_Init				();
	
	b_end_transition		= false;
	saved_anim				= cur_anim;

	fx_time_last_play		= 0;
	b_forced_velocity		= false;
	
	accel_init				();
	
	aa_time_last_attack		= 0;

	pCurAnimTriple			= 0;

	// �������� ���������� ��������
	UpdateAnimCount			();

	pJumping				= 0;

	anim_speed				= -1.f;
	cur_blend				= 0;
}


// ������������� ������� ��������, ������� ���������� ���������.
// ���������� false, ���� � ����� �������� ��� �������������
bool CMotionManager::PrepareAnimation()
{
	// �������� �� ����������� �������� ������
	if (!pMonster->g_Alive()) 
		if (should_play_die_anim) {
			should_play_die_anim = false;  // �������� �������� ������ ������ ���
			if (get_sd()->m_tAnims.find(eAnimDie) != get_sd()->m_tAnims.end()) cur_anim = eAnimDie;
			else return false;
		} else return false;

	if (pJumping && pJumping->IsActive())  return pJumping->PrepareAnimation(&m_tpCurAnim);
	
	//CheckAnimWithPath();

	if (0 != m_tpCurAnim) return false;
	
	if (pCurAnimTriple && pCurAnimTriple->is_active()) {
		if (pCurAnimTriple->prepare_animation(&m_tpCurAnim)) return true;
	}

	// �������� ������� SAnimItem ��������������� cur_anim
	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(cur_anim);
	VERIFY(get_sd()->m_tAnims.end() != anim_it);

	// ���������� ����������� ������
	int index;
	if (-1 != anim_it->second.spec_id) index = anim_it->second.spec_id;
	else {
		VERIFY(anim_it->second.count != 0);
		index = ::Random.randI(anim_it->second.count);
	}

	
	// ���������� ��������	
	m_tpCurAnim = get_motion_def(anim_it,index);

	// ��������� ������� ��������
	string64 st;
	sprintf(st, "%s%d", *anim_it->second.target_name, index);
	pMonster->cur_anim.name		= st; 
	pMonster->cur_anim.anim		= cur_anim;
	pMonster->cur_anim.index	= u8(index);
	pMonster->cur_anim.started	= Level().timeServer();

	// ���������������� ���������� � ������� �������� �������
	STEPS_Initialize();

	return true;
}

// ��������� ���������� �� ������� �� �������� from � to
void CMotionManager::CheckTransition(EMotionAnim from, EMotionAnim to)
{
	// ����� ���������������� ��������
	bool		bActivated	= false;
	EMotionAnim cur_from = from; 
	EPState		state_from	= GetState(cur_from);
	EPState		state_to	= GetState(to);

	TRANSITION_ANIM_VECTOR_IT I = get_sd()->m_tTransitions.begin();
	bool bVectEmpty = get_sd()->m_tTransitions.empty();

	while (!bVectEmpty) {		// ���� � ����, ���� ������ ��������� �� ������

		bool from_is_good	= ((I->from.state_used) ? (I->from.state == state_from) : (I->from.anim == cur_from));
		bool target_is_good = ((I->target.state_used) ? (I->target.state == state_to) : (I->target.anim == to));

		if (from_is_good && target_is_good) {

			// ������� �������
			Seq_Add(I->anim_transition);
			bActivated	= true;	

			if (I->chain) {
				cur_from	= I->anim_transition;
				state_from	= GetState(cur_from);
				I = get_sd()->m_tTransitions.begin();			// ������ �������
				continue;
			} else break;
		}
		if (get_sd()->m_tTransitions.end() == ++I) break;
	}

	if (bActivated) {
		b_end_transition = true;		
		saved_anim = to;
		Seq_Switch();
	}
}


// ��������� �������� � ������� ��������� ��� cur_anim
void CMotionManager::ApplyParams()
{
	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(cur_anim);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	//pMonster->m_fCurSpeed		= item_it->second.speed.linear;
	pMonster->m_velocity_linear.target	= item_it->second.velocity->velocity.linear;
	if (!b_forced_velocity) pMonster->m_velocity_angular.target = item_it->second.velocity->velocity.angular;

}

// Callback �� ���������� ��������
void CMotionManager::OnAnimationEnd() 
{ 
	m_tpCurAnim = 0;
	if (seq_playing) Seq_Switch();

	if (pJumping && pJumping->IsActive()) pJumping->OnAnimationEnd();
}

// ���� ������ ����� �� ����� � ������ �������� �������� - force stand idle
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
// ������ � ��������������������
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
		// ��������� �������������������
		seq_it = seq_states.begin();
	} else {
		++seq_it; 
		if (seq_states.end() == seq_it) {
			Seq_Finish();
			return;
		}
	}

	seq_playing = true;

	// ���������� ���������
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
		prev_anim = cur_anim = get_sd()->m_tMotions[m_tAction].anim;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attack Animation

#define TIME_OFFSET 10
#define TIME_DELTA	1000

// ���������� ���. ����� � ��������� ����� � �����
bool CMotionManager::AA_TimeTest(SAAParam &params)
{
	// ������� ����� �� ����� ���� ������ 'TIME_DELTA'
	TTime cur_time	= Level().timeServer();

	if (aa_time_last_attack + TIME_DELTA > cur_time) return false;

	// ������ ������� �������� � AA_MAP
	AA_MAP_IT it = get_sd()->aa_map.find(pMonster->cur_anim.name);
	if (it == get_sd()->aa_map.end()) return false;
	
	// ��������� ��������� ����� ���� � ������������ � ����������� �������� �����
	TTime offset_time = pMonster->cur_anim.started + u32(1000 * GetCurAnimTime() * it->second.time);

	if ((offset_time >= (cur_time - TIME_OFFSET)) && (offset_time <= (cur_time + TIME_OFFSET)) ){
		params = it->second;
		return true;
	}

	return false;
}

void CMotionManager::AA_GetParams(SAAParam &params, LPCSTR anim_name)
{
	ref_str st = anim_name;

	// ������ ������� �������� � AA_MAP
	AA_MAP_IT it = get_sd()->aa_map.find(st);
	if (it == get_sd()->aa_map.end()) return;
	
	params = it->second;
}
//////////////////////////////////////////////////////////////////////////


EPState	CMotionManager::GetState (EMotionAnim a)
{
	// ����� �������� 
	ANIM_ITEM_MAP_IT  item_it = get_sd()->m_tAnims.find(a);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	return item_it->second.pos_state;
}

void CMotionManager::ForceAnimSelect() 
{
	m_tpCurAnim = 0; 
	pMonster->SelectAnimation(pMonster->Direction(),pMonster->Direction(),0);
}

#define FX_CAN_PLAY_MIN_INTERVAL	50

void CMotionManager::FX_Play(EHitSide side, float amount)
{
	if (fx_time_last_play + FX_CAN_PLAY_MIN_INTERVAL > pMonster->m_dwCurrentTime) return;

	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(cur_anim);
	VERIFY(get_sd()->m_tAnims.end() != anim_it);
	
	clamp(amount,0.f,1.f);

	ref_str	*p_str = 0;
	switch (side) {
		case eSideFront:	p_str = &anim_it->second.fxs.front;	break;
		case eSideBack:		p_str = &anim_it->second.fxs.back;	break;
		case eSideLeft:		p_str = &anim_it->second.fxs.left;	break;
		case eSideRight:	p_str = &anim_it->second.fxs.right;	break;
	}
	
	if (p_str && p_str->size()) PSkeletonAnimated(pMonster->Visual())->PlayFX(*(*p_str), amount);

	fx_time_last_play = pMonster->m_dwCurrentTime;
}


float CMotionManager::GetCurAnimTime()
{
	//if (!cur_blend) return 9999.0f;
	VERIFY(cur_blend);

	return cur_blend->timeTotal;
}

float CMotionManager::GetAnimSpeed(EMotionAnim anim)
{
	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(anim);
	VERIFY(get_sd()->m_tAnims.end() != anim_it);

	CMotionDef *def = get_motion_def(anim_it, 0);

	return def->Dequantize(def->speed);
}


void CMotionManager::ForceAngularSpeed(float vel)
{
	pMonster->m_velocity_angular.current = pMonster->m_velocity_angular.target = vel;
	b_forced_velocity = true;
}

bool CMotionManager::IsStandCurAnim()
{
	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(cur_anim);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	if (fis_zero(item_it->second.velocity->velocity.linear)) return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////
// m_tAction processing
//////////////////////////////////////////////////////////////////////////

void CMotionManager::Update()
{
	if (seq_playing) return;
	
	// ��������� Yaw
	if (pMonster->IsMovingOnPath()) pMonster->SetDirectionLook( ((spec_params & ASP_MOVE_BKWD) == ASP_MOVE_BKWD) );
	
	b_forced_velocity	= false;

	SelectAnimation		();
	SelectVelocities	();

	spec_params		= 0;
}


//////////////////////////////////////////////////////////////////////////
// SelectAnimation
// In:	path, target_yaw, m_tAction
// Out:	���������� cur_anim
void CMotionManager::SelectAnimation()
{
	EAction							action = m_tAction;
	if (pMonster->IsMovingOnPath()) action = GetActionFromPath();

	cur_anim						= get_sd()->m_tMotions[action].anim;
	
	pMonster->CheckSpecParams		(spec_params);	
	if (Seq_Active()) return;

	if (prev_anim != cur_anim)		CheckTransition(prev_anim, cur_anim);
	if (Seq_Active()) return;

	CheckReplacedAnim				();
	
	pMonster->ProcessTurn			();
}


//////////////////////////////////////////////////////////////////////////
// SelectVelocities
// In:	path, target_yaw, ��������
// Out:	���������� linear � angular velocities, 
//		�� �������� �������� ������� ��������� �������� �� Velocity_Chain
void CMotionManager::SelectVelocities()
{
	// �������� �������� �������� �� ����
	bool		b_moving = pMonster->IsMovingOnPath();
	SMotionVel	path_vel;	path_vel.set(0.f,0.f);
	SMotionVel	anim_vel;	anim_vel.set(0.f,0.f);

	if (b_moving) {
		
		u32 cur_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index()].velocity;

		u32 next_point_velocity_index = u32(-1);
		if (pMonster->CDetailPathManager::path().size() > pMonster->curr_travel_point_index() + 1) 
			next_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index() + 1].velocity;

		// ���� ������ ����� �� ����� � ���� ���� ����� (�.�. ������ ���� � ��������),
		// �� ����������� ������� �� �����, � ������ ����������� �������� �� ��������� �����
		if ((cur_point_velocity_index == pMonster->eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
			if (angle_difference(pMonster->m_body.current.yaw, pMonster->m_body.target.yaw) < deg(1)) 
				cur_point_velocity_index = next_point_velocity_index;
		} 
		
		xr_map<u32,CDetailPathManager::STravelParams>::const_iterator it = pMonster->m_movement_params.find(cur_point_velocity_index);
		VERIFY(it != pMonster->m_movement_params.end());

		path_vel.set(_abs((*it).second.linear_velocity), (*it).second.angular_velocity);
	}

	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(cur_anim);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	// �������� �������� �������� �� ��������
	anim_vel.set(item_it->second.velocity->velocity.linear, item_it->second.velocity->velocity.angular);

//	// ��������� �� ����������
//	R_ASSERT(fsimilar(path_vel.linear,	anim_vel.linear));
//	R_ASSERT(fsimilar(path_vel.angular,	anim_vel.angular));

	// ��������� �������� ��������	
	// - ��������� �� ����������� ����������
	if (!accel_check_braking(0.f)) {
		pMonster->m_velocity_linear.target	= _abs(anim_vel.linear);
		if (fis_zero(pMonster->m_velocity_linear.target)) pMonster->m_velocity_linear.current = 0.f;
	} else {
		pMonster->m_velocity_linear.target	= 0.f;
	}

	// ���� ���������, �� ���������� �������� �� ����
	if (pMonster->state_invisible) pMonster->m_velocity_linear.target	= _abs(path_vel.linear);

	// ��������� ������������� �������� �� ���������� ��������
	float  real_speed = pMonster->m_PhysicMovementControl->GetVelocityMagnitude();
	EMotionAnim new_anim;
	float		a_speed;

	if (accel_chain_get(real_speed, cur_anim,new_anim, a_speed)) {
		cur_anim = new_anim;
		SetAnimSpeed(a_speed);
	} else SetAnimSpeed(-1.f);

	// ��������� ������� ��������
	if (!b_forced_velocity) {
		item_it = get_sd()->m_tAnims.find(cur_anim);
		VERIFY(get_sd()->m_tAnims.end() != item_it);

		pMonster->m_velocity_angular.target	= pMonster->m_velocity_angular.current = item_it->second.velocity->velocity.angular;
	}
	
	// ��������� 
	// ���� ������������� �������� ���������� �� ���������� - ���������� ����� ��������
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
		case pMonster->eVelocityParameterInvisible:		return ACT_RUN;
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
	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(anim);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	return *item_it->second.target_name;
}

LPCSTR CMotionManager::GetActionName(EAction action)
{
	return dbg_action_name_table[action];
}

// End Debug
//////////////////////////////////////////////////////////////////////////


void CMotionManager::STEPS_Update(u8 legs_num)
{
	if (step_info.disable) return;
	
	SGameMtlPair* mtl_pair		= pMonster->CMaterialManager::get_current_pair();
	if (!mtl_pair) return;

	// �������� ��������� ����
	SStepParam &step		= step_info.params;
	TTime		cur_time	= Level().timeServer();

	// ����� ������ ����� ��������
	float cycle_anim_time	= GetCurAnimTime() / step.cycles;

	for (u32 i=0; i<legs_num; i++) {
		
		// ���� ������� ��� ���������� ��� ���� ����, �� skip
		if (step_info.activity[i].handled && (step_info.activity[i].cycle == step_info.cur_cycle)) continue;
		
		// ��������� ��������� ����� ���� � ������������ � ����������� �������� ������
		TTime offset_time = pMonster->cur_anim.started + u32(1000 * (cycle_anim_time * (step_info.cur_cycle-1) + cycle_anim_time * step.step[i].time));

		if ((offset_time >= (cur_time - TIME_OFFSET)) && (offset_time <= (cur_time + TIME_OFFSET)) ){
			
			// ������ ����
			if (!mtl_pair->StepSounds.empty()) {
				
				Fvector sound_pos = pMonster->Position();
				sound_pos.y += 0.5;

				SELECT_RANDOM(step_info.activity[i].sound, mtl_pair, StepSounds);
				step_info.activity[i].sound.play_at_pos	(pMonster,sound_pos);

			}
			
			// ������ ��������
			if (!mtl_pair->CollideParticles.empty()) {
				LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];
				
				//�������� �������� ������������ ����������
				CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);

				// ��������� ������� � �������������� ��������
				Fmatrix pos; 

				// ���������� �����������
				pos.k.set(Fvector().set(0.0f,1.0f,0.0f));
				Fvector::generate_orthonormal_basis(pos.k, pos.i, pos.j);
				
				// ���������� �������
				pos.c.set(pMonster->get_foot_position(ELegType(i)));
				
				ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
				Level().ps_needtoplay.push_back(ps);
			}

			// Play Camera FXs
			pMonster->event_on_step();

			// �������� ���� handle
			step_info.activity[i].handled	= true;
			step_info.activity[i].cycle		= step_info.cur_cycle;
		}
	}

	// ���������� ������� ����
	if (step_info.cur_cycle < step.cycles) step_info.cur_cycle = 1 + u8(float(cur_time - pMonster->cur_anim.started) / (1000.f*cycle_anim_time));

	// ��������������� �������� �����
	for (i=0; i<legs_num; i++) {
		if (step_info.activity[i].handled && step_info.activity[i].sound.feedback) {
			Fvector sound_pos = pMonster->Position();
			sound_pos.y += 0.5;
			step_info.activity[i].sound.set_position	(sound_pos);
			//step_info.activity[i].sound.set_volume	(step_info.params.step[i].power);
		}
	}
}

void CMotionManager::STEPS_Initialize() 
{
	// ������ ������� �������� � STEPS_MAP
	STEPS_MAP_IT it = get_sd()->steps_map.find(pMonster->cur_anim.name);
	if (it == get_sd()->steps_map.end()) {
		step_info.disable = true;
		return;
	}

	step_info.disable	= false;
	step_info.params	= it->second;
	step_info.cur_cycle = 1;					// all cycles are 1-based

	for (u32 i=0; i<4; i++) {
		step_info.activity[i].handled	= false;
		step_info.activity[i].cycle		= step_info.cur_cycle;	
	}

}

/////////////////////////////////////////////////////////////////////////////////////////

void CMotionManager::TA_Activate(CAnimTriple *p_triple)
{
	if (pCurAnimTriple && pCurAnimTriple->is_active()) pCurAnimTriple->deactivate();
	
	pCurAnimTriple				= p_triple;
	pCurAnimTriple->activate	();

	ForceAnimSelect				();
}

void CMotionManager::TA_Deactivate()
{
	if (pCurAnimTriple && pCurAnimTriple->is_active()) pCurAnimTriple->deactivate();
	ForceAnimSelect				();
}

void CMotionManager::TA_PointBreak()
{
	if (pCurAnimTriple && pCurAnimTriple->is_active()) pCurAnimTriple->pointbreak();
	ForceAnimSelect();
}

bool CMotionManager::TA_IsActive()
{	
	if (pCurAnimTriple && pCurAnimTriple->is_active()) return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////

void CMotionManager::CheckAnimWithPath()
{
	//ANIM_ITEM_MAP_IT item_it = get_sd()->m_tAnims.find(pMonster->cur_anim.anim);
	//VERIFY(get_sd()->m_tAnims.end() != item_it);

	//bool is_moving_anim		= !fis_zero(item_it->second.velocity->velocity.linear);
	//bool is_moving_on_path	= pMonster->IsMovingOnPath();

	//if ( is_moving_on_path && is_moving_anim) {
	//	pMonster->SetDirectionLook(item_it->first == eAnimDragCorpse);
	//	return;
	//}

	//if (!is_moving_on_path && is_moving_anim) {
	//	m_tpCurAnim = 0;
	//	cur_anim = eAnimStandIdle;
	//	pMonster->m_velocity_linear.current	 = pMonster->m_velocity_linear.target	= 0.f;
	//	return;
	//}

	//if (is_moving_on_path && !is_moving_anim) {
	//	pMonster->m_velocity_linear.current	 = pMonster->m_velocity_linear.target	= 0.f;
	//	return;
	//}

	
	if (pMonster->IsMovingOnPath()) {
		pMonster->HDebug->L_Clear();
		for (u32 i=0; i<pMonster->CDetailPathManager::path().size();i++) {
			xr_map<u32,CDetailPathManager::STravelParams>::const_iterator it = pMonster->m_movement_params.find(pMonster->CDetailPathManager::path()[i].velocity);
			VERIFY(it != pMonster->m_movement_params.end());
			
			if (fis_zero((*it).second.linear_velocity))
				pMonster->HDebug->L_AddPoint(pMonster->CDetailPathManager::path()[i].position,0.15f,D3DCOLOR_XRGB(255,0,100));
			else 
				pMonster->HDebug->L_AddPoint(pMonster->CDetailPathManager::path()[i].position,0.15f,D3DCOLOR_XRGB(0,255,100));
		}
	}


	//if (pMonster->ps_Size() < 2) return;

	//CObject::SavedPosition	pos0 = pMonster->ps_Element	(pMonster->ps_Size()-1);
	//CObject::SavedPosition	pos1 = pMonster->ps_Element	(pMonster->ps_Size()-2);

	//float h1,p1,h2,p2;
	//pMonster->Direction().getHP(h1,p1);
	//Fvector().sub(pos0.vPosition,pos1.vPosition).getHP(h2,p2);

	//if ((angle_difference(h1,h2) > PI_DIV_6)  && (pMonster->m_PhysicMovementControl->GetVelocityMagnitude() > 0)) {
	//	pMonster->m_velocity_linear.current	 = pMonster->m_velocity_linear.target = 0.f;
	//	Msg("Velocity Fixed");
	//}
}

///////////////////////////////////////////////////////////////////////////////////////
void CMotionManager::UpdateAnimCount()
{
	for (ANIM_ITEM_MAP_IT it = get_sd()->m_tAnims.begin(); it != get_sd()->m_tAnims.end(); it++)	{
		
		// ���������, ���� �� ��� ��������� ������
		if (it->second.count != 0) return;

		string128	s, s_temp; 
		u8 count = 0;
		
		for (int i=0; ; ++i) {
			if (0 != PSkeletonAnimated(pMonster->Visual())->ID_Cycle_Safe(strconcat(s_temp, *it->second.target_name,itoa(i,s,10))))  count++;
			else break;
		}

		if (count != 0) it->second.count = count;
		else {
			sprintf(s, "Error! No animation: %s for monster %s", *it->second.target_name, *pMonster->cName());
			R_ASSERT2(count != 0, s);
		} 
	}
}

CMotionDef *CMotionManager::get_motion_def(ANIM_ITEM_MAP_IT &it, u32 index)
{
	string128 s1,s2;
	return PSkeletonAnimated(pMonster->Visual())->ID_Cycle_Safe(strconcat(s2,*it->second.target_name,itoa(index,s1,10)));
}


void CMotionManager::ActivateJump()
{
	pJumping = dynamic_cast<CJumping *>(pMonster);
}
void CMotionManager::DeactivateJump()
{
	pJumping = 0;
}

