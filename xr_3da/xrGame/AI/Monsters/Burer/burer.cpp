#include "stdafx.h"
#include "burer.h"
#include "../ai_monster_utils.h"
#include "../../../PhysicsShell.h"
#include "../../../actor.h"
#include "../ai_monster_debug.h"
#include "burer_state_manager.h"
#include "../../../../skeletonanimated.h"

CBurer::CBurer()
{
	StateMan = xr_new<CStateManagerBurer>(this);
	TScanner::init_external(this);
}

CBurer::~CBurer()
{
	xr_delete(StateMan);
}


void CBurer::reinit()
{
	inherited::reinit			();
	TScanner::reinit			();

	DeactivateShield			();

	time_last_scan				= 0;
}


void CBurer::reload(LPCSTR section)
{
	inherited::reload(section);

	CSoundPlayer::add(pSettings->r_string(section,"sound_gravi_attack"),	16,	SOUND_TYPE_MONSTER_ATTACKING,	2,	u32(1 << 31) | 16,	MonsterSpace::eMonsterSoundGraviAttack, "bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_tele_attack"),		16,	SOUND_TYPE_MONSTER_ATTACKING,	2,	u32(1 << 31) | 17,	MonsterSpace::eMonsterSoundTeleAttack, "bip01_head");

	CSkeletonAnimated *pSkel = smart_cast<CSkeletonAnimated*>(Visual());
	
	// Load triple gravi animations
	CMotionDef *def1, *def2, *def3;
	def1 = pSkel->ID_Cycle_Safe("stand_gravi_0");	VERIFY(def1);
	def2 = pSkel->ID_Cycle_Safe("stand_gravi_1");	VERIFY(def2);
	def3 = pSkel->ID_Cycle_Safe("stand_gravi_2");	VERIFY(def3);
	anim_triple_gravi.init_external	(def1, def2, def3);
	
	// Load triple tele animations
	def1 = pSkel->ID_Cycle_Safe("stand_tele_0");	VERIFY(def1);
	def2 = pSkel->ID_Cycle_Safe("stand_tele_1");	VERIFY(def2);
	def3 = pSkel->ID_Cycle_Safe("stand_tele_2");	VERIFY(def3);
	anim_triple_tele.init_external	(def1, def2, def3);

}

void CBurer::Load(LPCSTR section)
{
	inherited::Load				(section);
	TScanner::load				(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimStandIdle,		eAnimStandDamaged);	
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,			eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,		eAnimWalkDamaged);
	
	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);

	particle_gravi_wave		= pSettings->r_string(section,"Particle_Gravi_Wave");
	particle_gravi_prepare	= pSettings->r_string(section,"Particle_Gravi_Prepare");
	particle_tele_object	= pSettings->r_string(section,"Particle_Tele_Object");
	
	::Sound->create(sound_gravi_wave,	TRUE, pSettings->r_string(section,"sound_gravi_wave"),	SOUND_TYPE_WORLD);

	m_gravi_speed					= pSettings->r_u32(section,"Gravi_Speed");
	m_gravi_step					= pSettings->r_u32(section,"Gravi_Step");
	m_gravi_time_to_hold			= pSettings->r_u32(section,"Gravi_Time_To_Hold");
	m_gravi_radius					= pSettings->r_float(section,"Gravi_Radius");
	m_gravi_impulse_to_objects		= pSettings->r_float(section,"Gravi_Impulse_To_Objects");
	m_gravi_impulse_to_enemy		= pSettings->r_float(section,"Gravi_Impulse_To_Enemy");
	m_gravi_hit_power				= pSettings->r_float(section,"Gravi_Hit_Power");
	
	m_tele_max_handled_objects		= pSettings->r_u32(section,"Tele_Max_Handled_Objects");
	m_tele_time_to_hold				= pSettings->r_u32(section,"Tele_Time_To_Hold");
	m_tele_object_min_mass			= pSettings->r_float(section,"Tele_Object_Min_Mass");
	m_tele_object_max_mass			= pSettings->r_float(section,"Tele_Object_Max_Mass");
	m_tele_find_radius				= pSettings->r_float(section,"Tele_Find_Radius");

	particle_fire_shield			= pSettings->r_string(section,"Particle_Shield");
	
	if (MotionMan.start_load_shared(SUB_CLS_ID)) {

		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandDamaged,	"stand_idle_dmg_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,		PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,		PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimScared,			"stand_scared_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	

		MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimCheckCorpse,		"sit_check_corpse_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	
		MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND, 	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

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
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimScared);
		MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));
		//MotionMan.STEPS_Load(pSettings->r_string(section, "step_params"), get_legs_number());

		MotionMan.finish_load_shared();

	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

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

