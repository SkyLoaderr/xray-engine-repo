
#include "stdafx.h"
#include "ai_biting.h"
#include "../../PhysicsShell.h"
#include "../../CharacterPhysicsSupport.h"
#include "../../game_level_cross_table.h"
#include "../../game_graph.h"
#include "../../phmovementcontrol.h"
#include "../../xrserver_objects_alife_monsters.h"
#include "../ai_monster_jump.h"
#include "../ai_monster_utils.h"
#include "../ai_monster_debug.h"
#include "../ai_monster_squad_manager.h"
#include "../corpse_cover.h"
#include "../../cover_evaluators.h"
#include "../../seniority_hierarchy_holder.h"
#include "../../team_hierarchy_holder.h"
#include "../../squad_hierarchy_holder.h"
#include "../../group_hierarchy_holder.h"

CAI_Biting::CAI_Biting()
{
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);

#ifdef DEBUG
	HDebug							= xr_new<CMonsterDebug>(this, Fvector().set(0.0f,2.0f,0.0f), 20.f);
#endif
	
	m_pPhysics_support				->in_Init();

	m_dwFrameLoad		= u32(-1);
	m_dwFrameReload		= u32(-1);
	m_dwFrameReinit		= u32(-1);
	m_dwFrameSpawn		= u32(-1);
	m_dwFrameDestroy	= u32(-1);
	m_dwFrameClient		= u32(-1);

	// Components external init 

	CMonsterMovement::InitExternal	(this);
	EnemyMemory.init_external		(this, 20000);
	SoundMemory.init_external		(this, 20000);
	CorpseMemory.init_external		(this, 20000);
	HitMemory.init_external			(this, 50000);

	EnemyMan.init_external			(this);
	CorpseMan.init_external			(this);

	// »нициализаци€ параметров анимации	
	MotionMan.Init					(this);

	for (u32 i = 0; i < eLegsMaxNumber; i++) m_FootBones[i] = BI_NONE;

	m_fGoingSpeed					= 0.f;

	// Attack-stops init
	AS_Init							();

	m_corpse_cover_evaluator		= xr_new<CMonsterCorpseCoverEvaluator>(this);
	m_enemy_cover_evaluator			= xr_new<CCoverEvaluatorFarFromEnemy>(this);
	m_cover_evaluator_close_point	= xr_new<CCoverEvaluatorCloseToEnemy>(this);

	CurrentState					= 0;
}

CAI_Biting::~CAI_Biting()
{
	xr_delete(m_pPhysics_support);
#ifdef DEBUG
	xr_delete(HDebug);
#endif	

	xr_delete(m_corpse_cover_evaluator);
	xr_delete(m_enemy_cover_evaluator);
	xr_delete(m_cover_evaluator_close_point);
}

void CAI_Biting::reinit()
{
	if (!frame_check(m_dwFrameReinit))
		return;
	
	
	inherited::reinit					();
	CMonsterMovement::reinit			();
	
	MotionMan.reinit					();
	
	EnemyMemory.clear					();
	SoundMemory.clear					();
	CorpseMemory.clear					();
	HitMemory.clear						();

	EnemyMan.reinit						();
	CorpseMan.reinit					();

	flagEatNow						= false;
	m_bDamaged						= false;
	m_bAngry						= false;
	m_bAggressive					= false;

	state_invisible					= false;

	bone_part						= PSkeletonAnimated(Visual())->LL_PartID("default");

	b_velocity_reset				= false;

#ifdef 	DEEP_TEST_SPEED	
	time_next_update				= 0;
#endif

	prev_size						= 0;
	force_real_speed				= false;
	script_processing_active		= false;
}

void CAI_Biting::Load(LPCSTR section)
{
	if (!frame_check(m_dwFrameLoad))
		return;

	// load parameters from ".ltx" file
	inherited::Load					(section);
	CMonsterMovement::Load			(section);
	
	AS_Load							(section);

	m_pPhysics_support				->in_Load(section);
	
	m_tSelectorApproach->Load		(section,"selector_approach");

	m_fGoingSpeed					= pSettings->r_float	(section, "going_speed");
	m_dwHealth						= pSettings->r_u32		(section,"Health");

	fEntityHealth					= (float)m_dwHealth;

	inherited_shared::load_shared	(SUB_CLS_ID, section);

	m_fCurMinAttackDist				= get_sd()->m_fMinAttackDist;
}

