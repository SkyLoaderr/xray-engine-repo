#include "stdafx.h"
#include "pseudodog.h"
#include "../ai_monster_utils.h"
#include "../ai_monster_effector.h"
#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "../../stalker/ai_stalker.h"
#include "../../../../CameraBase.h"
#include "../../../xr_level_controller.h"
#include "pseudodog_state_manager.h"
#include "../../../../skeletonanimated.h"
#include "../../../sound_player.h"
#include "../../../level.h"
#include "../../../ai_monster_space.h"
#include "../ai_monster_movement.h"
#include "../ai_monster_movement_space.h"


CAI_PseudoDog::CAI_PseudoDog()
{
	StateMan = xr_new<CStateManagerPseudodog>(this);
	
	CJumping::Init		(this);
}

CAI_PseudoDog::~CAI_PseudoDog()
{
	xr_delete(StateMan);
}

void CAI_PseudoDog::reinit()
{
	inherited::reinit();

	strike_in_jump					= false;
	m_time_became_angry				= 0;
	time_growling					= 0;
}

void CAI_PseudoDog::Load(LPCSTR section)
{
	inherited::Load	(section);
	CJumping::Load	(section);


	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);


	// Load psi postprocess --------------------------------------------------------
	LPCSTR ppi_section = pSettings->r_string(section, "psi_effector");
	m_psi_effector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	m_psi_effector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	m_psi_effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	m_psi_effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	m_psi_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	m_psi_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	m_psi_effector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(m_psi_effector.ppi.noise.fps));

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &m_psi_effector.ppi.color_base.r,	&m_psi_effector.ppi.color_base.g,	&m_psi_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &m_psi_effector.ppi.color_gray.r,	&m_psi_effector.ppi.color_gray.g,	&m_psi_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &m_psi_effector.ppi.color_add.r,	&m_psi_effector.ppi.color_add.g,	&m_psi_effector.ppi.color_add.b);

	m_psi_effector.time			= pSettings->r_float(ppi_section,"time");
	m_psi_effector.time_attack	= pSettings->r_float(ppi_section,"time_attack");
	m_psi_effector.time_release	= pSettings->r_float(ppi_section,"time_release");

	m_psi_effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	m_psi_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	m_psi_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	m_psi_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	// --------------------------------------------------------------------------------

	m_anger_hunger_threshold	= pSettings->r_float(section, "anger_hunger_threshold");
	m_anger_loud_threshold		= pSettings->r_float(section, "anger_loud_threshold");

	::Sound->create(psy_effect_sound,TRUE, pSettings->r_string(section,"sound_psy_effect"), SOUND_TYPE_WORLD);

	psy_effect_turn_angle		= angle_normalize(pSettings->r_float(section,"psy_effect_turn_angle"));

	if (MotionMan.start_load_shared(CLS_ID)) {

		SVelocityParam &velocity_none		= movement().get_velocity(MonsterMovement::eVelocityParameterIdle);	
		SVelocityParam &velocity_turn		= movement().get_velocity(MonsterMovement::eVelocityParameterStand);
		SVelocityParam &velocity_walk		= movement().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
		SVelocityParam &velocity_run		= movement().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
		SVelocityParam &velocity_walk_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
		SVelocityParam &velocity_run_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
		SVelocityParam &velocity_steal		= movement().get_velocity(MonsterMovement::eVelocityParameterSteal);
		SVelocityParam &velocity_drag		= movement().get_velocity(MonsterMovement::eVelocityParameterDrag);


		// define animation set
		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,				PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,			PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,			PS_STAND);
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &velocity_none,				PS_STAND);
		MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &velocity_none,				PS_LIE);
		MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, &velocity_none,				PS_LIE);
		MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &velocity_none,				PS_SIT);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,			PS_STAND);
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND);
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, &velocity_walk_dmg,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&velocity_run,		PS_STAND);
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &velocity_run_dmg,		PS_STAND);
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&velocity_none,				PS_STAND);
		MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &velocity_drag,				PS_STAND);
		MotionMan.AddAnim(eAnimSniff,			"stand_sniff_",			-1, &velocity_none,				PS_STAND);
		MotionMan.AddAnim(eAnimHowling,			"stand_howling_",		-1,	&velocity_none,				PS_STAND);
		MotionMan.AddAnim(eAnimJumpGlide,		"jump_glide_",			-1, &velocity_none,				PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,				PS_STAND);
		MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &velocity_none,				PS_STAND);
		MotionMan.AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, &velocity_none,				PS_SIT);
		MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &velocity_none,				PS_STAND);	
		MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &velocity_none,				PS_SIT);
		MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1,	&velocity_none,				PS_LIE);
		MotionMan.AddAnim(eAnimSleepStandUp,	"lie_to_stand_up_",		-1, &velocity_none,				PS_LIE);
		MotionMan.AddAnim(eAnimAttackPsi,		"stand_psi_attack_",	-1, &velocity_turn,			PS_STAND);
		MotionMan.AddAnim(eAnimThreaten,		"stand_howling_",		-1,	&velocity_none,				PS_STAND);


		// define transitions
		// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
		MotionMan.AddTransition(eAnimLieIdle,	eAnimSleep,	eAnimLieToSleep,	false);
		MotionMan.AddTransition(eAnimSleep,		PS_STAND,	eAnimSleepStandUp,	false);
		MotionMan.AddTransition(PS_SIT,		PS_LIE,		eAnimSitLieDown,		false);
		MotionMan.AddTransition(PS_STAND,	PS_SIT,		eAnimStandSitDown,		false);
		MotionMan.AddTransition(PS_SIT,		PS_STAND,	eAnimSitStandUp,		false);

		// define links from Action to animations
		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);	
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimSniff);

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}

