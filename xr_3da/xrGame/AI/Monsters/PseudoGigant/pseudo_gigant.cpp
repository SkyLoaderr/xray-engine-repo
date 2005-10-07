#include "stdafx.h"
#include "pseudo_gigant.h"
#include "pseudo_gigant_step_effector.h"
#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "../../../level.h"
#include "pseudogigant_state_manager.h"
#include "../monster_velocity_space.h"
#include "../control_animation_base.h"
#include "../control_movement_base.h"
#include "../ai_monster_effector.h"
#include "../../../../CameraBase.h"
#include "../../../xr_level_controller.h"
#include "../../../detail_path_manager_space.h"
#include "../../../detail_path_manager.h"

const u32 pmt_threaten_delay = 30000;

CPseudoGigant::CPseudoGigant()
{
	CControlled::init_external(this);

	StateMan = xr_new<CStateManagerGigant>(this);
	
	com_man().add_ability(ControlCom::eControlRunAttack);
	com_man().add_ability(ControlCom::eControlThreaten);
	com_man().add_ability(ControlCom::eControlJump);
	com_man().add_ability(ControlCom::eControlRotationJump);
}

CPseudoGigant::~CPseudoGigant()
{
	xr_delete(StateMan);
}


void CPseudoGigant::Load(LPCSTR section)
{
	inherited::Load	(section);

	anim().AddReplacedAnim(&m_bDamaged,			eAnimRun,		eAnimRunDamaged);
	anim().AddReplacedAnim(&m_bDamaged,			eAnimWalkFwd,	eAnimWalkDamaged);
	anim().AddReplacedAnim(&m_bRunTurnLeft,		eAnimRun,		eAnimRunTurnLeft);
	anim().AddReplacedAnim(&m_bRunTurnRight,	eAnimRun,		eAnimRunTurnRight);

	anim().accel_load			(section);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	anim().accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	step_effector.time			= pSettings->r_float(section,	"step_effector_time");
	step_effector.amplitude		= pSettings->r_float(section,	"step_effector_amplitude");
	step_effector.period_number	= pSettings->r_float(section,	"step_effector_period_number");

	SVelocityParam &velocity_none		= move().get_velocity(MonsterMovement::eVelocityParameterIdle);	
	SVelocityParam &velocity_turn		= move().get_velocity(MonsterMovement::eVelocityParameterStand);
	SVelocityParam &velocity_walk		= move().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam &velocity_run		= move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	SVelocityParam &velocity_walk_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
	SVelocityParam &velocity_run_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
	SVelocityParam &velocity_steal		= move().get_velocity(MonsterMovement::eVelocityParameterSteal);

	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimLieIdle,		"stand_sleep_",			-1, &velocity_none,		PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSitIdle,		"sit_idle_",			-1, &velocity_none,		PS_SIT,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSleep,			"stand_sleep_",			-1, &velocity_none,		PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimWalkFwd,		"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimWalkDamaged,	"stand_walk_fwd_dmg_",	-1, &velocity_walk_dmg,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimRun,			"stand_run_fwd_",		-1,	&velocity_run,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&velocity_run_dmg,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimEat,			"stand_eat_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimLookAround,		"stand_idle_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimDie,			"stand_idle_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	
	anim().AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1, &velocity_none,		PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	//anim().AddAnim(eAnimThreaten,		"stand_kick_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	anim().AddAnim(eAnimRunTurnLeft,	"stand_run_left_",		-1, &velocity_run,		PS_STAND);
	anim().AddAnim(eAnimRunTurnRight,	"stand_run_right_",		-1, &velocity_run,		PS_STAND);

	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,	eAnimWalkFwd);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,		eAnimSleep);
	anim().LinkAction(ACT_REST,			eAnimSleep);
	anim().LinkAction(ACT_DRAG,			eAnimWalkFwd);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,		eAnimSteal);
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);

	// define transitions																											
	anim().AddTransition(eAnimStandLieDown,	eAnimSleep,		eAnimLieToSleep,		false);										
	anim().AddTransition(PS_STAND,			eAnimSleep,		eAnimStandLieDown,		true);
	anim().AddTransition(PS_STAND,			PS_LIE,			eAnimStandLieDown,		false);

#ifdef DEBUG	
	anim().accel_chain_test		();
#endif


	// Load psi postprocess --------------------------------------------------------
	LPCSTR ppi_section = pSettings->r_string(section, "threaten_effector");
	m_threaten_effector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	m_threaten_effector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	m_threaten_effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	m_threaten_effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	m_threaten_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	m_threaten_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	m_threaten_effector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(m_threaten_effector.ppi.noise.fps));

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &m_threaten_effector.ppi.color_base.r,	&m_threaten_effector.ppi.color_base.g,	&m_threaten_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &m_threaten_effector.ppi.color_gray.r,	&m_threaten_effector.ppi.color_gray.g,	&m_threaten_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &m_threaten_effector.ppi.color_add.r,	&m_threaten_effector.ppi.color_add.g,	&m_threaten_effector.ppi.color_add.b);

	m_threaten_effector.time			= pSettings->r_float(ppi_section,"time");
	m_threaten_effector.time_attack	= pSettings->r_float(ppi_section,"time_attack");
	m_threaten_effector.time_release	= pSettings->r_float(ppi_section,"time_release");

	m_threaten_effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	m_threaten_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	m_threaten_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	m_threaten_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	// --------------------------------------------------------------------------------
	

	::Sound->create(m_sound_threaten_hit,TRUE, pSettings->r_string(section,"sound_threaten_hit"), SOUND_TYPE_WORLD);
	::Sound->create(m_sound_start_threaten,TRUE, pSettings->r_string(section,"sound_threaten_start"), SOUND_TYPE_MONSTER_ATTACKING);

	m_kick_damage = pSettings->r_float(section,"HugeKick_Damage");
}

