#include "stdafx.h"
#include "poltergeist.h"
#include "poltergeist_state_manager.h"
#include "../../../PHMovementControl.h"
#include "../../../PhysicsShellHolder.h"
#include "../ai_monster_utils.h"
#include "../../../ai_debug.h"
#include "poltergeist_movement.h"
#include "../../../detail_path_manager.h"



#define HEIGHT_CHANGE_VELOCITY	0.5f
#define HEIGHT_CHANGE_MIN_TIME	3000
#define HEIGHT_CHANGE_MAX_TIME	10000
#define HEIGHT_MIN				0.4f
#define HEIGHT_MAX				2.0f


CPoltergeist::CPoltergeist()
{
	m_particles_object	= 0;

	StateMan = xr_new<CStateManagerPoltergeist>(this);

	invisible_vel.set	(0.1f, 0.1f);
}

CPoltergeist::~CPoltergeist()
{
	xr_delete		(StateMan);
}

void CPoltergeist::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);

	LoadFlame(section);

	m_particles_hidden		= pSettings->r_string(section,"Hidden_Particles");
	m_particles_hide		= pSettings->r_string(section,"Hide_Particles");

	invisible_vel.set(pSettings->r_float(section,"Velocity_Invisible_Linear"),pSettings->r_float(section,"Velocity_Invisible_Angular"));
	movement().detail().add_velocity(eVelocityParameterInvisible,CDetailPathManager::STravelParams(invisible_vel.linear, invisible_vel.angular));

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd, eAnimWalkDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,	 eAnimRunDamaged);
	
	if (MotionMan.start_load_shared(CLS_ID)) {

		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimDie,				"stand_idle_",			 0, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimMiscAction_00,	"fall_down_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimMiscAction_01,	"fly_",					-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_walk_dmg_",		-1, &inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	

		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_REST,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();

	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif


	m_flame_delay.min			= pSettings->r_float(section,"Delay_Flame_Min");
	m_flame_delay.normal		= pSettings->r_float(section,"Delay_Flame_Normal");
	m_flame_delay.aggressive	= pSettings->r_float(section,"Delay_Flame_Aggressive");

	m_tele_delay.min			= pSettings->r_float(section,"Delay_Tele_Min");
	m_tele_delay.normal			= pSettings->r_float(section,"Delay_Tele_Normal");
	m_tele_delay.aggressive		= pSettings->r_float(section,"Delay_Tele_Aggressive");

	m_scare_delay.min			= pSettings->r_float(section,"Delay_Scare_Min");
	m_scare_delay.normal		= pSettings->r_float(section,"Delay_Scare_Normal");
	m_scare_delay.aggressive	= pSettings->r_float(section,"Delay_Scare_Aggressive");
}

void CPoltergeist::reload(LPCSTR section)
{
	inherited::reload(section);
	Energy::reload(section,"Invisible_");
}

void CPoltergeist::reinit()
{
	inherited::reinit();
	Energy::reinit();

	m_current_position = Position();

	time_tele_start		= 0;
	tele_enemy			= 0;	

	target_height		= 0.3f;
	time_height_updated = 0;

	Energy::set_auto_activate();
	Energy::set_auto_deactivate();
	Energy::enable();

	// start hidden
	state_invisible						= true;	
	setVisible							(false);
	
	MotionMan.ForceAnimSelect			();
	
	m_current_position = Position		();
	movement_control()->DestroyCharacter();
	
	m_height							= 0.3f;
	time_height_updated					= 0;
}

void CPoltergeist::Hide()
{
	if (state_invisible) return;
	
	state_invisible   = true;	

	setVisible(false);
	
	MotionMan.ForceAnimSelect();

	m_current_position = Position		();
	movement_control()->DestroyCharacter();

	CParticlesPlayer::StartParticles(m_particles_hidden,Fvector().set(0.0f,0.1f,0.0f),ID());
	CParticlesPlayer::StartParticles(m_particles_hide,Fvector().set(0.0f,0.1f,0.0f),ID());
}

void CPoltergeist::Show()
{
	if (!state_invisible) return;

	state_invisible = false;
	
	setVisible(TRUE);

	
	MotionMan.Seq_Add		(eAnimMiscAction_00);
	MotionMan.Seq_Switch	();

	Position() = m_current_position;
	movement_control()->SetPosition(Position());
	movement_control()->CreateCharacter();
	
	CParticlesPlayer::StartParticles(m_particles_hide,Fvector().set(0.0f,0.1f,0.0f),ID());
	CParticlesPlayer::StopParticles(m_particles_hidden);
}

void CPoltergeist::UpdateCL()
{
	inherited::UpdateCL();
	
	def_lerp(m_height, target_height, HEIGHT_CHANGE_VELOCITY, Device.fTimeDelta);
}

void CPoltergeist::ForceFinalAnimation()
{
	if (state_invisible) 
		MotionMan.SetCurAnim(eAnimMiscAction_01);
}


void CPoltergeist::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	CTelekinesis::schedule_update();
	Energy::schedule_update();

	UpdateFlame();
	UpdateTelekinesis();
	UpdateHeight();
}

BOOL CPoltergeist::net_Spawn (CSE_Abstract* DC) 
{
	if (!inherited::net_Spawn(DC)) return(FALSE);

	// спаунится нивидимым
	setVisible		(false);

	CParticlesPlayer::StartParticles(m_particles_hidden,Fvector().set(0.0f,0.1f,0.0f),ID());
	CParticlesPlayer::StartParticles(m_particles_hide,Fvector().set(0.0f,0.1f,0.0f),ID());
	
	return			(TRUE);
}

void CPoltergeist::net_Destroy()
{
	inherited::net_Destroy();
	RemoveFlames();
	Energy::disable();
}

void CPoltergeist::Die(CObject* who)
{
	if (state_invisible) {
		setVisible(true);
		Position() = m_current_position;
		CParticlesPlayer::StopParticles(m_particles_hidden);
	}

	inherited::Die(who);
	Energy::disable();
}

void CPoltergeist::UpdateHeight()
{
	if (!state_invisible) return;
	
	u32 cur_time = Device.dwTimeGlobal;
	
	if (time_height_updated < cur_time)	{
		time_height_updated = cur_time + Random.randI(HEIGHT_CHANGE_MIN_TIME,HEIGHT_CHANGE_MAX_TIME);
		target_height		= Random.randF(HEIGHT_MIN, HEIGHT_MAX);		
	}
}

void CPoltergeist::on_activate()
{
	if (m_disable_hide) return;

	Hide();
	
	m_height			= 0.3f;
	time_height_updated = 0;
}

void CPoltergeist::on_deactivate()
{
	if (m_disable_hide) return;

	Show();
}

CMovementManager *CPoltergeist::create_movement_manager	()
{
	return		(m_movement_manager = xr_new<CPoltergeisMovementManager>(this));
}

#ifdef DEBUG
CBaseMonster::SDebugInfo CPoltergeist::show_debug_info()
{
	CBaseMonster::SDebugInfo info = inherited::show_debug_info();
	if (!info.active) return CBaseMonster::SDebugInfo();

	string128 text;
	sprintf(text, "Invisibility Value = [%f]", Energy::get_value());
	DBG().text(this).add_item(text, info.x, info.y+=info.delta_y, info.color);
	DBG().text(this).add_item("---------------------------------------", info.x, info.y+=info.delta_y, info.delimiter_color);

	return CBaseMonster::SDebugInfo();
}
#endif

