#include "stdafx.h"
#include "poltergeist.h"

CPoltergeist::CPoltergeist()
{
	stateRest			= xr_new<CBitingRest>(this);

	CurrentState		= stateRest;
	CurrentState->Reset	();
	
	m_particles_object	= 0;
	m_hidden			= false;

	m_time_flame_started = 0;
}

CPoltergeist::~CPoltergeist()
{
	xr_delete		(stateRest);
	StopParticles	();
}

void CPoltergeist::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.accel_load			(section);

	LoadFlame(section);

	m_height				= 2.f;
	
	m_particles_hidden		= pSettings->r_string(section,"Hidden_Particles");

	if (!MotionMan.start_load_shared(SUB_CLS_ID)) return;

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_walk_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimMiscAction_00,	"fall_down_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimMiscAction_01,	"fly_",					-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_REST,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
	MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

	MotionMan.finish_load_shared();
}

void CPoltergeist::reload(LPCSTR section)
{
	inherited::reload(section);
}

void CPoltergeist::StateSelector()
{	
	SetState(stateRest);
}

void CPoltergeist::Hide()
{
	m_hidden   =true;	
	
	setEnabled(false);
	setVisible(false);
	
	StartParticles();
	MotionMan.ForceAnimSelect();
}

void CPoltergeist::Show()
{
	m_hidden = false;
	
	setEnabled(true);
	setVisible(true);

	StopParticles();
	
	MotionMan.Seq_Add	(eAnimMiscAction_00);
	MotionMan.Seq_Switch();
}


void CPoltergeist::UpdateCL()
{
	inherited::UpdateCL();

	UpdateParticles();
	
}

void CPoltergeist::ForceFinalAnimation()
{
	if (m_hidden) {
		MotionMan.SetCurAnim(eAnimMiscAction_01);
	}
}

//////////////////////////////////////////////////////////////////////////
// Particles
//////////////////////////////////////////////////////////////////////////

void CPoltergeist::StartParticles()
{
	if(m_particles_object != NULL) {
		UpdateParticles();
		return;
	}

	m_particles_object = xr_new<CParticlesObject>(m_particles_hidden, false);
	UpdateParticles();
	m_particles_object->Play();
}

void CPoltergeist::StopParticles ()
{
	if(m_particles_object == NULL) return;

	m_particles_object->Stop		();
	m_particles_object->PSI_destroy	();
	m_particles_object				= NULL;

}
void CPoltergeist::UpdateParticles()
{
	if(!m_particles_object) return;
	
	// вычислить позицию и направленность партикла
	Fmatrix particles_pos; 
	particles_pos.identity();
	particles_pos.k.set(Fvector().set(0.f,1.f,0.f));
	Fvector::generate_orthonormal_basis_normalized(particles_pos.k,particles_pos.j,particles_pos.i);

	// установить позицию
	Fvector new_pos;
	new_pos = Position();
	new_pos.y += m_height;
	particles_pos.translate_over(new_pos);
	m_particles_object->SetXFORM(particles_pos);

	if(!m_particles_object->IsAutoRemove() && !m_particles_object->IsLooped() && !m_particles_object->PSI_alive()) {
		m_particles_object->Stop		();
		m_particles_object->PSI_destroy	();
		m_particles_object				= NULL;
	}
}



void CPoltergeist::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	CTelekinesis::schedule_update();
	
	UpdateFlame();
}