void CPseudoGigant::reinit()
{
	inherited::reinit();

	m_time_last_threaten = 0;

	move().load_velocity(*cNameSect(), "Velocity_JumpPrepare",MonsterMovement::eGiantVelocityParameterJumpPrepare);
	move().load_velocity(*cNameSect(), "Velocity_JumpGround",MonsterMovement::eGiantVelocityParameterJumpGround);
	
	com_man().load_jump_data(0,"jump_attack_0", "jump_attack_1", "jump_attack_2", MonsterMovement::eGiantVelocityParameterJumpPrepare, MonsterMovement::eGiantVelocityParameterJumpGround,0);
	com_man().add_rotation_jump_data("1","2","3","4", PI_DIV_2);
}



#define MAX_STEP_RADIUS 60.f

void CPseudoGigant::event_on_step()
{
	//////////////////////////////////////////////////////////////////////////
	// Earthquake Effector	//////////////
	CActor* pActor =  smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor)
	{
		float dist_to_actor = pActor->Position().distance_to(Position());
		float max_dist		= MAX_STEP_RADIUS;
		if (dist_to_actor < max_dist) 
			pActor->EffectorManager().AddEffector(xr_new<CPseudogigantStepEffector>(
				step_effector.time, 
				step_effector.amplitude, 
				step_effector.period_number, 
				(max_dist - dist_to_actor) / (1.2f * max_dist))
			);
	}
	//////////////////////////////////
}

bool CPseudoGigant::check_start_conditions(ControlCom::EControlType type)
{
	if (type == ControlCom::eControlRunAttack)
		return true;

	if (type == ControlCom::eControlThreaten) {
		if (m_time_last_threaten + pmt_threaten_delay > time()) return false;
	}

	if (type == ControlCom::eControlRotationJump) {
		EMonsterState state = StateMan->get_state_type();
		if (!is_state(state, eStateAttack)) return false;
	}

	return true;
}

void CPseudoGigant::on_activate_control(ControlCom::EControlType type)
{
	if (type == ControlCom::eControlThreaten) {
		m_sound_start_threaten.play_at_pos(this,get_head_position(this));
		m_time_last_threaten = time();
	}
}

void CPseudoGigant::on_threaten_execute()
{
	// hit objects
	xr_vector<CObject*> tpObjects;
	Level().ObjectSpace.GetNearest(tpObjects,Position(), 15.f, NULL); 

	for (u32 i=0;i<tpObjects.size();i++) {
		CPhysicsShellHolder  *obj = smart_cast<CPhysicsShellHolder *>(tpObjects[i]);
		if (!obj || !obj->m_pPhysicsShell) continue;

		Fvector dir;
		Fvector pos;
		pos.set(obj->Position());
		pos.y += 2.f;
		dir.sub(pos, Position());
		dir.normalize();
		obj->m_pPhysicsShell->applyImpulse(dir,20 * obj->m_pPhysicsShell->getMass());
	}

	Fvector		pos;
	pos.set		(Position());
	pos.y		+= 0.1f;
	m_sound_threaten_hit.play_at_pos(this,pos);

	CActor *pA = const_cast<CActor *>(smart_cast<const CActor *>(EnemyMan.get_enemy()));
	if (!pA) return;
	if (pA->is_jump()) return;

	pA->EffectorManager().AddEffector(xr_new<CMonsterEffectorHit>(m_threaten_effector.ce_time,m_threaten_effector.ce_amplitude,m_threaten_effector.ce_period_number,m_threaten_effector.ce_power));
	Level().Cameras.AddEffector(xr_new<CMonsterEffector>(m_threaten_effector.ppi, m_threaten_effector.time, m_threaten_effector.time_attack, m_threaten_effector.time_release));

	if (pA->cam_Active()) {
		pA->cam_Active()->Move(Random.randI(2) ? kRIGHT : kLEFT, Random.randF(0.3f)); 
		pA->cam_Active()->Move(Random.randI(2) ? kUP	: kDOWN, Random.randF(0.3f)); 
	}

	NET_Packet	l_P;
	u_EventGen	(l_P,GE_HIT, pA->ID());
	l_P.w_u16	(ID());
	l_P.w_u16	(ID());
	l_P.w_dir	(Fvector().set(0.f,1.f,0.f));
	l_P.w_float	(m_kick_damage);
	l_P.w_s16	(smart_cast<CKinematics*>(pA->Visual())->LL_GetBoneRoot());
	l_P.w_vec3	(Fvector().set(0.f,0.f,0.f));
	l_P.w_float	(20 * pA->movement_control()->GetMass());
	l_P.w_u16	( u16(ALife::eHitTypeWound) );
	u_EventSend	(l_P);	

}

void CPseudoGigant::HitEntityInJump		(const CEntity *pEntity) 
{
	SAAParam &params	= anim().AA_GetParams("jump_attack_1");
	HitEntity			(pEntity, params.hit_power, params.impulse, params.impulse_dir);
}