void CAI_Biting::load_shared(LPCSTR section)
{
	// «агрузка параметров из LTX
	get_sd()->m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	get_sd()->m_fsVelocityStandTurn.Load		(section,"Velocity_Stand");
	get_sd()->m_fsVelocityWalkFwdNormal.Load (section,"Velocity_WalkFwdNormal");
	get_sd()->m_fsVelocityWalkFwdDamaged.Load(section,"Velocity_WalkFwdDamaged");
	get_sd()->m_fsVelocityRunFwdNormal.Load	(section,"Velocity_RunFwdNormal");
	get_sd()->m_fsVelocityRunFwdDamaged.Load (section,"Velocity_RunFwdDamaged");
	get_sd()->m_fsVelocityDrag.Load			(section,"Velocity_Drag");
	get_sd()->m_fsVelocitySteal.Load			(section,"Velocity_Steal");

	get_sd()->m_dwDayTimeBegin				= pSettings->r_u32	(section,"DayTime_Begin");
	get_sd()->m_dwDayTimeEnd					= pSettings->r_u32	(section,"DayTime_End");		
	get_sd()->m_fMinSatiety					= pSettings->r_float(section,"Min_Satiety");
	get_sd()->m_fMaxSatiety					= pSettings->r_float(section,"Max_Satiety");

	get_sd()->m_fDistToCorpse				= pSettings->r_float(section,"distance_to_corpse");
	get_sd()->m_fMinAttackDist				= pSettings->r_float(section,"MinAttackDist");
	get_sd()->m_fMaxAttackDist				= pSettings->r_float(section,"MaxAttackDist");

	get_sd()->m_fDamagedThreshold			= pSettings->r_float(section,"DamagedThreshold");

	get_sd()->m_dwIdleSndDelay				= pSettings->r_u32	(section,"idle_sound_delay");
	get_sd()->m_dwEatSndDelay				= pSettings->r_u32	(section,"eat_sound_delay");
	get_sd()->m_dwAttackSndDelay				= pSettings->r_u32	(section,"attack_sound_delay");

	get_sd()->m_fMoraleSuccessAttackQuant	= pSettings->r_float(section,"MoraleSuccessAttackQuant");
	get_sd()->m_fMoraleDeathQuant			= pSettings->r_float(section,"MoraleDeathQuant");
	get_sd()->m_fMoraleFearQuant				= pSettings->r_float(section,"MoraleFearQuant");
	get_sd()->m_fMoraleRestoreQuant			= pSettings->r_float(section,"MoraleRestoreQuant");
	get_sd()->m_fMoraleBroadcastDistance		= pSettings->r_float(section,"MoraleBroadcastDistance");

	get_sd()->m_fEatFreq						= pSettings->r_float(section,"eat_freq");
	get_sd()->m_fEatSlice					= pSettings->r_float(section,"eat_slice");
	get_sd()->m_fEatSliceWeight				= pSettings->r_float(section,"eat_slice_weight");

	get_sd()->m_legs_number					= pSettings->r_u8(section, "LegsCount");
	get_sd()->m_max_hear_dist				= pSettings->r_float(section, "max_hear_dist");

	// Load attack postprocess --------------------------------------------------------
	LPCSTR ppi_section = pSettings->r_string(section, "attack_effector");
	get_sd()->m_attack_effector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	get_sd()->m_attack_effector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	get_sd()->m_attack_effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	get_sd()->m_attack_effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	get_sd()->m_attack_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	get_sd()->m_attack_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	get_sd()->m_attack_effector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &get_sd()->m_attack_effector.ppi.color_base.r, &get_sd()->m_attack_effector.ppi.color_base.g, &get_sd()->m_attack_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &get_sd()->m_attack_effector.ppi.color_gray.r, &get_sd()->m_attack_effector.ppi.color_gray.g, &get_sd()->m_attack_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &get_sd()->m_attack_effector.ppi.color_add.r,	&get_sd()->m_attack_effector.ppi.color_add.g,&get_sd()->m_attack_effector.ppi.color_add.b);

	get_sd()->m_attack_effector.time			= pSettings->r_float(ppi_section,"time");
	get_sd()->m_attack_effector.time_attack	= pSettings->r_float(ppi_section,"time_attack");
	get_sd()->m_attack_effector.time_release	= pSettings->r_float(ppi_section,"time_release");

	get_sd()->m_attack_effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	get_sd()->m_attack_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	get_sd()->m_attack_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	get_sd()->m_attack_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	// --------------------------------------------------------------------------------

}