void CBurer::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	TTelekinesis::schedule_update();
	TScanner::schedule_update	 ();
}

void CBurer::CheckSpecParams(u32 spec_params)
{
}

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
	float dist = dt * float(m_gravi_speed)/1000.f;
		
	if (dist < m_gravi_step) return;
	
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

	float trace_dist = float(m_gravi_step);

	Collide::rq_result	l_rq;
	if (Level().ObjectSpace.RayPick(new_pos, dir, trace_dist, Collide::rqtBoth, l_rq)) {
		const CObject *enemy = smart_cast<const CObject *>(m_gravi_object.enemy);
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
				Fvector impulse_dir;

				impulse_dir.set(0.0f,0.0f,1.0f);
				impulse_dir.normalize();

				HitEntity(m_gravi_object.enemy, m_gravi_hit_power, m_gravi_impulse_to_enemy, impulse_dir);
				m_gravi_object.deactivate();
				return;
			}
		}
	}
																								
	m_gravi_object.cur_pos				= new_pos;
	m_gravi_object.time_last_update		= Level().timeServer();

	// ---------------------------------------------------------------------
	// draw particle
	CParticlesObject* ps = xr_new<CParticlesObject>(particle_gravi_wave);

	// вычислить позицию и направленность партикла
	Fmatrix pos; 
	pos.identity();
	pos.k.set(dir);
	Fvector::generate_orthonormal_basis_normalized(pos.k,pos.j,pos.i);
	// установить позицию
	pos.translate_over(m_gravi_object.cur_pos);

	ps->UpdateParent(pos, zero_vel);
	ps->Play();
	
	// hit objects
	Level().ObjectSpace.GetNearest(m_gravi_object.cur_pos, m_gravi_radius); 
	xr_vector<CObject*> &tpObjects = Level().ObjectSpace.q_nearest;

	for (u32 i=0;i<tpObjects.size();i++) {
		CPhysicsShellHolder  *obj = smart_cast<CPhysicsShellHolder *>(tpObjects[i]);
		if (!obj || !obj->m_pPhysicsShell) continue;
		
		Fvector dir;
		dir.sub(obj->Position(), m_gravi_object.cur_pos);
		dir.normalize();
		obj->m_pPhysicsShell->applyImpulse(dir,m_gravi_impulse_to_objects * obj->m_pPhysicsShell->getMass());
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
	TScanner::frame_update(Device.dwTimeDelta);

	UpdateGraviObject();
}

void CBurer::StartGraviPrepare() 
{
	const CEntityAlive *enemy = EnemyMan.get_enemy();
	if (!enemy) return;
	
	CActor *pA = const_cast<CActor *>(smart_cast<const CActor*>(enemy));
	if (!pA) return;

	pA->CParticlesPlayer::StartParticles(particle_gravi_prepare,Fvector().set(0.0f,0.1f,0.0f),pA->ID());
}
void CBurer::StopGraviPrepare() 
{
	CActor *pA = smart_cast<CActor*>(Level().CurrentEntity());
	if (!pA) return;

	pA->CParticlesPlayer::StopParticles(particle_gravi_prepare);
}

void CBurer::StartTeleObjectParticle(CGameObject *pO) 
{
	CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(pO);
	if(!PP) return;
	PP->StartParticles(particle_tele_object,Fvector().set(0.0f,0.1f,0.0f),pO->ID());
}
void CBurer::StopTeleObjectParticle(CGameObject *pO) 
{
	CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(pO);
	if(!PP) return;
	PP->StopParticles(particle_tele_object);
}

void CBurer::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	if (m_shield_active && (hit_type == ALife::eHitTypeFireWound) && (Device.dwFrame != last_hit_frame)) {

		// вычислить позицию и направленность партикла
		Fmatrix pos; 
		//CParticlesPlayer::MakeXFORM(this,element,Fvector().set(0.f,0.f,1.f),p_in_object_space,pos);
		CParticlesPlayer::MakeXFORM(this,element,dir,p_in_object_space,pos);

		// установить particles
		CParticlesObject* ps = xr_new<CParticlesObject>(particle_fire_shield);
		
		ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
		Level().ps_needtoplay.push_back(ps);

	} else if (!m_shield_active)
		inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);

	last_hit_frame = Device.dwFrame;
}


void CBurer::Die()
{
	inherited::Die( );

	MotionMan.TA_Deactivate();
	CTelekinesis::Deactivate();
}

void CBurer::on_scanning()
{
	time_last_scan = Level().timeServer();
}

void CBurer::on_scan_success()
{
	CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());
	if (!pA) return;

	EnemyMan.add_enemy(pA);
}
