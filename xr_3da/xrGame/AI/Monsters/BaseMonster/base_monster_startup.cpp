#include "stdafx.h"
#include "base_monster.h"
#include "../../../CharacterPhysicsSupport.h"
#include "../../../phmovementcontrol.h"
#include "../ai_monster_squad_manager.h"
#include "../../../../skeletonanimated.h"
#include "../critical_action_info.h"
#include "../ai_monster_jump.h"
#include "../../../detail_path_manager.h"
#include "../../../level_graph.h"
#include "../ai_monster_movement.h"

void CBaseMonster::reload	(LPCSTR section)
{
	if (!frame_check(m_dwFrameReload))
		return;

	CCustomMonster::reload		(section);
	movement().reload	(section);

	CSoundPlayer::add(pSettings->r_string(section,"sound_idle"),				16,		SOUND_TYPE_MONSTER_TALKING,		7,	u32(1 << 31) | 3,	MonsterSpace::eMonsterSoundIdle, 		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_eat"),					16,		SOUND_TYPE_MONSTER_TALKING,		6,	u32(1 << 31) | 2,	MonsterSpace::eMonsterSoundEat,			"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_attack"),				16,		SOUND_TYPE_MONSTER_ATTACKING,	5,	u32(1 << 31) | 1,	MonsterSpace::eMonsterSoundAttack,		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_attack_hit"),			16,		SOUND_TYPE_MONSTER_ATTACKING,	2,	u32(-1),			MonsterSpace::eMonsterSoundAttackHit,	"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_take_damage"),			16,		SOUND_TYPE_MONSTER_INJURING,	1,	u32(-1),			MonsterSpace::eMonsterSoundTakeDamage,	"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_die"),					16,		SOUND_TYPE_MONSTER_DYING,		0,	u32(-1),			MonsterSpace::eMonsterSoundDie,			"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_die_in_anomaly"),		16,		SOUND_TYPE_MONSTER_DYING,		0,	u32(-1),			MonsterSpace::eMonsterSoundDieInAnomaly,"bip01_head");	
	CSoundPlayer::add(pSettings->r_string(section,"sound_threaten"),			16,		SOUND_TYPE_MONSTER_ATTACKING,	3,	u32(1 << 31) | 0,	MonsterSpace::eMonsterSoundThreaten,	"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_landing"),				16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 1,	MonsterSpace::eMonsterSoundLanding,		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_steal"),				16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 5,	MonsterSpace::eMonsterSoundSteal,		"bip01_head");	
	CSoundPlayer::add(pSettings->r_string(section,"sound_panic"),				16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 6,	MonsterSpace::eMonsterSoundPanic,		"bip01_head");	
	CSoundPlayer::add(pSettings->r_string(section,"sound_growling"),			16,		SOUND_TYPE_MONSTER_STEP,		5,	u32(1 << 31) | 7,	MonsterSpace::eMonsterSoundGrowling,	"bip01_head");	
}

void CBaseMonster::reinit()
{
	if (!frame_check(m_dwFrameReinit))
		return;

	inherited::reinit					();
	movement().reinit			();
	CStepManager::reinit				();

	MotionMan.reinit					();

	EnemyMemory.clear					();
	SoundMemory.clear					();
	CorpseMemory.clear					();
	HitMemory.clear						();

	EnemyMan.reinit						();
	CorpseMan.reinit					();

	DirMan.reinit						();
	
	StateMan->reinit					();
	CriticalActionInfo->reinit			();
	
	Morale.reinit						();


	flagEatNow						= false;
	m_bDamaged						= false;
	m_bAngry						= false;
	m_bAggressive					= false;
	m_bSleep						= false;

	state_invisible					= false;
	m_default_bone_part				= smart_cast<CSkeletonAnimated*>(Visual())->LL_PartID("default");

	b_velocity_reset				= false;

	force_real_speed				= false;
	script_processing_active		= false;

	m_first_update_initialized		= false;
}

