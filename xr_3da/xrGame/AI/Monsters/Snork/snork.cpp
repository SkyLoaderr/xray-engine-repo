#include "stdafx.h"
#include "snork.h"
#include "snork_state_manager.h"
#include "snork_jump.h"
#include "../ai_monster_debug.h"
#include "../../../detail_path_manager_space.h"
#include "../../../detail_path_manager.h"
#include "../ai_monster_movement.h"
#include "../../../level.h"

CSnork::CSnork() 
{
	StateMan	= xr_new<CStateManagerSnork>	(this);
	Jump		= xr_new<CSnorkJump>			(this);
}

CSnork::~CSnork()
{
	xr_delete		(StateMan);
	xr_delete		(Jump);
}

void CSnork::Load(LPCSTR section)
{
	inherited::Load			(section);
	Jump->load				(section);

	MotionMan.accel_load	(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimStandIdle,	eAnimStandDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	if (MotionMan.start_load_shared(CLS_ID)) {
		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandDamaged,	"stand_idle_damaged_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_damaged_",	-1,	&inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_damaged_",	-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimDie,				"stand_die_",			0,  &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND);
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);


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
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));
		
		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

	m_fsVelocityJumpOne.Load(section, "Velocity_Jump_Stand");
	m_fsVelocityJumpTwo.Load(section, "Velocity_Jump_Forward");
}

void CSnork::reinit()
{
	inherited::reinit	();
	
	movement().detail().add_velocity(eVelocityParameterJumpOne,	CDetailPathManager::STravelParams(m_fsVelocityJumpOne.velocity.linear,	m_fsVelocityJumpOne.velocity.angular_path, m_fsVelocityJumpOne.velocity.angular_real));
	movement().detail().add_velocity(eVelocityParameterJumpTwo,	CDetailPathManager::STravelParams(m_fsVelocityJumpTwo.velocity.linear,	m_fsVelocityJumpTwo.velocity.angular_path, m_fsVelocityJumpTwo.velocity.angular_real));
}

void CSnork::UpdateCL()
{
	inherited::UpdateCL	();
	Jump->update_frame	();

	//////////////////////////////////////////////////////////////////////////
	CObject *obj = Level().CurrentEntity();
	if (!obj) return;
	
	//find_geometry	();
	//////////////////////////////////////////////////////////////////////////
}

#define TRACE_RANGE 30.f

float CSnork::trace(const Fvector &dir)
{
	float ret_val = flt_max;

	setEnabled	(false);
	collide::rq_result	l_rq;

	Fvector		trace_from;
	Center		(trace_from);

	float		trace_dist = Radius() + TRACE_RANGE;

	if (Level().ObjectSpace.RayPick(trace_from, dir, trace_dist, collide::rqtStatic, l_rq)) {
		if ((l_rq.range < trace_dist)) ret_val = l_rq.range;
	}

	setEnabled	(true);
	
	return		ret_val;
}

#define JUMP_DISTANCE 10.f
bool CSnork::find_geometry(Fvector &dir)
{
	// 1. trace direction
	dir		= Direction();
	float	range;
	
	if (trace_geometry(dir, range)) {
		if (range < JUMP_DISTANCE) {
			return true;	
		}
	}

	return false;
}

bool CSnork::trace_geometry(const Fvector &d, float &range)
{
	Fvector				dir;
	float				h, p;

	Fvector				Pl,Pc,Pr;
	Fvector				center;
	Center				(center);

	range				= trace (d);
	if (range > TRACE_RANGE) return false;
	
	float angle			= asin(1.f / range);

	// trace center ray
	dir					= d;

	dir.getHP			(h,p);
	p					+= angle;
	dir.setHP			(h,p);
	dir.normalize_safe	();

	range				= trace (dir);
	if (range > TRACE_RANGE) return false;

	Pc.mad				(center, dir, range);

	// trace left ray
	Fvector				temp_p;
	temp_p.mad			(Pc, XFORM().i, Radius() / 2);
	dir.sub				(temp_p, center);
	dir.normalize_safe	();

	range				= trace (dir);
	if (range > TRACE_RANGE) return false;

	Pl.mad				(center, dir, range);

	// trace right ray
	Fvector inv			= XFORM().i; 
	inv.invert			();
	temp_p.mad			(Pc, inv, Radius() / 2);
	dir.sub				(temp_p, center);
	dir.normalize_safe	();

	range				= trace (dir);
	if (range > TRACE_RANGE) return false;

	Pr.mad				(center, dir, range);

	float				h1,p1,h2,p2;

	Fvector().sub(Pl, Pc).getHP(h1,p1);
	Fvector().sub(Pc, Pr).getHP(h2,p2);

	return (fsimilar(h1,h2,0.1f) && fsimilar(p1,p2,0.1f));
}


void CSnork::try_to_jump()
{
	Jump->try_to_jump(eVelocityParamsJump);
}

void CSnork::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
		MotionMan.SetCurAnim(eAnimLookAround);
		return;
	}
}

void CSnork::HitEntityInJump(const CEntity *pEntity)
{
	SAAParam params;
	MotionMan.AA_GetParams	(params, "stand_attack_2_1");
	HitEntity				(pEntity, params.hit_power, params.impulse, params.impulse_dir);
}
