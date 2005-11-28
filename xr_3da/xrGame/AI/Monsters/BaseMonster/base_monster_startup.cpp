#include "stdafx.h"
#include "base_monster.h"
#include "../../../ai_space.h"
#include "../../../hit.h"
#include "../../../PHDestroyable.h"
#include "../../../CharacterPhysicsSupport.h"
#include "../../../phmovementcontrol.h"
#include "../ai_monster_squad_manager.h"
#include "../../../../skeletonanimated.h"
#include "../../../detail_path_manager.h"
#include "../../../level_graph.h"
#include "../corpse_cover.h"
#include "../../../cover_evaluators.h"
#include "../../../sound_player.h"

#include "../state_manager.h"
#include "../controlled_entity.h"
#include "../anomaly_detector.h"
#include "../monster_cover_manager.h"
#include "../monster_home.h"

void CBaseMonster::Load(LPCSTR section)
{
	// load parameters from ".ltx" file
	inherited::Load					(section);

	m_corpse_cover_evaluator		= xr_new<CMonsterCorpseCoverEvaluator>	(&movement().restrictions());
	m_enemy_cover_evaluator			= xr_new<CCoverEvaluatorFarFromEnemy>	(&movement().restrictions());
	m_cover_evaluator_close_point	= xr_new<CCoverEvaluatorCloseToEnemy>	(&movement().restrictions());

	movement().Load					(section);

	MeleeChecker.load				(section);
	Morale.load						(section);

	m_pPhysics_support				->in_Load(section);

	SetfHealth						( (float)pSettings->r_u32		(section,"Health"));

	m_controlled					= smart_cast<CControlledEntityBase*>(this);

	settings_load					(section);

	control().load					(section);

	m_anomaly_detector->load		(section);
	CoverMan->load					();

	m_rank							= (pSettings->line_exist(section,"rank")) ? int(pSettings->r_u8(section,"rank")) : 0;
}

// if sound is absent just do not load that one
#define LOAD_SOUND(sound_name,_count,_type,_prior,_mask,_int_type)		\
	if (pSettings->line_exist(section,sound_name))						\
		sound().add(pSettings->r_string(section,sound_name), _count,_type,_prior,_mask,_int_type,"bip01_head");

void CBaseMonster::reload	(LPCSTR section)
{
	CCustomMonster::reload		(section);
	CStepManager::reload		(section);
	movement().reload	(section);

	// load base sounds
	LOAD_SOUND("sound_idle",			16,		SOUND_TYPE_MONSTER_TALKING,		7,	u32(1 << 31) | 3,	MonsterSpace::eMonsterSoundIdle);
	LOAD_SOUND("sound_eat",				16,		SOUND_TYPE_MONSTER_TALKING,		6,	u32(1 << 31) | 2,	MonsterSpace::eMonsterSoundEat);
	LOAD_SOUND("sound_aggressive",			16,		SOUND_TYPE_MONSTER_ATTACKING,	5,	u32(1 << 31) | 1,	MonsterSpace::eMonsterSoundAttack);
	LOAD_SOUND("sound_attack_hit",		16,		SOUND_TYPE_MONSTER_ATTACKING,	2,	u32(-1),			MonsterSpace::eMonsterSoundAttackHit);
	LOAD_SOUND("sound_take_damage",		16,		SOUND_TYPE_MONSTER_INJURING,	1,	u32(-1),			MonsterSpace::eMonsterSoundTakeDamage);
	LOAD_SOUND("sound_die",				16,		SOUND_TYPE_MONSTER_DYING,		0,	u32(-1),			MonsterSpace::eMonsterSoundDie);
	LOAD_SOUND("sound_die_in_anomaly",	16,		SOUND_TYPE_MONSTER_DYING,		0,	u32(-1),			MonsterSpace::eMonsterSoundDieInAnomaly);	
	LOAD_SOUND("sound_threaten",		16,		SOUND_TYPE_MONSTER_ATTACKING,	3,	u32(1 << 31) | 0,	MonsterSpace::eMonsterSoundThreaten);
	LOAD_SOUND("sound_steal",			16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 5,	MonsterSpace::eMonsterSoundSteal);	
	LOAD_SOUND("sound_panic",			16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 6,	MonsterSpace::eMonsterSoundPanic);	
	
	control().reload					(section);

	// load monster type
	m_monster_type = eMonsterTypeUniversal;
	if (pSettings->line_exist(section,"monster_type")) {
		if (xr_strcmp(pSettings->r_string(section,"monster_type"), "indoor") == 0)
			m_monster_type = eMonsterTypeIndoor;
		else if (xr_strcmp(pSettings->r_string(section,"monster_type"), "outdoor") == 0)
			m_monster_type = eMonsterTypeOutdoor;
	}

	Home->load						("home");
}

