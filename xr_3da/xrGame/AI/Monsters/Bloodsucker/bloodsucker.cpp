#include "stdafx.h"
#include "bloodsucker.h"
#include "bloodsucker_effector.h"
#include "bloodsucker_state_manager.h"
#include "../../../../skeletoncustom.h"
#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "../../../../skeletonanimated.h"
#include "../../../level.h"
#include "../../../material_manager.h"
#include "bloodsucker_vampire_effector.h"
#include "../../../detail_path_manager.h"
#include "../ai_monster_movement.h"
#include "../ai_monster_utils.h"
#include "../../../level_debug.h"

CAI_Bloodsucker::CAI_Bloodsucker()
{
	StateMan			= xr_new<CStateManagerBloodsucker>	(this);

	invisible_vel.set	(0.1f, 0.1f);
}

CAI_Bloodsucker::~CAI_Bloodsucker()
{
	xr_delete	(StateMan);
}

void CAI_Bloodsucker::Load(LPCSTR section) 
{
	inherited::Load(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimStandIdle,	eAnimStandDamaged);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	invisible_vel.set(pSettings->r_float(section,"Velocity_Invisible_Linear"),pSettings->r_float(section,"Velocity_Invisible_Angular"));
	movement().detail().add_velocity(eVelocityParameterInvisible,CDetailPathManager::STravelParams(invisible_vel.linear, invisible_vel.angular));

	invisible_particle_name = pSettings->r_string(section,"Particle_Invisible");

	LoadVampirePPEffector			(pSettings->r_string(section,"vampire_effector"));
	
	if (MotionMan.start_load_shared(CLS_ID)) {

		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandDamaged,	"stand_damaged_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,	  	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		
		MotionMan.AddAnim(eAnimRunTurnLeft,		"stand_run_turn_left_",	-1, &inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRunTurnRight,	"stand_run_turn_right_",-1, &inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimScared,			"stand_scared_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	

		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		
		MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimAttackRun,		"stand_attack_run_",	-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSitToSleep,		"sit_sleep_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		
		MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimMiscAction_00,	"stand_to_aggressive_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	

		// define transitions
		//	MotionMan.AddTransition(PS_STAND,			eAnimThreaten,	eAnimMiscAction_00,	false);
		MotionMan.AddTransition(eAnimStandSitDown,	eAnimSleep,		eAnimSitToSleep,	false);
		MotionMan.AddTransition(PS_STAND,			eAnimSleep,		eAnimStandSitDown,	true);
		MotionMan.AddTransition(PS_STAND,			PS_SIT,			eAnimStandSitDown,	false);
		MotionMan.AddTransition(PS_STAND,			PS_LIE,			eAnimStandSitDown,	false);
		MotionMan.AddTransition(PS_SIT,				PS_STAND,		eAnimSitStandUp,	false);
		MotionMan.AddTransition(PS_LIE,				PS_STAND,		eAnimSitStandUp,	false);

		// define links from Action to animations
		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimWalkBkwd);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround); 

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();
	}

	#ifdef DEBUG	
		MotionMan.accel_chain_test		();
	#endif
}


void CAI_Bloodsucker::reinit()
{
	inherited::reinit			();
	CInvisibility::reinit		();
	CControlledActor::reinit	();

	Bones.Reset();


	// Load triple vampire animations
	MotionID			def1, def2, def3;
	CSkeletonAnimated	*skel_animated = smart_cast<CSkeletonAnimated*>(Visual());
	def1				= skel_animated->ID_Cycle_Safe("vampire_0");	VERIFY(def1);
	def2				= skel_animated->ID_Cycle_Safe("vampire_1");	VERIFY(def2);
	def3				= skel_animated->ID_Cycle_Safe("vampire_2");	VERIFY(def3);
	anim_triple_vampire.reinit_external	(&EventMan, def1, def2, def3, false);

	MotionMan.AddAnimTranslation	(def1,"vampire_0");
	MotionMan.AddAnimTranslation	(def2,"vampire_1");
	MotionMan.AddAnimTranslation	(def3,"vampire_2");
}

void CAI_Bloodsucker::reload(LPCSTR section)
{
	inherited::reload(section);
	CInvisibility::reload(section);
}


void CAI_Bloodsucker::LoadVampirePPEffector(LPCSTR section)
{
	pp_vampire_effector.duality.h			= pSettings->r_float(section,"duality_h");
	pp_vampire_effector.duality.v			= pSettings->r_float(section,"duality_v");
	pp_vampire_effector.gray				= pSettings->r_float(section,"gray");
	pp_vampire_effector.blur				= pSettings->r_float(section,"blur");
	pp_vampire_effector.noise.intensity		= pSettings->r_float(section,"noise_intensity");
	pp_vampire_effector.noise.grain			= pSettings->r_float(section,"noise_grain");
	pp_vampire_effector.noise.fps			= pSettings->r_float(section,"noise_fps");
	VERIFY(!fis_zero(pp_vampire_effector.noise.fps));

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &pp_vampire_effector.color_base.r, &pp_vampire_effector.color_base.g, &pp_vampire_effector.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &pp_vampire_effector.color_gray.r, &pp_vampire_effector.color_gray.g, &pp_vampire_effector.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &pp_vampire_effector.color_add.r,  &pp_vampire_effector.color_add.g,  &pp_vampire_effector.color_add.b);
}




void __stdcall CAI_Bloodsucker::BoneCallback(CBoneInstance *B)
{
	CAI_Bloodsucker*	this_class = static_cast<CAI_Bloodsucker*> (B->Callback_Param);

	this_class->Bones.Update(B, Device.dwTimeGlobal);
}