BOOL CAI_Biting::net_Spawn (LPVOID DC) 
{
	if (!frame_check(m_dwFrameSpawn))
		return	(TRUE);
	
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract						*e	= (CSE_Abstract*)(DC);
	m_pPhysics_support->in_NetSpawn		(e);


//	CSE_ALifeMonsterBiting			*l_tpSE_Biting	= dynamic_cast<CSE_ALifeMonsterBiting*>(e);
//	m_body.current.yaw = m_body.target.yaw	= angle_normalize_signed(-l_tpSE_Biting->o_Angle.y);
	
	R_ASSERT2						(ai().get_level_graph() && ai().get_cross_table() && (ai().level_graph().level_id() != u32(-1)),"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!");
	
	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);

	m_movement_params.insert(std::make_pair(eVelocityParameterStand,		STravelParams(get_sd()->m_fsVelocityStandTurn.velocity.linear,		get_sd()->m_fsVelocityStandTurn.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterWalkNormal,	STravelParams(get_sd()->m_fsVelocityWalkFwdNormal.velocity.linear,	get_sd()->m_fsVelocityWalkFwdNormal.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterRunNormal,	STravelParams(get_sd()->m_fsVelocityRunFwdNormal.velocity.linear,	get_sd()->m_fsVelocityRunFwdNormal.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterWalkDamaged,	STravelParams(get_sd()->m_fsVelocityWalkFwdDamaged.velocity.linear,	get_sd()->m_fsVelocityWalkFwdDamaged.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterRunDamaged,	STravelParams(get_sd()->m_fsVelocityRunFwdDamaged.velocity.linear,	get_sd()->m_fsVelocityRunFwdDamaged.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterSteal,		STravelParams(get_sd()->m_fsVelocitySteal.velocity.linear,			get_sd()->m_fsVelocitySteal.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterDrag,			STravelParams(-get_sd()->m_fsVelocityDrag.velocity.linear,			get_sd()->m_fsVelocityDrag.velocity.angular)));

	monster_squad().register_member((u8)g_Team(),(u8)g_Squad(), this);

	return(TRUE);
}

void CAI_Biting::net_Destroy()
{
	if (!frame_check(m_dwFrameDestroy))
		return;

	inherited::net_Destroy();
	m_pPhysics_support->in_NetDestroy();

	monster_squad().remove_member((u8)g_Team(),(u8)g_Squad(), this);
}

void CAI_Biting::net_Save			(NET_Packet& P)
{
	inherited::net_Save(P);
	m_pPhysics_support->in_NetSave(P);
}
BOOL CAI_Biting::net_SaveRelevant	()
{
	return BOOL(PPhysicsShell()!=NULL);
}
void CAI_Biting::net_Export(NET_Packet& P) 
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_float_q16			(fEntityHealth,-500,1000);
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);
	P.w_u8					(u8(g_Team()));
	P.w_u8					(u8(g_Squad()));
	P.w_u8					(u8(g_Group()));

	ALife::_GRAPH_ID		l_game_vertex_id = game_vertex_id();
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	float					f1 = 0;
	if (ai().game_graph().valid_vertex_id(l_game_vertex_id)) {
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.w					(&f1,						sizeof(f1));
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.w					(&f1,						sizeof(f1));
	}
	else {
		P.w					(&f1,						sizeof(f1));
		P.w					(&f1,						sizeof(f1));
	}

}

void CAI_Biting::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;

	float health;
	P.r_float_q16		(health,-500,1000);
	fEntityHealth = health;

	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);
	id_Team					= P.r_u8();
	id_Squad				= P.r_u8();
	id_Group				= P.r_u8();

	ALife::_GRAPH_ID		l_game_vertex_id = game_vertex_id();
	P.r						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.r						(&l_game_vertex_id,			sizeof(l_game_vertex_id));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	P.r						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.r						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	float					f1 = 0;
	if (ai().game_graph().valid_vertex_id(l_game_vertex_id)) {
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.r					(&f1,						sizeof(f1));
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.r					(&f1,						sizeof(f1));
	}
	else {
		P.r					(&f1,						sizeof(f1));
		P.r					(&f1,						sizeof(f1));
	}


	setVisible				(TRUE);
	setEnabled				(TRUE);
}

void CAI_Biting::UpdateCL()
{
	if (!frame_check(m_dwFrameClient))
		return;

	inherited::UpdateCL();
	
	if (g_Alive()) {
		
		MotionMan.FrameUpdate();

		// ѕроверка состо€ни€ анимации (атака)
		AA_CheckHit();
		MotionMan.STEPS_Update(get_legs_number());

		// ѕоправка Pitch
		bool b_need_pitch_correction = true;
		if (ability_can_jump()) {
			CJumping *pJumping = dynamic_cast<CJumping *>(this);
			if (pJumping && pJumping->IsActive()) b_need_pitch_correction = false;
		}
		if (b_need_pitch_correction) PitchCorrection();

		// ќбновить угловую и линейную скорости движени€
		CMonsterMovement::update_velocity();
		m_fCurSpeed		= m_velocity_linear.current;
		set_desirable_speed(m_fCurSpeed);

		if (!fis_zero(m_velocity_linear.current) && !fis_zero(m_velocity_linear.target))
			m_body.speed	= m_velocity_angular.target * m_velocity_linear.current / (m_velocity_linear.target + EPS_L);
		else 
			m_body.speed	= m_velocity_angular.target;
	}

	Exec_Look			(Device.fTimeDelta);

	m_pPhysics_support->in_UpdateCL();

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug))
		HDebug->M_Update	();
