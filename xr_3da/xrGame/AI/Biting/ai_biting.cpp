
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

	// ������������� ���������� ��������	
	MotionMan.Init					(this);

	for (u32 i = 0; i < eLegsMaxNumber; i++) m_FootBones[i] = BI_NONE;

	m_fGoingSpeed					= 0.f;

	// Attack-stops init
	AS_Init							();
}

CAI_Biting::~CAI_Biting()
{
	xr_delete(m_pPhysics_support);
#ifdef DEBUG
	xr_delete(HDebug);
#endif	
}

void CAI_Biting::reinit()
{
	if (!frame_check(m_dwFrameReinit))
		return;
	
	m_pPhysics_support->in_NetSpawn		();
	
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

	anim_speed						= -1.f;
	cur_blend						= 0;

	cur_anim.anim					= eAnimStandIdle;
	cur_anim.index					= 0;
	cur_anim.started				= 0;

	state_invisible					= false;


#ifdef 	DEEP_TEST_SPEED	
	time_next_update				= 0;
#endif

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

	m_fCurMinAttackDist				= _sd->m_fMinAttackDist;

	CSoundPlayer::add(pSettings->r_string(section,"sound_idle"),		16,		SOUND_TYPE_MONSTER_TALKING,		7,	u32(1 << 31) | 3,	MonsterSpace::eMonsterSoundIdle, 		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_eat"),			16,		SOUND_TYPE_MONSTER_TALKING,		6,	u32(1 << 31) | 2,	MonsterSpace::eMonsterSoundEat,			"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_attack"),		16,		SOUND_TYPE_MONSTER_ATTACKING,	5,	u32(1 << 31) | 1,	MonsterSpace::eMonsterSoundAttack,		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_attack_hit"),	16,		SOUND_TYPE_MONSTER_ATTACKING,	2,	u32(-1),			MonsterSpace::eMonsterSoundAttackHit,	"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_take_damage"),	16,		SOUND_TYPE_MONSTER_INJURING,	1,	u32(-1),			MonsterSpace::eMonsterSoundTakeDamage,	"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_die"),			16,		SOUND_TYPE_MONSTER_DYING,		0,	u32(-1),			MonsterSpace::eMonsterSoundDie,			"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_bkgnd"),		16,		SOUND_TYPE_MONSTER_STEP,		1,	u32(1 << 15) | 0,	MonsterSpace::eMonsterSoundBkgnd,		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_threaten"),	16,		SOUND_TYPE_MONSTER_ATTACKING,	3,	u32(1 << 31) | 0,	MonsterSpace::eMonsterSoundThreaten,	"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_landing"),		16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 1,	MonsterSpace::eMonsterSoundLanding,		"bip01_head");
	CSoundPlayer::add(pSettings->r_string(section,"sound_steal"),		16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 5,	MonsterSpace::eMonsterSoundSteal,		"bip01_head");	
	CSoundPlayer::add(pSettings->r_string(section,"sound_panic"),		16,		SOUND_TYPE_MONSTER_STEP,		4,	u32(1 << 31) | 6,	MonsterSpace::eMonsterSoundPanic,		"bip01_head");	
	CSoundPlayer::add(pSettings->r_string(section,"sound_growling"),	16,		SOUND_TYPE_MONSTER_STEP,		5,	u32(1 << 31) | 7,	MonsterSpace::eMonsterSoundGrowling,	"bip01_head");	

}

