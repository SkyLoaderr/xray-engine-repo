#include "stdafx.h"
#include "burer.h"
#include "../../ai_monster_utils.h"
#include "burer_states.h"
#include "../../../PhysicsShell.h"
#include "../../../actor.h"

CBurer::CBurer() : CStateManagerBurer("Burer State Manager")
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBurerAttack>		(this);
	stateEat			= xr_new<CBitingEat>		(this);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this);
	stateNull			= xr_new<CBitingNull>		();

	CurrentState		= stateRest;

	Init();
}

CBurer::~CBurer()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
	xr_delete(stateNull);

}


void CBurer::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
}

void CBurer::reinit()
{
	inherited::reinit			();
	CStateManagerBurer::reinit	(this);	
	
}


void CBurer::reload(LPCSTR section)
{
	inherited::reload(section);

	// Load triple gravi animations
	CMotionDef *def1, *def2, *def3;
	def1 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_gravi_0");
	VERIFY(def1);

	def2 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_gravi_1");
	VERIFY(def2);

	def3 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_gravi_2");
	VERIFY(def3);

	anim_triple_gravi.init_external	(def1, def2, def3);
	
	// Load triple tele animations
	def1 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_tele_0");
	VERIFY(def1);

	def2 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_tele_1");
	VERIFY(def2);

	def3 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_tele_2");
	VERIFY(def3);

	anim_triple_tele.init_external	(def1, def2, def3);

}

void CBurer::Load(LPCSTR section)
{
	inherited::Load				(section);
	CStateManagerBurer::Load	(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimStandIdle,		eAnimStandDamaged);	
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,			eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,		eAnimWalkDamaged);
	
	BEGIN_LOAD_SHARED_MOTION_DATA();
	
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimStandDamaged,	"stand_idle_dmg_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::_sd->m_fsVelocityWalkFwdNormal,		PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::_sd->m_fsVelocityWalkFwdDamaged,	PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::_sd->m_fsVelocityRunFwdNormal,		PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::_sd->m_fsVelocityRunFwdDamaged,		PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	MotionMan.AddAnim(eAnimScared,			"stand_scared_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::_sd->m_fsVelocitySteal,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	
	
	MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimCheckCorpse,		"sit_check_corpse_",	-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	MotionMan.AddTransition(PS_SIT,				PS_STAND,		eAnimSitStandUp,	false);
	MotionMan.AddTransition(PS_STAND,			PS_SIT,			eAnimStandSitDown,	false);
	
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSitIdle);
	MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

	MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));
	MotionMan.STEPS_Load(pSettings->r_string(section, "step_params"), get_legs_number());
	
	END_LOAD_SHARED_MOTION_DATA();

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);

	particle_gravi_wave		= pSettings->r_string(section,"Particle_Gravi_Wave");
	particle_gravi_prepare	= pSettings->r_string(section,"Particle_Gravi_Prepare");
	particle_tele_object	= pSettings->r_string(section,"Particle_Tele_Object");
	
	CSoundPlayer::add(pSettings->r_string(section,"sound_gravi_attack"),	16,	SOUND_TYPE_MONSTER_ATTACKING,	2,	u32(1 << 31) | 16,	MonsterSpace::eMonsterSoundGraviAttack, "bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_tele_attack"),		16,	SOUND_TYPE_MONSTER_ATTACKING,	2,	u32(1 << 31) | 17,	MonsterSpace::eMonsterSoundTeleAttack, "bip01_head");
	
	::Sound->create(sound_gravi_wave,TRUE, pSettings->r_string(section,"sound_gravi_wave"), SOUND_TYPE_WORLD);
}

void CBurer::StateSelector()
{	
	IState *state = 0;

	if (EnemyMan.get_enemy()) {
		switch (EnemyMan.get_danger_type()) {
			case eVeryStrong:				state = statePanic; break;
			case eStrong:		
			case eNormal:
			case eWeak:						state = stateAttack; break;
		}
	} else if (hear_dangerous_sound || hear_interesting_sound) {
		if (hear_dangerous_sound)			state = stateExploreNDE;		
		if (hear_interesting_sound)			state = stateExploreNDE;	
	} else	if (CorpseMan.get_corpse() && ((GetSatiety() < _sd->m_fMinSatiety) || flagEatNow))					
											state = stateEat;	
	else									state = stateRest;

	SetState(state); 
}

