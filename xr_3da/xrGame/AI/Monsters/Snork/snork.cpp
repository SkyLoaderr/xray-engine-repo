#include "stdafx.h"
#include "snork.h"
#include "snork_state_manager.h"
#include "../../../detail_path_manager_space.h"
#include "../../../detail_path_manager.h"
#include "../../../level.h"
#include "../monster_velocity_space.h"
#include "../../../sound_player.h"
#include "../control_animation_base.h"
#include "../control_movement_base.h"
#include "../../../level_debug.h"

#include "../../../PHMovementControl.h"

CSnork::CSnork() 
{
	StateMan		= xr_new<CStateManagerSnork>	(this);
	com_man().add_ability(ControlCom::eControlJump);
}

CSnork::~CSnork()
{
	xr_delete		(StateMan);
}

void CSnork::Load(LPCSTR section)
{
	inherited::Load			(section);

	anim().accel_load	(section);

	anim().AddReplacedAnim(&m_bDamaged, eAnimStandIdle,	eAnimStandDamaged);
	anim().AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	anim().AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	SVelocityParam &velocity_none		= move().get_velocity(MonsterMovement::eVelocityParameterIdle);	
	SVelocityParam &velocity_turn		= move().get_velocity(MonsterMovement::eVelocityParameterStand);
	SVelocityParam &velocity_walk		= move().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam &velocity_run		= move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	SVelocityParam &velocity_walk_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
	SVelocityParam &velocity_run_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
	SVelocityParam &velocity_steal		= move().get_velocity(MonsterMovement::eVelocityParameterSteal);
	//SVelocityParam &velocity_drag		= move().get_velocity(MonsterMovement::eVelocityParameterDrag);

	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimStandDamaged,	"stand_idle_damaged_",	-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimWalkDamaged,	"stand_walk_damaged_",	-1,	&velocity_walk_dmg,	PS_STAND);
	anim().AddAnim(eAnimRunDamaged,		"stand_run_damaged_",	-1,	&velocity_run_dmg,	PS_STAND);
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimWalkFwd,		"stand_walk_fwd_",		-1,	&velocity_walk,		PS_STAND);
	anim().AddAnim(eAnimRun,			"stand_run_",			-1,	&velocity_run,		PS_STAND);
	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimDie,			"stand_die_",			0,  &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimLookAround,		"stand_look_around_",	-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,	PS_STAND);
	anim().AddAnim(eAnimEat,			"stand_eat_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimCheckCorpse,	"stand_check_corpse_",	-1,	&velocity_none,		PS_STAND);


	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimStandIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,	eAnimWalkFwd);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,		eAnimStandIdle);
	anim().LinkAction(ACT_REST,			eAnimStandIdle);
	anim().LinkAction(ACT_DRAG,			eAnimStandIdle);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,		eAnimSteal);
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);

#ifdef DEBUG	
	anim().accel_chain_test		();
#endif

	m_fsVelocityJumpOne.Load(section, "Velocity_Jump_Stand");
	m_fsVelocityJumpTwo.Load(section, "Velocity_Jump_Forward");
}

void CSnork::reinit()
{
	inherited::reinit	();
	
	movement().detail().add_velocity(MonsterMovement::eSnorkVelocityParameterJumpOne,	CDetailPathManager::STravelParams(m_fsVelocityJumpOne.velocity.linear,	m_fsVelocityJumpOne.velocity.angular_path, m_fsVelocityJumpOne.velocity.angular_real));
	movement().detail().add_velocity(MonsterMovement::eSnorkVelocityParameterJumpTwo,	CDetailPathManager::STravelParams(m_fsVelocityJumpTwo.velocity.linear,	m_fsVelocityJumpTwo.velocity.angular_path, m_fsVelocityJumpTwo.velocity.angular_real));
	
	com_man().load_jump_data("stand_attack_2_0", "stand_attack_2_1", "stand_somersault_0", MonsterMovement::eSnorkVelocityParamsJump);
}

void CSnork::UpdateCL()
{
	inherited::UpdateCL	();

	//////////////////////////////////////////////////////////////////////////
	//CObject *obj = Level().CurrentEntity();
	//if (!obj) return;
	
	//find_geometry	();
	//////////////////////////////////////////////////////////////////////////


	CObject *actor = Level().CurrentEntity();
	CEntityAlive *obj = smart_cast<CEntityAlive *>(actor);

	Fvector V1,V2;
	obj->movement_control()->GetCharacterVelocity(V1);
	V2.set(obj->movement_control()->GetVelocity());
	
	string128 st;
	sprintf(st,"V1=[%f,%f,%f] V2=[%f,%f,%f]",VPUSH(V1),VPUSH(V2));
#ifdef DEBUG
	DBG().text(this).clear();
	DBG().text(this).add_item("", 200, 100, D3DCOLOR_XRGB(255,0,255));
#endif
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

void CSnork::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		com_man().seq_run(anim().get_motion_id(eAnimCheckCorpse));	
	}

	if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
		anim().SetCurAnim(eAnimLookAround);
		return;
	}
}

void CSnork::HitEntityInJump(const CEntity *pEntity)
{
	
	SAAParam &params	= anim().AA_GetParams("stand_attack_2_1");
	HitEntity			(pEntity, params.hit_power, params.impulse, params.impulse_dir);
}

bool CSnork::jump(CObject *enemy)
{
	if (com_man().script_jump (enemy)) {
		sound().play			(MonsterSpace::eMonsterSoundAttack);
		return true;
	}
	
	return false;
}