void CAI_Bloodsucker::vfAssignBones()
{
	// Установка callback на кости
	bone_spine =	&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_spine"));
	bone_head =		&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_head"));
	bone_spine->set_callback(BoneCallback,this);
	bone_head->set_callback(BoneCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(bone_spine, AXIS_X);	Bones.AddBone(bone_spine, AXIS_Y);
	Bones.AddBone(bone_head, AXIS_X);	Bones.AddBone(bone_head, AXIS_Y);
}


#define MAX_BONE_ANGLE PI_DIV_4

void CAI_Bloodsucker::LookDirection(Fvector to_dir, float bone_turn_speed)
{
	// получаем вектор направления к источнику звука и его мировые углы
	float		yaw,pitch;
	to_dir.getHP(yaw,pitch);

	// установить параметры вращения по yaw
	float cur_yaw = -movement().m_body.current.yaw;						// текущий мировой угол монстра
	float bone_angle;											// угол для боны	

	float dy = _abs(angle_normalize_signed(yaw - cur_yaw));		// дельта, на которую нужно поворачиваться

	if (angle_difference(cur_yaw,yaw) <= MAX_BONE_ANGLE) {		// bone turn only
		bone_angle = dy;
	} else {													// torso & bone turn 
		if (movement().IsMoveAlongPathFinished() || !movement().enabled()) movement().m_body.target.yaw = angle_normalize(-yaw);
		if (dy / 2 < MAX_BONE_ANGLE) bone_angle = dy / 2;
		else bone_angle = MAX_BONE_ANGLE;
	}

	bone_angle /= 2;
	if (from_right(yaw,cur_yaw)) bone_angle *= -1.f;

	Bones.SetMotion(bone_spine, AXIS_X, bone_angle, bone_turn_speed, 100);
	Bones.SetMotion(bone_head,	AXIS_X, bone_angle, bone_turn_speed, 100);

	// установить параметры вращения по pitch
	clamp(pitch, -MAX_BONE_ANGLE, MAX_BONE_ANGLE);
	pitch /= 2; 

	Bones.SetMotion(bone_spine, AXIS_Y, pitch, bone_turn_speed, 100);
	Bones.SetMotion(bone_head,	AXIS_Y, pitch, bone_turn_speed, 100);	
}

void CAI_Bloodsucker::LookPosition(Fvector to_point, float angular_speed)
{
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());
	LookDirection(dir,angular_speed);
}

void CAI_Bloodsucker::ActivateVampireEffector(float max_dist)
{
	CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());
	if (pA) {
		pA->EffectorManager().AddEffector(xr_new<CVampireCameraEffector>(6.0f, 5.f, max_dist));
		Level().Cameras.AddEffector(xr_new<CVampirePPEffector>(pp_vampire_effector, 6.0f));
	}
}


void CAI_Bloodsucker::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		MotionMan.SetCurAnim(eAnimThreaten);
		return;
	}

	if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
		MotionMan.SetCurAnim(eAnimLookAround);
		return;
	}

}

BOOL CAI_Bloodsucker::net_Spawn (CSE_Abstract* DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones	();

	// спаунится нивидимым
	setVisible		(false);

	return(TRUE);
}

void CAI_Bloodsucker::UpdateCL()
{
	inherited::UpdateCL				();
	CInvisibility::frame_update		();
	CControlledActor::frame_update	();
}


void CAI_Bloodsucker::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	
	if (!g_Alive())	setVisible(TRUE);
	CInvisibility::schedule_update();

	if (state_invisible && g_Alive() && (m_fCurSpeed != 0)) {
		SGameMtlPair* mtl_pair		= material().get_current_pair();
		if (!mtl_pair) return;

		if (mtl_pair->CollideParticles.empty()) return;

		LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];

		//отыграть партиклы столкновения материалов
		CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);

		// вычислить позицию и направленность партикла
		Fmatrix pos; 

		// установить направление
		pos.k.set(Fvector().set(0.0f,1.0f,0.0f));
		Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
		// установить позицию
		pos.c.set(CStepManager::get_foot_position(eFrontLeft));

		ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
		Level().ps_needtoplay.push_back(ps);
	}
}

void CAI_Bloodsucker::on_change_visibility(bool b_visibility)
{
	setVisible(b_visibility);
}

void CAI_Bloodsucker::on_activate()
{
	CInvisibility::on_activate();
	
	CParticlesPlayer::StartParticles(invisible_particle_name,Fvector().set(0.0f,0.1f,0.0f),ID());		
	state_invisible = true;

	//spatial.type |= STYPE_VISIBLEFORAI;

}

void CAI_Bloodsucker::on_deactivate()
{
	CInvisibility::on_deactivate();
	
	CParticlesPlayer::StartParticles(invisible_particle_name,Fvector().set(0.0f,0.1f,0.0f),ID());
	state_invisible = false;
	
	//spatial.type &= ~STYPE_VISIBLEFORAI;
}

void CAI_Bloodsucker::net_Destroy()
{
	CInvisibility::deactivate(); 
	CInvisibility::disable();
	inherited::net_Destroy();
}

void CAI_Bloodsucker::Die(CObject* who)
{
	CInvisibility::deactivate(); 
	CInvisibility::disable();

	inherited::Die(who);
}

#ifdef DEBUG
CBaseMonster::SDebugInfo CAI_Bloodsucker::show_debug_info()
{
	CBaseMonster::SDebugInfo info = inherited::show_debug_info();
	if (!info.active) return CBaseMonster::SDebugInfo();

	string128 text;
	sprintf(text, "Invisibility Value = [%f]", CInvisibility::get_value());
	DBG().text(this).add_item(text, info.x, info.y+=info.delta_y, info.color);
	DBG().text(this).add_item("---------------------------------------", info.x, info.y+=info.delta_y, info.delimiter_color);

	return CBaseMonster::SDebugInfo();
}
#endif