void CBaseMonster::reinit()
{
	inherited::reinit					();

	EnemyMemory.clear					();
	SoundMemory.clear					();
	CorpseMemory.clear					();
	HitMemory.clear						();

	EnemyMan.reinit						();
	CorpseMan.reinit					();

	StateMan->reinit					();
	
	Morale.reinit						();

	m_bDamaged						= false;
	m_bAngry						= false;
	m_bAggressive					= false;
	m_bSleep						= false;
	m_bRunTurnLeft					= false;
	m_bRunTurnRight					= false;

	state_invisible					= false;

	m_force_real_speed				= false;
	m_script_processing_active		= false;

	m_first_update_initialized		= false;

	if (m_controlled)				m_controlled->on_reinit();

	ignore_collision_hit			= false;

	control().reinit				();

	m_anomaly_detector->reinit		();

	m_skip_transfer_enemy			= false;

#ifdef DEBUG
	m_show_debug_info				= 0;
#endif 

}


BOOL CBaseMonster::net_Spawn (CSE_Abstract* DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract							*e	= (CSE_Abstract*)(DC);
	m_pPhysics_support->in_NetSpawn			(e);

	R_ASSERT2								(ai().get_level_graph() && ai().get_cross_table() && (ai().level_graph().level_id() != u32(-1)),"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!");

	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);

	monster_squad().register_member			((u8)g_Team(),(u8)g_Squad(), this);

	settings_overrides						();

	return(TRUE);
}

void CBaseMonster::net_Destroy()
{
	// ������� ������� ���� ������� ����� inherited
	if (m_controlled) m_controlled->on_destroy	();
	if (StateMan) StateMan->critical_finalize	();

	inherited::net_Destroy				();
	m_pPhysics_support->in_NetDestroy	();

	monster_squad().remove_member((u8)g_Team(),(u8)g_Squad(), this);

#ifdef DEBUG
	m_show_debug_info				= 0;
#endif 

}

#define READ_SETTINGS(var,name,method,ltx,section) {\
	if (ltx == pSettings) var = ltx->method(section,name); \
	else if (ltx->line_exist(section,name)) var = ltx->method(section,name);\
}

