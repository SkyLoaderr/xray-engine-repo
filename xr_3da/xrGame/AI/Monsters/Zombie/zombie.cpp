#include "stdafx.h"
#include "zombie.h"
#include "zombie_state_manager.h"
#include "../../../profiler.h"
#include "../../../../skeletonanimated.h"
#include "../ai_monster_movement.h"
#include "../../../entitycondition.h"
#include "../ai_monster_movement_space.h"

CZombie::CZombie()
{
	StateMan = xr_new<CStateManagerZombie>(this);
	
	CControlled::init_external(this);
}

CZombie::~CZombie()
{
	xr_delete		(StateMan);
}



void CZombie::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);

	fake_death_count		= 1 + u8(Random.randI(pSettings->r_u8(section,"FakeDeathCount")));
	health_death_threshold	= pSettings->r_float(section,"StartFakeDeathHealthThreshold");

	if (MotionMan.start_load_shared(CLS_ID)) {
		
		SVelocityParam velocity_none;
		SVelocityParam &velocity_turn		= movement().get_velocity(MonsterMovement::eVelocityParameterStand);
		SVelocityParam &velocity_walk		= movement().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
		SVelocityParam &velocity_run		= movement().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
		//SVelocityParam &velocity_walk_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
		//SVelocityParam &velocity_run_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
		//SVelocityParam &velocity_steal		= movement().get_velocity(MonsterMovement::eVelocityParameterSteal);
		//SVelocityParam &velocity_drag		= movement().get_velocity(MonsterMovement::eVelocityParameterDrag);


		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&velocity_run,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimDie,				"stand_die_",			0, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

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

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}

void CZombie::reinit()
{
	inherited::reinit();
	CControlled::reinit();

	Bones.Reset();
	
	time_dead_start			= 0;
	last_hit_frame			= 0;
	time_resurrect			= 0;
	last_health_fake_death	= 1.f;

	active_triple_idx		= u8(-1);
}

void CZombie::reload(LPCSTR section)
{
	inherited::reload(section);

	// Load triple death animations
	MotionID			def1, def2, def3;
	CSkeletonAnimated	*pSkel = smart_cast<CSkeletonAnimated*>(Visual());
	
	def1				= pSkel->ID_Cycle_Safe("fake_death_0_0");	VERIFY(def1);
	def2				= pSkel->ID_Cycle_Safe("fake_death_0_1");	VERIFY(def2);
	def3				= pSkel->ID_Cycle_Safe("fake_death_0_2");	VERIFY(def3);
	anim_triple_death[0].reinit_external	(&EventMan, def1, def2, def3);


	def1				= pSkel->ID_Cycle_Safe("fake_death_1_0");	VERIFY(def1);
	def2				= pSkel->ID_Cycle_Safe("fake_death_1_1");	VERIFY(def2);
	def3				= pSkel->ID_Cycle_Safe("fake_death_1_2");	VERIFY(def3);
	anim_triple_death[1].reinit_external	(&EventMan, def1, def2, def3);

	def1				= pSkel->ID_Cycle_Safe("fake_death_2_0");	VERIFY(def1);
	def2				= pSkel->ID_Cycle_Safe("fake_death_2_1");	VERIFY(def2);
	def3				= pSkel->ID_Cycle_Safe("fake_death_2_2");	VERIFY(def3);
	anim_triple_death[2].reinit_external	(&EventMan, def1, def2, def3);
}


void __stdcall CZombie::BoneCallback(CBoneInstance *B)
{
	CZombie*	this_class = static_cast<CZombie*>(B->Callback_Param);

	START_PROFILE("AI/Zombie/Bones Update");
	this_class->Bones.Update(B, Device.dwTimeGlobal);
	STOP_PROFILE("AI/Zombie/Bones Update");
}


void CZombie::vfAssignBones()
{
	// ��������� callback �� �����
	bone_spine =	&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_spine"));
	bone_head =		&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_head"));
	//bone_spine->set_callback(BoneCallback,this);
	//bone_head->set_callback(BoneCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(bone_spine, AXIS_Z);	Bones.AddBone(bone_spine, AXIS_Y); Bones.AddBone(bone_spine, AXIS_X);
	Bones.AddBone(bone_head, AXIS_Z);	Bones.AddBone(bone_head, AXIS_Y);
}

BOOL CZombie::net_Spawn (CSE_Abstract* DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones	();

	return(TRUE);
}

#define TIME_FAKE_DEATH			5000
#define TIME_RESURRECT_RESTORE	2000

void CZombie::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);

	if (!g_Alive()) return;
	
	if ((hit_type == ALife::eHitTypeFireWound) && (Device.dwFrame != last_hit_frame)) {
		if (!MotionMan.TA_IsActive() && (time_resurrect + TIME_RESURRECT_RESTORE < Device.dwTimeGlobal) && (conditions().GetHealth() < health_death_threshold)) {
			if (conditions().GetHealth() < last_health_fake_death) {
				
				if ((last_health_fake_death - conditions().GetHealth()) > (health_death_threshold / fake_death_count)) {
					
					active_triple_idx			= u8(Random.randI(FAKE_DEATH_TYPES_COUNT));
					MotionMan.TA_Activate		(&anim_triple_death[active_triple_idx]);
					movement().stop_now	();
					time_dead_start				= Device.dwTimeGlobal;

					last_health_fake_death		= conditions().GetHealth();
				}
			}
		}
	}

	last_hit_frame = Device.dwFrame;
}


void CZombie::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	if (time_dead_start != 0) {
		if (time_dead_start + TIME_FAKE_DEATH < Device.dwTimeGlobal) {
			time_dead_start  = 0;

			VERIFY(anim_triple_death[active_triple_idx].is_active());

			MotionMan.TA_PointBreak();	

			time_resurrect = Device.dwTimeGlobal;
		}
	}
}

void CZombie::Die(CObject* who)
{
	inherited::Die		(who);
	CControlled::on_die	();
}

void CZombie::net_Destroy()
{
	CControlled::on_destroy	();
	inherited::net_Destroy	();
}