#endif

}

void CAI_Biting::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	
	m_pPhysics_support->in_shedule_Update(dt);
}


//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////


void CAI_Biting::Die()
{
	inherited::Die( );

	CSoundPlayer::play(MonsterSpace::eMonsterSoundDie);
	MotionMan.ForceAnimSelect();

	monster_squad().remove_member((u8)g_Team(),(u8)g_Squad(), this);
}


void CAI_Biting::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CAI_Biting::PHHit(float P,Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*=ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(P,dir,element,p_in_object_space,impulse,hit_type);
}
CBoneInstance *CAI_Biting::GetBoneInstance(LPCTSTR bone_name)
{
	int bone = PKinematics(Visual())->LL_BoneID(bone_name);
	return (&PKinematics(Visual())->LL_GetBoneInstance(u16(bone)));
}

CBoneInstance *CAI_Biting::GetBoneInstance(int bone_id)
{
	return (&PKinematics(Visual())->LL_GetBoneInstance(u16(bone_id)));
}

CBoneInstance *CAI_Biting::GetEatBone()
{
	int bone = PKinematics(Visual())->LL_BoneID("bip01_ponytail2");
	return (&PKinematics(Visual())->LL_GetBoneInstance(u16(bone)));
}


void CAI_Biting::MoraleBroadcast(float fValue)
{
	CGroupHierarchyHolder &Group = Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group());
	for (u32 i=0; i<Group.members().size(); ++i) {
		CEntityAlive *pE = dynamic_cast<CEntityAlive *>(Group.members()[i]);
		if (!pE) continue;
		
		if (pE->g_Alive() && (pE->Position().distance_to(Position()) < get_sd()->m_fMoraleBroadcastDistance)) pE->ChangeEntityMorale(fValue);
	}
}

#ifdef DEBUG
void CAI_Biting::OnRender()
{
	inherited::OnRender();
	
	if (psAI_Flags.test(aiMonsterDebug)) {
		HDebug->L_Update();
		HDebug->HT_Update();
	}
}
#endif