void CBaseMonster::settings_read(CInifile *ini, LPCSTR section, SMonsterSettings &data)
{
	READ_SETTINGS(data.m_fSoundThreshold, "SoundThreshold", r_float, ini, section);

	if (ability_run_attack()) {
		READ_SETTINGS(data.m_run_attack_path_dist,	"RunAttack_PathDistance",	r_float, ini, section);
		READ_SETTINGS(data.m_run_attack_start_dist, "RunAttack_StartDistance",	r_float, ini, section);
	}

	READ_SETTINGS(data.m_dwDayTimeBegin,	"DayTime_Begin",		r_u32, ini, section);
	READ_SETTINGS(data.m_dwDayTimeEnd,		"DayTime_End",			r_u32, ini, section);

	READ_SETTINGS(data.m_fMinSatiety,		"Min_Satiety",			r_float, ini, section);
	READ_SETTINGS(data.m_fMaxSatiety,		"Max_Satiety",			r_float, ini, section);
	READ_SETTINGS(data.m_fDistToCorpse,		"distance_to_corpse",	r_float, ini, section);
	READ_SETTINGS(data.m_fDamagedThreshold, "DamagedThreshold",		r_float, ini, section);

	READ_SETTINGS(data.m_dwIdleSndDelay,	"idle_sound_delay",		r_u32, ini, section);
	READ_SETTINGS(data.m_dwEatSndDelay,		"eat_sound_delay",		r_u32, ini, section);
	READ_SETTINGS(data.m_dwAttackSndDelay,	"attack_sound_delay",	r_u32, ini, section);

	READ_SETTINGS(data.m_fEatFreq,			"eat_freq",				r_float, ini, section);
	READ_SETTINGS(data.m_fEatSlice,			"eat_slice",			r_float, ini, section);
	READ_SETTINGS(data.m_fEatSliceWeight,	"eat_slice_weight",		r_float, ini, section);

	READ_SETTINGS(data.m_legs_number,		"LegsCount",			r_u8,	 ini, section);
	READ_SETTINGS(data.m_max_hear_dist,		"max_hear_dist",		r_float, ini, section);


	// Load attack postprocess 
	if (ini->line_exist(section,"attack_effector")) {

		LPCSTR ppi_section = ini->r_string(section, "attack_effector");

		READ_SETTINGS(data.m_attack_effector.ppi.duality.h,			"duality_h",		r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.ppi.duality.v,			"duality_v",		r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.ppi.gray,				"gray",				r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.ppi.blur,				"blur",				r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.ppi.noise.intensity,	"noise_intensity",	r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.ppi.noise.grain,		"noise_grain",		r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.ppi.noise.fps,			"noise_fps",		r_float, ini, ppi_section);

		VERIFY(!fis_zero(data.m_attack_effector.ppi.noise.fps));

		if (ini->line_exist(ppi_section,"color_base")) 
			sscanf(ini->r_string(ppi_section,"color_base"),	"%f,%f,%f", &data.m_attack_effector.ppi.color_base.r, &data.m_attack_effector.ppi.color_base.g, &data.m_attack_effector.ppi.color_base.b);		
		if (ini->line_exist(ppi_section,"color_base")) 
			sscanf(ini->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &data.m_attack_effector.ppi.color_gray.r, &data.m_attack_effector.ppi.color_gray.g, &data.m_attack_effector.ppi.color_gray.b);
		if (ini->line_exist(ppi_section,"color_base")) 
			sscanf(ini->r_string(ppi_section,"color_add"),	"%f,%f,%f", &data.m_attack_effector.ppi.color_add.r,  &data.m_attack_effector.ppi.color_add.g,	&data.m_attack_effector.ppi.color_add.b);

		READ_SETTINGS(data.m_attack_effector.time,					"time",				r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.time_attack,			"time_attack",		r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.time_release,			"time_release",		r_float, ini, ppi_section);

		READ_SETTINGS(data.m_attack_effector.ce_time,				"ce_time",			r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.ce_amplitude,			"ce_amplitude",		r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.ce_period_number,		"ce_period_number",	r_float, ini, ppi_section);
		READ_SETTINGS(data.m_attack_effector.ce_power,				"ce_power",			r_float, ini, ppi_section);
	}
}

void CBaseMonster::settings_load(LPCSTR section)
{
	SMonsterSettings		data;

	settings_read			(pSettings, section, data);

	u32 crc					= crc32(&data,sizeof(SMonsterSettings));
	m_base_settings.create	(crc,1,&data);
}


void CBaseMonster::settings_overrides()
{
	SMonsterSettings			*data;
	data						= *m_base_settings;

	if (spawn_ini() && spawn_ini()->section_exist("settings_overrides")) {
		settings_read			(spawn_ini(),"settings_overrides", (*data));
	}

	u32 crc						= crc32(data,sizeof(SMonsterSettings));
	m_current_settings.create	(crc,1,data);
}