void CBaseMonster::Load(LPCSTR section)
{
	if (!frame_check(m_dwFrameLoad))
		return;

	// load parameters from ".ltx" file
	inherited::Load					(section);
	movement().Load			(section);
	CStepManager::load				(section);

	MeleeChecker.load				(section);
	Morale.load						(section);

	m_pPhysics_support				->in_Load(section);

	m_dwHealth						= pSettings->r_u32		(section,"Health");
	fEntityHealth					= (float)m_dwHealth;

	inherited_shared::load_shared	(SUB_CLS_ID, section);

	if (ability_can_jump())
		m_jumping					= smart_cast<CJumping *>(this);
}

void CBaseMonster::load_shared(LPCSTR section)
{
	// �������� ���������� �� LTX
	get_sd()->m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	get_sd()->m_fsVelocityStandTurn.Load		(section,"Velocity_Stand");
	get_sd()->m_fsVelocityWalkFwdNormal.Load	(section,"Velocity_WalkFwdNormal");
	get_sd()->m_fsVelocityWalkFwdDamaged.Load	(section,"Velocity_WalkFwdDamaged");
	get_sd()->m_fsVelocityRunFwdNormal.Load		(section,"Velocity_RunFwdNormal");
	get_sd()->m_fsVelocityRunFwdDamaged.Load	(section,"Velocity_RunFwdDamaged");
	get_sd()->m_fsVelocityDrag.Load				(section,"Velocity_Drag");
	get_sd()->m_fsVelocitySteal.Load			(section,"Velocity_Steal");

	if (ability_run_attack()) {
		get_sd()->m_fsVelocityRunAttack.Load	(section,"Velocity_RunAttack");
		
		get_sd()->m_run_attack_path_dist		= pSettings->r_float(section, "RunAttack_PathDistance");
		get_sd()->m_run_attack_start_dist		= pSettings->r_float(section, "RunAttack_StartDistance");
	}

	get_sd()->m_dwDayTimeBegin				= pSettings->r_u32	(section,"DayTime_Begin");
	get_sd()->m_dwDayTimeEnd				= pSettings->r_u32	(section,"DayTime_End");		
	get_sd()->m_fMinSatiety					= pSettings->r_float(section,"Min_Satiety");
	get_sd()->m_fMaxSatiety					= pSettings->r_float(section,"Max_Satiety");

	get_sd()->m_fDistToCorpse				= pSettings->r_float(section,"distance_to_corpse");
	get_sd()->m_fDamagedThreshold			= pSettings->r_float(section,"DamagedThreshold");

	get_sd()->m_dwIdleSndDelay				= pSettings->r_u32	(section,"idle_sound_delay");
	get_sd()->m_dwEatSndDelay				= pSettings->r_u32	(section,"eat_sound_delay");
	get_sd()->m_dwAttackSndDelay			= pSettings->r_u32	(section,"attack_sound_delay");

	get_sd()->m_fEatFreq					= pSettings->r_float(section,"eat_freq");
	get_sd()->m_fEatSlice					= pSettings->r_float(section,"eat_slice");
	get_sd()->m_fEatSliceWeight				= pSettings->r_float(section,"eat_slice_weight");

	get_sd()->m_legs_number					= pSettings->r_u8(section, "LegsCount");
	get_sd()->m_max_hear_dist				= pSettings->r_float(section, "max_hear_dist");

	// Load attack postprocess --------------------------------------------------------
	LPCSTR ppi_section = pSettings->r_string(section, "attack_effector");
	get_sd()->m_attack_effector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	get_sd()->m_attack_effector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	get_sd()->m_attack_effector.ppi.gray			= pSettings->r_float(ppi_section,"gray");
	get_sd()->m_attack_effector.ppi.blur			= pSettings->r_float(ppi_section,"blur");
	get_sd()->m_attack_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	get_sd()->m_attack_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	get_sd()->m_attack_effector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(get_sd()->m_attack_effector.ppi.noise.fps));
	

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &get_sd()->m_attack_effector.ppi.color_base.r, &get_sd()->m_attack_effector.ppi.color_base.g, &get_sd()->m_attack_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &get_sd()->m_attack_effector.ppi.color_gray.r, &get_sd()->m_attack_effector.ppi.color_gray.g, &get_sd()->m_attack_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &get_sd()->m_attack_effector.ppi.color_add.r,	&get_sd()->m_attack_effector.ppi.color_add.g,&get_sd()->m_attack_effector.ppi.color_add.b);

	get_sd()->m_attack_effector.time			= pSettings->r_float(ppi_section,"time");
	get_sd()->m_attack_effector.time_attack		= pSettings->r_float(ppi_section,"time_attack");
	get_sd()->m_attack_effector.time_release	= pSettings->r_float(ppi_section,"time_release");

	get_sd()->m_attack_effector.ce_time				= pSettings->r_float(ppi_section,"ce_time");
	get_sd()->m_attack_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	get_sd()->m_attack_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	get_sd()->m_attack_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	// --------------------------------------------------------------------------------

}


