#include "stdafx.h"
#include "controller.h"
#include "../../ai_monster_utils.h"
#include "controller_state_manager.h"
#include "../../controlled_entity.h"
#include "../../ai_monster_debug.h"
#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "../../ai_monster_effector.h"

CController::CController()
{
	StateMan = xr_new<CStateManagerController>(this);

	CJumping::Init(this);
}

CController::~CController()
{
	xr_delete(StateMan);
}

void CController::Load(LPCSTR section)
{
	inherited::Load	(section);
	CJumping::Load	(section);

	m_max_controlled_number			= pSettings->r_u8(section,"Max_Controlled_Count");
	m_controlled_objects.reserve	(m_max_controlled_number);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);

	::Sound->create(control_start_sound,TRUE, pSettings->r_string(section,"sound_control_start"),	SOUND_TYPE_WORLD);
	::Sound->create(control_hit_sound,	TRUE, pSettings->r_string(section,"sound_control_hit"),		SOUND_TYPE_WORLD);



	// Load control postprocess --------------------------------------------------------
	LPCSTR ppi_section = pSettings->r_string(section, "control_effector");
	m_control_effector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	m_control_effector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	m_control_effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	m_control_effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	m_control_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	m_control_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	m_control_effector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &m_control_effector.ppi.color_base.r,	&m_control_effector.ppi.color_base.g,	&m_control_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &m_control_effector.ppi.color_gray.r,	&m_control_effector.ppi.color_gray.g,	&m_control_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &m_control_effector.ppi.color_add.r,	&m_control_effector.ppi.color_add.g,	&m_control_effector.ppi.color_add.b);

	m_control_effector.time			= pSettings->r_float(ppi_section,"time");
	m_control_effector.time_attack	= pSettings->r_float(ppi_section,"time_attack");
	m_control_effector.time_release	= pSettings->r_float(ppi_section,"time_release");

	m_control_effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	m_control_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	m_control_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	m_control_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	if (!MotionMan.start_load_shared(SUB_CLS_ID)) return;

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimStandDamaged,	"stand_idle_dmg_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
	MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);	
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
	MotionMan.AddAnim(eAnimSleep,			"sit_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);

	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

	MotionMan.AddTransition(PS_STAND,	PS_SIT,		eAnimStandSitDown,	false);
	MotionMan.AddTransition(PS_SIT,		PS_STAND,	eAnimSitStandUp,	false);

	MotionMan.finish_load_shared();
}

bool CController::UpdateStateManager()
{
	UpdateControlled	();
	StateMan->execute	();
	return true;
}

void CController::UpdateControlled()
{
	// ���� ���� ����, ��������� ����� �� ���� ���� ���� ��� ��������
	if (EnemyMan.get_enemy()) {
		CControlledEntityBase *entity = dynamic_cast<CControlledEntityBase *>(const_cast<CEntityAlive *>(EnemyMan.get_enemy()));
		if (entity) {
			if (!entity->is_under_control() && (m_controlled_objects.size() < m_max_controlled_number)) {
				// ����� ��� ��������
				entity->set_under_control	(this);
				entity->set_task_follow		(this);
				m_controlled_objects.push_back(const_cast<CEntityAlive *>(EnemyMan.get_enemy()));
			}
		}
	}

	// ������� ������� �������	
	if (!m_controlled_objects.empty()) 
		for (int i=m_controlled_objects.size()-1; i>=0;i--) {
			if (!m_controlled_objects[i]->g_Alive() || m_controlled_objects[i]->getDestroy()) {
				m_controlled_objects[i] = m_controlled_objects.back();
				m_controlled_objects.pop_back();
			}  
		}


#ifdef DEBUG
	// Draw Controlled Lines
	HDebug->L_Clear();
	Fvector my_pos = Position();
	my_pos.y += 1.5f;
	

	for (u32 i=0; i < m_controlled_objects.size(); i++) {
		Fvector enemy_pos	= m_controlled_objects[i]->Position();
		
		Fvector dir;
		dir.sub(enemy_pos, Position());
		dir.div(2.f);
		Fvector new_pos;
		new_pos.add(Position(),dir);
		new_pos.y += 10.f;

		enemy_pos.y += 1.0f;
		HDebug->L_AddLine(my_pos,		new_pos, D3DCOLOR_XRGB(0,255,255));
		HDebug->L_AddLine(enemy_pos,	new_pos, D3DCOLOR_XRGB(0,255,255));
	}


#endif
}