void CAI_PseudoDog::reload(LPCSTR section)
{
	inherited::reload	(section);
	
	sound().add	(pSettings->r_string(section,"sound_psy_attack"),	16,	SOUND_TYPE_MONSTER_ATTACKING,	1,	u32(1 << 31) | 15,	MonsterSpace::eMonsterSoundPsyAttack, "bip01_head");
	
	
	SVelocityParam &velocity_run = movement().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	CJumping::AddState	(smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("jump_prepare_0"),	JT_CUSTOM,	true,	0.f, velocity_run.velocity.angular_real);
	CJumping::AddState	(smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("jump_glide_0"),	JT_GLIDE,	false,	0.f, velocity_run.velocity.angular_real);
}

void CAI_PseudoDog::UpdateCL()
{
	inherited::UpdateCL	();
	CJumping::Update	();

	float trace_dist = 1.5f;
	
	// ��������� �� ��������� ���� �� ����� ������
	if (CJumping::IsGlide()) {

		if (strike_in_jump) return;

		const CEntity *pE = smart_cast<const CEntity *>(CJumping::GetEnemy());
		if (!pE) return;

		Fvector trace_from;
		Center(trace_from);
		setEnabled(false);
		collide::rq_result	l_rq;

		if (Level().ObjectSpace.RayPick(trace_from, Direction(), trace_dist , collide::rqtBoth, l_rq)) {
			if ((l_rq.O == CJumping::GetEnemy()) && (l_rq.range < trace_dist)) {
				SAAParam params;
				MotionMan.AA_GetParams(params, "jump_glide_0");
				HitEntity(pE, params.hit_power, params.impulse, params.impulse_dir);
				strike_in_jump = true;
			}
		}
		setEnabled(true);			
	}

}
void CAI_PseudoDog::OnJumpStop()
{
	MotionMan.DeactivateJump();
	strike_in_jump = false;
}


void CAI_PseudoDog::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_PSI_ATTACK) == ASP_PSI_ATTACK) {
		MotionMan.Seq_Add(eAnimAttackPsi);
		MotionMan.Seq_Switch();

		CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());
		if (pA) {
			pA->EffectorManager().AddEffector(xr_new<CMonsterEffectorHit>(m_psi_effector.ce_time,m_psi_effector.ce_amplitude,m_psi_effector.ce_period_number,m_psi_effector.ce_power));
			Level().Cameras.AddEffector(xr_new<CMonsterEffector>(m_psi_effector.ppi, m_psi_effector.time, m_psi_effector.time_attack, m_psi_effector.time_release));

			if (pA->cam_Active()) {
				pA->cam_Active()->Move(Random.randI(2) ? kRIGHT : kLEFT, Random.randF(psy_effect_turn_angle)); 
				pA->cam_Active()->Move(Random.randI(2) ? kUP	: kDOWN, Random.randF(psy_effect_turn_angle)); 
			}

		}
	}

	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		MotionMan.SetCurAnim(eAnimThreaten);
	}
}


//void CAI_PseudoDog::play_effect_sound()
//{
//	CActor *pA = smart_cast<CActor*>(Level().CurrentEntity());
//	if (!pA) return;
//	
//	Fvector pos = pA->Position();
//	pos.y += 1.5f;
//	::Sound->play_at_pos(psy_effect_sound,pA,pos);
//}

