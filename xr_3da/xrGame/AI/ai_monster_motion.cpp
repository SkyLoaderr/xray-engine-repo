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

	should_play_die_anim	= true;			//���� ���� �� NetSpawn ������ ��������������� � true

	Seq_Init				();

	b_end_transition		= false;
	saved_anim				= cur_anim;

	fx_time_last_play		= 0;
	
	b_forced_velocity		= false;
	
	accel_init				();

	aa_time_last_attack		= 0;
}

// ������������� ������� ��������, ������� ���������� ���������.
// ���������� false, ���� � ����� �������� ��� �������������
bool CMotionManager::PrepareAnimation()
{
	if (pJumping && pJumping->IsActive())  return pJumping->PrepareAnimation(&m_tpCurAnim);
	if (0 != m_tpCurAnim) return false;

	// �������� �� ����������� �������� ������
	if (!pMonster->g_Alive()) 
		if (should_play_die_anim) {
			should_play_die_anim = false;  // �������� �������� ������ ������ ���
			if (_sd->m_tAnims.find(eAnimDie) != _sd->m_tAnims.end()) cur_anim = eAnimDie;
			else return false;
		} else return false;

	// �������� ������� SAnimItem ��������������� cur_anim
	ANIM_ITEM_MAP_IT anim_it = _sd->m_tAnims.find(cur_anim);
	R_ASSERT(_sd->m_tAnims.end() != anim_it);

	// ���������� ����������� ������
	int index;
	if (-1 != anim_it->second.spec_id) index = anim_it->second.spec_id;
	else {
		R_ASSERT(!anim_it->second.pMotionVect.empty());
		index = ::Random.randI(anim_it->second.pMotionVect.size());
	}

	// ���������� ��������	
	m_tpCurAnim = anim_it->second.pMotionVect[index];

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

	TRANSITION_ANIM_VECTOR_IT I = _sd->m_tTransitions.begin();
	bool bVectEmpty = _sd->m_tTransitions.empty();

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
				I = _sd->m_tTransitions.begin();			// ������ �������
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


// ��������� �������� � ������� ��������� ��� cur_anim
void CMotionManager::ApplyParams()
{
	ANIM_ITEM_MAP_IT	item_it = _sd->m_tAnims.find(cur_anim);
	R_ASSERT(_sd->m_tAnims.end() != item_it);

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
		prev_anim = cur_anim = _sd->m_tMotions[m_tAction].anim;
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
	AA_MAP_IT it = _sd->aa_map.find(pMonster->cur_anim.name);
	if (it == _sd->aa_map.end()) return false;
	
	// ��������� ��������� ����� ���� � ������������ � ����������� �������� �����
	TTime offset_time = pMonster->cur_anim.started + u32(1000 * GetAnimTime(*pMonster->cur_anim.name) * it->second.time);

	//Msg("Anim = [%s] Anim time = [%f]", *pMonster->cur_anim.name, GetAnimTime(*pMonster->cur_anim.name));

	if ((offset_time >= (cur_time - TIME_OFFSET)) && (offset_time <= (cur_time + TIME_OFFSET)) ){
		params = it->second;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////


EPState	CMotionManager::GetState (EMotionAnim a)
{
	// ����� �������� 
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
	// �������� ������� SAnimItem ��������������� anim
	ANIM_ITEM_MAP_IT anim_it = _sd->m_tAnims.find(anim);
	R_ASSERT(_sd->m_tAnims.end() != anim_it);

	CMotionDef			*def = anim_it->second.pMotionVect[index];
	CBoneData			&bone_data = PKinematics(pVisual)->LL_GetData(0);
	CBoneDataAnimated	*bone_anim = dynamic_cast<CBoneDataAnimated *>(&bone_data);

	return  bone_anim->Motions[def->motion].GetLength();
}

float CMotionManager::GetAnimTime(LPCSTR anim_name)
{
	CMotionDef			*def		= PSkeletonAnimated(pVisual)->ID_Cycle(anim_name);
	CBoneData			&bone_data	= PSkeletonAnimated(pVisual)->LL_GetData(0);
	CBoneDataAnimated	*bone_anim	= dynamic_cast<CBoneDataAnimated *>(&bone_data);

	return  bone_anim->Motions[def->motion].GetLength() / def->Dequantize(def->speed);
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
		R_ASSERT(it != pMonster->m_movement_params.end());

		path_vel.set(_abs((*it).second.linear_velocity), (*it).second.angular_velocity);
	}

	ANIM_ITEM_MAP_IT	item_it = _sd->m_tAnims.find(cur_anim);
	R_ASSERT(_sd->m_tAnims.end() != item_it);

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
		pMonster->m_velocity_linear.target	= 0.1f;
	}

	// ��������� ������������� �������� �� ���������� ��������
	float  real_speed = pMonster->m_fCurSpeed;
	EMotionAnim new_anim;
	float		a_speed;

	if (accel_chain_get(real_speed, cur_anim,new_anim, a_speed)) {
		cur_anim = new_anim;
		pMonster->SetAnimSpeed(a_speed);
	} else pMonster->SetAnimSpeed(-1.f);

	// ��������� ������� ��������
	if (!b_forced_velocity) {
		item_it = _sd->m_tAnims.find(cur_anim);
		R_ASSERT(_sd->m_tAnims.end() != item_it);

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


void CMotionManager::STEPS_Update(u8 legs_num)
{
	if (step_info.disable) return;
	
	SGameMtlPair* mtl_pair		= pMonster->CMaterialManager::get_current_pair();
	if (!mtl_pair) return;
	if (mtl_pair->StepSounds.empty()) return;

	// �������� ��������� ����
	SStepParam &step		= step_info.params;
	TTime		cur_time	= Level().timeServer();
	
	for (u32 i=0; i<legs_num; i++) {
		
		// ���� ������� ��� ���������� ��� ���� ����, �� skip
		if (step_info.activity[i].handled) continue;
		
		// ��������� ��������� ����� ���� � ������������ � ����������� �������� ������
		TTime offset_time = pMonster->cur_anim.started + u32(1000 * GetAnimTime(*pMonster->cur_anim.name) * step.step[i].time);
		
		if ((offset_time >= (cur_time - TIME_OFFSET)) && (offset_time <= (cur_time + TIME_OFFSET)) ){
			
			// Play Sound
			step_info.activity[i].sound = SELECT_RANDOM(mtl_pair->StepSounds);
			step_info.activity[i].sound.play_at_pos	(pMonster,pMonster->Position());
			//step_info.activity[i].sound.set_volume	();

			// Play Particle
			// Play Camera FXs

			step_info.activity[i].handled = true;
		}
	}

	// ��������������� �������� �����
	for (u32 i=0; i<legs_num; i++) {
		if (step_info.activity[i].handled && step_info.activity[i].sound.feedback) {
			step_info.activity[i].sound.set_position	(pMonster->Position());
		}
	}
}

void CMotionManager::STEPS_Initialize() 
{
	// ������ ������� �������� � STEPS_MAP
	STEPS_MAP_IT it = _sd->steps_map.find(pMonster->cur_anim.name);
	if (it == _sd->steps_map.end()) {
		step_info.disable = true;
		return;
	}

	step_info.disable = false;

	for (u32 i=0; i<4; i++) step_info.activity[i].handled = false;

	step_info.params = it->second;
}