void CController::set_controlled_task(u32 task)
{
	if (!HasUnderControl()) return;
	
	const CEntity *object = ((((ETask)task) == eTaskNone) ? 0 : ((((ETask)task) == eTaskFollow) ? this : EnemyMan.get_enemy()));
	
	for	(u32 i=0; i<m_controlled_objects.size(); i++) {
		CControlledEntityBase *entity = dynamic_cast<CControlledEntityBase *>(m_controlled_objects[i]);		
		entity->get_data().m_object = object;
		entity->get_data().m_task	= (ETask)task;
	}
}

void CController::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

} 

void CController::InitThink()
{
	for	(u32 i=0; i<m_controlled_objects.size(); i++) {	
		CAI_Biting *base = dynamic_cast<CAI_Biting*>(m_controlled_objects[i]);
		if (!base) continue;
		if (base->EnemyMan.get_enemy()) 
			EnemyMemory.add_enemy  (base->EnemyMan.get_enemy(), 
									base->EnemyMan.get_enemy_position(),
									base->EnemyMan.get_enemy_vertex(),
									base->EnemyMan.get_enemy_time_last_seen()
			);
	}
}

void CController::play_control_sound_start()
{
	CActor *pA = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (!pA) return;
	
	Fvector pos = pA->Position();
	pos.y += 1.5f;

	if (control_start_sound.feedback) control_start_sound.stop();
	control_start_sound.play_at_pos(pA,pos);
}

void CController::play_control_sound_hit()
{
	CActor *pA = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (!pA) return;

	Fvector pos = pA->Position();
	pos.y += 1.5f;
	
	if (control_hit_sound.feedback) control_hit_sound.stop();
	control_hit_sound.play_at_pos(pA,pos);
}

void CController::reload(LPCSTR section)
{
	inherited::reload(section);

	// Load triple gravi animations
	CMotionDef *def1, *def2, *def3;
	def1 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_sit_down_attack_0");
	VERIFY(def1);

	def2 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("control_attack_0");
	VERIFY(def2);

	def3 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("sit_stand_up_attack_0");
	VERIFY(def3);

	anim_triple_control.init_external	(def1, def2, def3);

	CJumping::AddState(PSkeletonAnimated(Visual())->ID_Cycle_Safe("jump_glide_0"), JT_GLIDE, false,	0.f, inherited::get_sd()->m_fsVelocityRunFwdNormal.velocity.angular);

}

void CController::control_hit()
{
	play_control_sound_hit();
	
	// start postprocess
	CActor *pA = dynamic_cast<CActor *>(Level().CurrentEntity());
	if (pA) {
		pA->EffectorManager().AddEffector(xr_new<CMonsterEffectorHit>(m_control_effector.ce_time,m_control_effector.ce_amplitude,m_control_effector.ce_period_number,m_control_effector.ce_power));
		Level().Cameras.AddEffector(xr_new<CMonsterEffector>(m_control_effector.ppi, m_control_effector.time, m_control_effector.time_attack, m_control_effector.time_release));
	}
}

void CController::UpdateCL()
{
	inherited::UpdateCL();
	CJumping::Update();
}

void CController::Jump()
{
	if (CJumping::Check(Position(),EnemyMan.get_enemy()->Position(),EnemyMan.get_enemy())){
		CSoundPlayer::play(MonsterSpace::eMonsterSoundAttackHit);
		MotionMan.ActivateJump();
	}
}