void CAI_Biting::load_shared(LPCSTR section)
{
	// �������� ���������� �� LTX
	_sd->m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	_sd->m_fsVelocityStandTurn.Load		(section,"Velocity_Stand");
	_sd->m_fsVelocityWalkFwdNormal.Load (section,"Velocity_WalkFwdNormal");
	_sd->m_fsVelocityWalkFwdDamaged.Load(section,"Velocity_WalkFwdDamaged");
	_sd->m_fsVelocityRunFwdNormal.Load	(section,"Velocity_RunFwdNormal");
	_sd->m_fsVelocityRunFwdDamaged.Load (section,"Velocity_RunFwdDamaged");
	_sd->m_fsVelocityDrag.Load			(section,"Velocity_Drag");
	_sd->m_fsVelocitySteal.Load			(section,"Velocity_Steal");

	_sd->m_dwDayTimeBegin				= pSettings->r_u32	(section,"DayTime_Begin");
	_sd->m_dwDayTimeEnd					= pSettings->r_u32	(section,"DayTime_End");		
	_sd->m_fMinSatiety					= pSettings->r_float(section,"Min_Satiety");
	_sd->m_fMaxSatiety					= pSettings->r_float(section,"Max_Satiety");

	_sd->m_fDistToCorpse				= pSettings->r_float(section,"distance_to_corpse");
	_sd->m_fMinAttackDist				= pSettings->r_float(section,"MinAttackDist");
	_sd->m_fMaxAttackDist				= pSettings->r_float(section,"MaxAttackDist");

	_sd->m_fDamagedThreshold			= pSettings->r_float(section,"DamagedThreshold");

	_sd->m_dwIdleSndDelay				= pSettings->r_u32	(section,"idle_sound_delay");
	_sd->m_dwEatSndDelay				= pSettings->r_u32	(section,"eat_sound_delay");
	_sd->m_dwAttackSndDelay				= pSettings->r_u32	(section,"attack_sound_delay");

	_sd->m_fMoraleSuccessAttackQuant	= pSettings->r_float(section,"MoraleSuccessAttackQuant");
	_sd->m_fMoraleDeathQuant			= pSettings->r_float(section,"MoraleDeathQuant");
	_sd->m_fMoraleFearQuant				= pSettings->r_float(section,"MoraleFearQuant");
	_sd->m_fMoraleRestoreQuant			= pSettings->r_float(section,"MoraleRestoreQuant");
	_sd->m_fMoraleBroadcastDistance		= pSettings->r_float(section,"MoraleBroadcastDistance");

	_sd->m_fEatFreq						= pSettings->r_float(section,"eat_freq");
	_sd->m_fEatSlice					= pSettings->r_float(section,"eat_slice");
	_sd->m_fEatSliceWeight				= pSettings->r_float(section,"eat_slice_weight");


	// Load attack postprocess --------------------------------------------------------
	LPCSTR ppi_section = pSettings->r_string(section, "attack_effector");
	_sd->m_attack_effector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	_sd->m_attack_effector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	_sd->m_attack_effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	_sd->m_attack_effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	_sd->m_attack_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	_sd->m_attack_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	_sd->m_attack_effector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &_sd->m_attack_effector.ppi.color_base.r, &_sd->m_attack_effector.ppi.color_base.g, &_sd->m_attack_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &_sd->m_attack_effector.ppi.color_gray.r, &_sd->m_attack_effector.ppi.color_gray.g, &_sd->m_attack_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &_sd->m_attack_effector.ppi.color_add.r,	&_sd->m_attack_effector.ppi.color_add.g,&_sd->m_attack_effector.ppi.color_add.b);

	_sd->m_attack_effector.time			= pSettings->r_float(ppi_section,"time");
	_sd->m_attack_effector.time_attack	= pSettings->r_float(ppi_section,"time_attack");
	_sd->m_attack_effector.time_release	= pSettings->r_float(ppi_section,"time_release");

	_sd->m_attack_effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	_sd->m_attack_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	_sd->m_attack_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	_sd->m_attack_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	// --------------------------------------------------------------------------------

}


