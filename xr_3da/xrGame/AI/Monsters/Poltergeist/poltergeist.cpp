#include "stdafx.h"
#include "poltergeist.h"
#include "poltergeist_state_manager.h"
#include "../../../PHMovementControl.h"
#include "../../../PhysicsShellHolder.h"
#include "../../ai_monster_debug.h"
#include "../../ai_monster_utils.h"


#define HEIGHT_CHANGE_VELOCITY	0.5f
#define HEIGHT_CHANGE_MIN_TIME	3000
#define HEIGHT_CHANGE_MAX_TIME	10000
#define HEIGHT_MIN				0.4f
#define HEIGHT_MAX				2.0f


CPoltergeist::CPoltergeist()
{
	m_particles_object	= 0;
	m_hidden			= false;

	StateMan = xr_new<CStateManagerPoltergeist>(this);
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

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd, eAnimWalkDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,	 eAnimRunDamaged);
	
	if (!MotionMan.start_load_shared(SUB_CLS_ID)) return;

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
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

	MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

	MotionMan.finish_load_shared();
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

	m_disable_hide		= false;
}

void CPoltergeist::Hide()
{
	if (m_hidden) return;
	
	m_hidden   = true;	

	setVisible(false);
	
	MotionMan.ForceAnimSelect();

	m_current_position = Position		();
	movement_control()->DestroyCharacter();

	CParticlesPlayer::StartParticles(m_particles_hidden,Fvector().set(0.0f,0.1f,0.0f),ID());
	CParticlesPlayer::StartParticles(m_particles_hide,Fvector().set(0.0f,0.1f,0.0f),ID());
}

void CPoltergeist::Show()
{
	if (!m_hidden) return;

	m_hidden = false;
	
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
	if (m_hidden) 
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

void CPoltergeist::net_Destroy()
{
	inherited::net_Destroy();
	RemoveFlames();
	Energy::disable();
}

void CPoltergeist::Die()
{
	if (m_hidden) {
		setVisible(true);
		Position() = m_current_position;
		CParticlesPlayer::StopParticles(m_particles_hidden);
	}

	inherited::Die();
	Energy::disable();
}

void CPoltergeist::UpdateHeight()
{
	if (!m_hidden) return;
	
	u32 cur_time = Level().timeServer();
	
	if (time_height_updated < cur_time)	{
		time_height_updated = cur_time + Random.randI(HEIGHT_CHANGE_MIN_TIME,HEIGHT_CHANGE_MAX_TIME);
		target_height		= Random.randF(HEIGHT_MIN, HEIGHT_MAX);		
	}
}

bool CPoltergeist::UpdateStateManager()
{
	// Squad calculations
	//CMonsterSquad	*pSquad = monster_squad().get_squad(this);
	//if (pSquad && pSquad->SquadActive()) {
	//	pSquad->UpdateMonsterData(this,const_cast<CEntityAlive *>(EnemyMan.get_enemy()));
	//	if ((pSquad->GetLeader() == this)) {
	//		pSquad->UpdateDecentralized();
	//	} 
	//}
	
	StateMan->execute	();
	return true;
}

void CPoltergeist::on_activate()
{
	if (m_disable_hide) return;
	if (!m_disable_hide) return;
	

	Hide();
	
	m_height			= 0.3f;
	time_height_updated = 0;
}

void CPoltergeist::on_deactivate()
{
	if (m_disable_hide) return;

	Show();
}

