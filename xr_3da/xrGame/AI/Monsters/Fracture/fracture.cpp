#include "stdafx.h"
#include "fracture.h"

CFracture::CFracture()
{
	stateRest			= xr_new<CBitingRest>(this);

	CurrentState		= stateRest;
	CurrentState->Reset	();
}

CFracture::~CFracture()
{
	xr_delete(stateRest);
}

void CFracture::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.accel_load			(section);

	if (!MotionMan.start_load_shared(SUB_CLS_ID)) return;

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_idle_",			0, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_RUN,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_REST,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_STEAL,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

	MotionMan.finish_load_shared();
}

void CFracture::StateSelector()
{
	SetState(stateRest);
}