void CBurer::ProcessTurn()
{
	float delta_yaw = angle_difference(m_body.target.yaw, m_body.current.yaw);
	if (delta_yaw < deg(1)) {
		//m_body.current.yaw = m_body.target.yaw;
		return;
	}

	EMotionAnim anim = MotionMan.GetCurAnim();

	bool turn_left = true;
	if (from_right(m_body.target.yaw, m_body.current.yaw)) turn_left = false; 

	switch (anim) {
		case eAnimStandIdle: 
			(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			return;
		default:
			if (delta_yaw > deg(30)) {
				(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			}
			return;
	}
}

bool CBurer::UpdateStateManager()
{
	//CStateManagerBurer::update(m_current_update - m_dwLastUpdateTime);
	return false;
}


void CBurer::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	TTelekinesis::schedule_update();
}

void CBurer::CheckSpecParams(u32 spec_params)
{
}

// speed 30 m/sec
#define SPEED						42/1000
// 1 meter
#define GRAVI_STEP					3

#define GRAVI_RADIUS				3.f
#define GRAVI_IMPULSE				8.f

void CBurer::UpdateGraviObject()
{
	if (!m_gravi_object.active) return;
	
	if (!m_gravi_object.enemy || (m_gravi_object.enemy && m_gravi_object.enemy->getDestroy())) {
		m_gravi_object.deactivate();
		return;
	}

	if (m_gravi_object.from_pos.distance_to(m_gravi_object.cur_pos) > (m_gravi_object.from_pos.distance_to(m_gravi_object.target_pos))) {
		m_gravi_object.deactivate();
		return;
	}

	float dt = float(Level().timeServer() - m_gravi_object.time_last_update);
	float dist = dt * SPEED;
		
	if (dist < GRAVI_STEP) return;
	
	Fvector new_pos;
	Fvector dir;
	dir.sub(m_gravi_object.target_pos,m_gravi_object.cur_pos);
	dir.normalize();
	
	new_pos.mad(m_gravi_object.cur_pos,dir,dist);

	// Trace to enemy 
	Fvector enemy_center;
	m_gravi_object.enemy->Center(enemy_center);
	dir.sub(enemy_center, new_pos);
	dir.normalize();

	float trace_dist = GRAVI_STEP * 2;

	Collide::rq_result	l_rq;
	if (Level().ObjectSpace.RayPick(new_pos, dir, trace_dist, Collide::rqtBoth, l_rq)) {
		const CObject *enemy = dynamic_cast<const CObject *>(m_gravi_object.enemy);
		if ((l_rq.O == enemy) && (l_rq.range < trace_dist)) {
			
			// check for visibility
			bool b_enemy_visible = false;
			xr_vector<CObject *> visible_objects;
			feel_vision_get(visible_objects);

			// find object
			for (u32 i = 0; i<visible_objects.size(); i++) {
				if (visible_objects[i] == enemy) {
					b_enemy_visible = true;
					break;
				}
			}
			
			if (b_enemy_visible) {
				float	hit_power		= 50;
				float	hit_impulse		= 2000;
				Fvector impulse_dir;

				impulse_dir.set(0.0f,0.0f,1.0f);
				impulse_dir.normalize();

				HitEntity(m_gravi_object.enemy, hit_power, hit_impulse, impulse_dir);
				m_gravi_object.deactivate();
				return;
			}
		}
	}

	m_gravi_object.cur_pos				= new_pos;
	m_gravi_object.time_last_update		= Level().timeServer();

	// draw particle
	CParticlesObject* ps = xr_new<CParticlesObject>(particle_gravi_wave);

	// вычислить позицию и направленность партикла
	Fmatrix pos; 

	// установить направление
	pos.k.set(Fvector().set(0.0f,1.0f,0.0f));
	Fvector::generate_orthonormal_basis(pos.k, pos.i, pos.j);
	// установить позицию
	pos.c.set(m_gravi_object.cur_pos);

	ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
	Level().ps_needtoplay.push_back(ps);
	
	// hit objects
	Level().ObjectSpace.GetNearest(m_gravi_object.cur_pos, GRAVI_RADIUS); 
	xr_vector<CObject*> &tpObjects = Level().ObjectSpace.q_nearest;

	for (u32 i=0;i<tpObjects.size();i++) {
		CGameObject *obj = dynamic_cast<CGameObject *>(tpObjects[i]);
		if (!obj || !obj->m_pPhysicsShell) continue;
		
		Fvector dir;
		dir.sub(obj->Position(), m_gravi_object.cur_pos);
		dir.normalize();
		obj->m_pPhysicsShell->applyImpulse(dir,GRAVI_IMPULSE * obj->m_pPhysicsShell->getMass());
	}

	// играть звук

	Fvector snd_pos = m_gravi_object.cur_pos;
	snd_pos.y += 0.5f;
	if (sound_gravi_wave.feedback) {
		sound_gravi_wave.set_position(snd_pos);
	} else ::Sound->play_at_pos(sound_gravi_wave,0,snd_pos);
}

void CBurer::UpdateCL()
{
	inherited::UpdateCL();

	UpdateGraviObject();
}

void CBurer::StartGraviPrepare() 
{
	const CEntityAlive *enemy = EnemyMan.get_enemy();
	if (!enemy) return;
	
	CActor *pA = const_cast<CActor *>(dynamic_cast<const CActor*>(enemy));
	if (!pA) return;

	pA->CParticlesPlayer::StartParticles(particle_gravi_prepare,Fvector().set(0.0f,0.1f,0.0f),pA->ID());
}
void CBurer::StopGraviPrepare() 
{
	CActor *pA = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (!pA) return;

	pA->CParticlesPlayer::StopParticles(particle_gravi_prepare);
}

void CBurer::StartTeleObjectParticle(CGameObject *pO) 
{
	pO->CParticlesPlayer::StartParticles(particle_tele_object,Fvector().set(0.0f,0.1f,0.0f),pO->ID());
}
void CBurer::StopTeleObjectParticle(CGameObject *pO) 
{
	pO->CParticlesPlayer::StopParticles(particle_tele_object);
}