// ѕолучить рассто€ние от fire_bone до врага
// ¬ыполнить RayQuery от fire_bone в enemy.center
float CAI_Biting::GetRealDistToEnemy(const CEntity *pE)
{
	Fvector enemy_center;
	pE->Center(enemy_center);
	
	Fmatrix global_transform;
	global_transform.set(XFORM());
	
	global_transform.mulB(PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_head")).mTransform);
	
	Fvector dir; 
	dir.sub(enemy_center, global_transform.c);
	Collide::ray_defs	r_query(global_transform.c, dir, 10.f, CDB::OPT_CULL | CDB::OPT_ONLYNEAREST, Collide::rqtObject);
	Collide::rq_results	r_res;

	float ret_val = -1.f;

	setEnabled(false);
	if (pE->CFORM()->_RayQuery(r_query, r_res)) 
		if (r_res.r_begin()->O) ret_val = r_res.r_begin()->range;
	setEnabled(true);		
			
	return ret_val;
}


bool CAI_Biting::useful(const CGameObject *object) const
{
	//if (!CItemManager::useful(object))
	//	return			(false);

	const CEntityAlive *pCorpse = dynamic_cast<const CEntityAlive *>(object); 
	if (!pCorpse) return false;
	
	if (!pCorpse->g_Alive()) return true;
	return false;
}

float CAI_Biting::evaluate(const CGameObject *object) const
{
	return (0.f);
}




void CAI_Biting::reload	(LPCSTR section)
{
	if (!frame_check(m_dwFrameReload))
		return;

	CCustomMonster::reload		(section);
	CMonsterMovement::reload	(section);

	LoadFootBones();

	CSoundPlayer::add(pSettings->r_string(section,"sound_idle"),		16,		SOUND_TYPE_MONSTER_TALKING,		7,	u32(1 << 31) | 3,	MonsterSpace::eMonsterSoundIdle, 		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_eat"),			16,		SOUND_TYPE_MONSTER_TALKING,		6,	u32(1 << 31) | 2,	MonsterSpace::eMonsterSoundEat,			"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_attack"),		16,		SOUND_TYPE_MONSTER_ATTACKING,	5,	u32(1 << 31) | 1,	MonsterSpace::eMonsterSoundAttack,		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_attack_hit"),	16,		SOUND_TYPE_MONSTER_ATTACKING,	2,	u32(-1),			MonsterSpace::eMonsterSoundAttackHit,	"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_take_damage"),	16,		SOUND_TYPE_MONSTER_INJURING,	1,	u32(-1),			MonsterSpace::eMonsterSoundTakeDamage,	"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_die"),			16,		SOUND_TYPE_MONSTER_DYING,		0,	u32(-1),			MonsterSpace::eMonsterSoundDie,			"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_threaten"),	16,		SOUND_TYPE_MONSTER_ATTACKING,	3,	u32(1 << 31) | 0,	MonsterSpace::eMonsterSoundThreaten,	"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_landing"),		16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 1,	MonsterSpace::eMonsterSoundLanding,		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_steal"),		16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 5,	MonsterSpace::eMonsterSoundSteal,		"bip01_head");	
	CSoundPlayer::add(pSettings->r_string(section,"sound_panic"),		16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 6,	MonsterSpace::eMonsterSoundPanic,		"bip01_head");	
	CSoundPlayer::add(pSettings->r_string(section,"sound_growling"),	16,		SOUND_TYPE_MONSTER_STEP,		5,	u32(1 << 31) | 7,	MonsterSpace::eMonsterSoundGrowling,	"bip01_head");	
}

float CAI_Biting::get_custom_pitch_speed(float def_speed)
{
	float cur_speed = angle_difference(m_body.current.pitch, m_body.target.pitch) * 4.0f;
	clamp(cur_speed, PI_DIV_6, 5 * PI_DIV_6);

	return cur_speed;
}

#define REAL_DIST_THROUGH_TRACE_THRESHOLD	6.0f