BOOL CBaseMonster::net_Spawn (LPVOID DC) 
{
	if (!frame_check(m_dwFrameSpawn))
		return	(TRUE);

	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract						*e	= (CSE_Abstract*)(DC);
	m_pPhysics_support->in_NetSpawn		(e);

	R_ASSERT2							(ai().get_level_graph() && ai().get_cross_table() && (ai().level_graph().level_id() != u32(-1)),"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!");

	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);

	movement().detail_path_manager().add_velocity(eVelocityParameterStand,			CDetailPathManager::STravelParams(get_sd()->m_fsVelocityStandTurn.velocity.linear,		get_sd()->m_fsVelocityStandTurn.velocity.angular_path,		get_sd()->m_fsVelocityStandTurn.velocity.angular_real));
	movement().detail_path_manager().add_velocity(eVelocityParameterWalkNormal,	CDetailPathManager::STravelParams(get_sd()->m_fsVelocityWalkFwdNormal.velocity.linear,	get_sd()->m_fsVelocityWalkFwdNormal.velocity.angular_path,	get_sd()->m_fsVelocityWalkFwdNormal.velocity.angular_real));
	movement().detail_path_manager().add_velocity(eVelocityParameterRunNormal,		CDetailPathManager::STravelParams(get_sd()->m_fsVelocityRunFwdNormal.velocity.linear,	get_sd()->m_fsVelocityRunFwdNormal.velocity.angular_path,	get_sd()->m_fsVelocityRunFwdNormal.velocity.angular_real));
	movement().detail_path_manager().add_velocity(eVelocityParameterWalkDamaged,	CDetailPathManager::STravelParams(get_sd()->m_fsVelocityWalkFwdDamaged.velocity.linear,	get_sd()->m_fsVelocityWalkFwdDamaged.velocity.angular_path,	get_sd()->m_fsVelocityWalkFwdDamaged.velocity.angular_real));
	movement().detail_path_manager().add_velocity(eVelocityParameterRunDamaged,	CDetailPathManager::STravelParams(get_sd()->m_fsVelocityRunFwdDamaged.velocity.linear,	get_sd()->m_fsVelocityRunFwdDamaged.velocity.angular_path,	get_sd()->m_fsVelocityRunFwdDamaged.velocity.angular_real));
	movement().detail_path_manager().add_velocity(eVelocityParameterSteal,			CDetailPathManager::STravelParams(get_sd()->m_fsVelocitySteal.velocity.linear,			get_sd()->m_fsVelocitySteal.velocity.angular_path,			get_sd()->m_fsVelocitySteal.velocity.angular_real));
	movement().detail_path_manager().add_velocity(eVelocityParameterDrag,			CDetailPathManager::STravelParams(-get_sd()->m_fsVelocityDrag.velocity.linear,			get_sd()->m_fsVelocityDrag.velocity.angular_path,			get_sd()->m_fsVelocityDrag.velocity.angular_real));
	movement().detail_path_manager().add_velocity(eVelocityParameterRunAttack,		CDetailPathManager::STravelParams(get_sd()->m_fsVelocityRunAttack.velocity.linear,		get_sd()->m_fsVelocityRunAttack.velocity.angular_path,		get_sd()->m_fsVelocityRunAttack.velocity.angular_real));
	
	monster_squad().register_member((u8)g_Team(),(u8)g_Squad(), this);

	return(TRUE);
}

void CBaseMonster::net_Destroy()
{
	if (!frame_check(m_dwFrameDestroy)) return;

	inherited::net_Destroy				();
	m_pPhysics_support->in_NetDestroy	();

	monster_squad().remove_member((u8)g_Team(),(u8)g_Squad(), this);
}
