#include "stdafx.h"
#include "zombie.h"
#include "zombie_state_manager.h"
#include "../../../profiler.h"

CZombie::CZombie()
{
	StateMan = xr_new<CStateManagerZombie>(this);
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

	if (!MotionMan.start_load_shared(SUB_CLS_ID)) return;

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			0, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

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

	MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

	MotionMan.finish_load_shared();
}

bool CZombie::UpdateStateManager()
{
	StateMan->execute	();

	if (!Bones.IsActive()) {
		
		float look_k = (Random.randI(2) ? (-1.f) : (1.f));
		
		Bones.SetMotion(bone_spine, AXIS_Z, 2 * look_k * PI_DIV_6/2,	2 * PI_DIV_6 / 2,	1);		
		Bones.SetMotion(bone_spine, AXIS_Y, 2 * look_k * PI_DIV_6,		2 * PI_DIV_6,		1);
	}
	
	return true;
}

void CZombie::reinit()
{
	inherited::reinit();
	Bones.Reset();
}

void __stdcall CZombie::BoneCallback(CBoneInstance *B)
{
	CZombie*	this_class = dynamic_cast<CZombie*> (static_cast<CObject*>(B->Callback_Param));

	START_PROFILE("AI/Zombie/Bones Update");
	this_class->Bones.Update(B, Level().timeServer());
	STOP_PROFILE("AI/Zombie/Bones Update");
}


void CZombie::vfAssignBones()
{
	// Установка callback на кости
	bone_spine =	&PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_spine"));
	bone_head =		&PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_head"));
	bone_spine->set_callback(BoneCallback,this);
	bone_head->set_callback(BoneCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(bone_spine, AXIS_Z);	Bones.AddBone(bone_spine, AXIS_Y);
	Bones.AddBone(bone_head, AXIS_Z);	Bones.AddBone(bone_head, AXIS_Y);
}

BOOL CZombie::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones	();

	return(TRUE);
}