float CAI_Biting::GetEnemyDistances(float &min_dist, float &max_dist, const CEntity *enemy)
{
	// обновить минимальную и максимальную дистанции до врага
	min_dist = m_fCurMinAttackDist;
	max_dist = get_sd()->m_fMaxAttackDist - (get_sd()->m_fMinAttackDist - m_fCurMinAttackDist);

	const CEntity *t_enemy = (enemy) ? enemy : EnemyMan.get_enemy();
	// определить рассто€ние до противника
	float cur_dist = t_enemy->Position().distance_to(Position()); 
	if (cur_dist < REAL_DIST_THROUGH_TRACE_THRESHOLD) cur_dist = GetRealDistToEnemy(t_enemy); 

	return cur_dist;
}

//////////////////////////////////////////////////////////////////////////
// Function for foot processing
//////////////////////////////////////////////////////////////////////////
Fvector	CAI_Biting::get_foot_position(ELegType leg_type)
{
	R_ASSERT2(m_FootBones[leg_type] != BI_NONE, "foot bone had not been set");
	
	CKinematics *pK = PKinematics(Visual());
	Fmatrix bone_transform;

	bone_transform = pK->LL_GetBoneInstance(m_FootBones[leg_type]).mTransform;	

	Fmatrix global_transform;
	global_transform.set(XFORM());
	global_transform.mulB(bone_transform);

	return global_transform.c;
}

void CAI_Biting::LoadFootBones()
{

	CInifile* ini		= PKinematics(Visual())->LL_UserData();
	if(ini&&ini->section_exist("foot_bones")){
		
		CInifile::Sect& data		= ini->r_section("foot_bones");
		for (CInifile::SectIt I=data.begin(); I!=data.end(); I++){
			CInifile::Item& item	= *I;
			
			u16 index = PKinematics(Visual())->LL_BoneID(*item.second);
			VERIFY3(index != BI_NONE, "foot bone not found", *item.second);
			
			if (xr_strcmp(*item.first, "front_left") == 0) 			m_FootBones[eFrontLeft]		= index;
			else if (xr_strcmp(*item.first, "front_right")== 0)		m_FootBones[eFrontRight]	= index;
			else if (xr_strcmp(*item.first, "back_right")== 0)		m_FootBones[eBackRight]		= index;
			else if (xr_strcmp(*item.first, "back_left")== 0)		m_FootBones[eBackLeft]		= index;
		}
	} else VERIFY("section [foot_bones] not found in monster user_data");

	// проверка на соответсвие
	int count = 0;
	for (u32 i = 0; i < eLegsMaxNumber; i++) 
		if (m_FootBones[i] != BI_NONE) count++;

	VERIFY(count == get_legs_number());
}

//////////////////////////////////////////////////////////////////////////

void CAI_Biting::ChangeTeam(int team, int squad, int group)
{
	if ((team == g_Team()) && (squad == g_Squad()) && (group == g_Group())) return;
		
	// remove from current team
	monster_squad().remove_member	((u8)g_Team(),(u8)g_Squad(), this);
	inherited::ChangeTeam			(team,squad,group);
	monster_squad().register_member	((u8)g_Team(),(u8)g_Squad(), this);
}


void CAI_Biting::ProcessTurn()
{
	float delta_yaw = angle_difference(m_body.target.yaw, m_body.current.yaw);
	if (delta_yaw < deg(1)) return;

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

bool CAI_Biting::IsVisibleObject(const CGameObject *object)
{
	return CMemoryManager::visible_now(object);
}


void CAI_Biting::Exec_Look		( float dt )
{
	m_body.current.yaw		= angle_normalize_signed	(m_body.current.yaw);
	m_body.current.pitch	= angle_normalize_signed	(m_body.current.pitch);
	m_body.target.yaw		= angle_normalize_signed	(m_body.target.yaw);
	m_body.target.pitch		= angle_normalize_signed	(m_body.target.pitch);

	float pitch_speed		= get_custom_pitch_speed(m_body.speed);
	angle_lerp_bounds		(m_body.current.yaw,m_body.target.yaw,m_body.speed,dt);
	angle_lerp_bounds		(m_body.current.pitch,m_body.target.pitch,pitch_speed,dt);

	Fvector P				= Position();
	XFORM().setHPB			(-m_body.current.yaw,-m_body.current.pitch,0);
	Position()				= P;
}