BOOL CAI_Biting::net_Spawn (LPVOID DC) 
{
	if (!frame_check(m_dwFrameSpawn))
		return	(TRUE);
	
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterBiting			*l_tpSE_Biting	= dynamic_cast<CSE_ALifeMonsterBiting*>(e);
	
	m_body.current.yaw = m_body.target.yaw	= angle_normalize_signed(-l_tpSE_Biting->o_Angle.y);
	
	R_ASSERT2						(ai().get_level_graph() && ai().get_cross_table() && (ai().level_graph().level_id() != u32(-1)),"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!");
	
	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);

	m_movement_params.insert(std::make_pair(eVelocityParameterStand,		STravelParams(_sd->m_fsVelocityStandTurn.velocity.linear,		_sd->m_fsVelocityStandTurn.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterWalkNormal,	STravelParams(_sd->m_fsVelocityWalkFwdNormal.velocity.linear,	_sd->m_fsVelocityWalkFwdNormal.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterRunNormal,	STravelParams(_sd->m_fsVelocityRunFwdNormal.velocity.linear,	_sd->m_fsVelocityRunFwdNormal.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterWalkDamaged,	STravelParams(_sd->m_fsVelocityWalkFwdDamaged.velocity.linear,	_sd->m_fsVelocityWalkFwdDamaged.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterRunDamaged,	STravelParams(_sd->m_fsVelocityRunFwdDamaged.velocity.linear,	_sd->m_fsVelocityRunFwdDamaged.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterSteal,		STravelParams(_sd->m_fsVelocitySteal.velocity.linear,			_sd->m_fsVelocitySteal.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterDrag,			STravelParams(-_sd->m_fsVelocityDrag.velocity.linear,			_sd->m_fsVelocityDrag.velocity.angular)));

	return(TRUE);
}

void CAI_Biting::net_Destroy()
{
	if (!frame_check(m_dwFrameDestroy))
		return;

	inherited::net_Destroy();
	m_pPhysics_support->in_NetDestroy();
}

void CAI_Biting::net_Export(NET_Packet& P) 
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_float_q16			(fEntityHealth,-1000,1000);
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);

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
	P.r_float_q16		(health,-1000,1000);
	fEntityHealth = health;

	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);

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
	
	// �������� ��������� �������� (�����)
	if (g_Alive()) {
		AA_CheckHit();

		MotionMan.STEPS_Update(get_legs_number());
	}

	
	CJumping *pJumping = dynamic_cast<CJumping *>(this);
	if (!pJumping || !pJumping->IsActive())
		PitchCorrection();


	m_pPhysics_support->in_UpdateCL();

	CMonsterMovement::update_velocity();
	m_fCurSpeed		= m_velocity_linear.current;

	if (!fis_zero(m_velocity_linear.current) && !fis_zero(m_velocity_linear.target))
		m_body.speed	= m_velocity_angular.target * m_velocity_linear.current / (m_velocity_linear.target + EPS_L);
	else 
		m_body.speed	= m_velocity_angular.target;

#ifdef DEBUG
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

	Level().SquadMan.RemoveMember((u8)g_Squad(), this);
}


void CAI_Biting::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CAI_Biting::PHHit(Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*=ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(dir,element,p_in_object_space,impulse,hit_type);
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
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	for (u32 i=0; i<Group.Members.size(); ++i) {
		CEntityAlive *pE = dynamic_cast<CEntityAlive *>(Group.Members[i]);
		if (!pE) continue;
		
		if (pE->g_Alive() && (pE->Position().distance_to(Position()) < _sd->m_fMoraleBroadcastDistance)) pE->ChangeEntityMorale(fValue);
	}
}

#ifdef DEBUG
void CAI_Biting::OnRender()
{
	inherited::OnRender();
	

	HDebug->L_Update();
	HDebug->HT_Update();
}
#endif

// �������� ���������� �� fire_bone �� �����
// ��������� RayQuery �� fire_bone � enemy.center
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
	if (!CItemManager::useful(object))
		return			(false);


	const CEntityAlive *pCorpse = dynamic_cast<const CEntityAlive *>(object); 
	if (!pCorpse) return false;
	
	if (!pCorpse->g_Alive()) return true;
	return false;
}

void CAI_Biting::reload	(LPCSTR section)
{
	if (!frame_check(m_dwFrameReload))
		return;

	CCustomMonster::reload		(section);
	CMonsterMovement::reload	(section);

	LoadFootBones();
}


void CAI_Biting::PitchCorrection() 
{
	CLevelGraph::SContour	contour;
	ai().level_graph().contour(contour, level_vertex_id());
	
	Fplane  P;
	P.build(contour.v1,contour.v2,contour.v3);

	// ������� �������� �����, ������� �� ������� �������� �����������
	Fvector dir_point, proj_point;
	dir_point.mad(Position(), Direction(), 1.f);
	P.project(proj_point,dir_point);
	
	// �������� ������� ������ �����������
	Fvector target_dir;
	target_dir.sub(proj_point,Position());

	float yaw,pitch;
	target_dir.getHP(yaw,pitch);

	m_body.target.pitch = -pitch;

}

float CAI_Biting::get_custom_pitch_speed(float def_speed)
{
	float cur_speed = angle_difference(m_body.current.pitch, m_body.target.pitch) * 4.0f;
	clamp(cur_speed, PI_DIV_6, 5 * PI_DIV_6);

	return cur_speed;
}

#define REAL_DIST_THROUGH_TRACE_THRESHOLD	6.0f

float CAI_Biting::GetEnemyDistances(float &min_dist, float &max_dist)
{
	// �������� ����������� � ������������ ��������� �� �����
	min_dist = m_fCurMinAttackDist;
	max_dist = _sd->m_fMaxAttackDist - (_sd->m_fMinAttackDist - m_fCurMinAttackDist);

	// ���������� ���������� �� ����������
	float cur_dist = EnemyMan.get_enemy()->Position().distance_to(Position()); 
	if (cur_dist < REAL_DIST_THROUGH_TRACE_THRESHOLD) cur_dist = GetRealDistToEnemy(EnemyMan.get_enemy()); 

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

	// �������� �� �����������
	int count = 0;
	for (u32 i = 0; i < eLegsMaxNumber; i++) 
		if (m_FootBones[i] != BI_NONE) count++;

	VERIFY(count == get_legs_number());
}

//////////////////////////////////////////////////////////////////////////
