////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../../PhysicsShell.h"
#include "../../CharacterPhysicsSupport.h"
#include "../../game_level_cross_table.h"
#include "../../game_graph.h"
#include "../../phmovementcontrol.h"
#include "../../xrserver_objects_alife_monsters.h"
#include "../ai_monster_jump.h"

CAI_Biting::CAI_Biting()
{
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);

#ifdef DEBUG
	HDebug							= xr_new<CMonsterDebug>(this, Fvector().set(0.0f,2.0f,0.0f), 20.f);
#endif
}

CAI_Biting::~CAI_Biting()
{
	xr_delete(m_pPhysics_support);
#ifdef DEBUG
	xr_delete(HDebug);
#endif	
}

void CAI_Biting::Init()
{
	// initializing class members
	m_fGoingSpeed					= 0.f;
	

	m_fAttackSuccessProbability[0]	= .8f;
	m_fAttackSuccessProbability[1]	= .6f;
	m_fAttackSuccessProbability[2]	= .4f;
	m_fAttackSuccessProbability[3]	= .2f;

	InitMemory						(20000,15000);

	CDetailPathManager::Init();

	m_pPhysics_support				->in_Init();

	flagEatNow						= false;

	// Attack-stops init
	AS_Init							();

	// ������������� ���������� ��������	
	MotionMan.Init					(this);


}

void CAI_Biting::reinit()
{
	m_pPhysics_support->in_NetSpawn		();
	
	inherited::reinit					();
	CMonsterMovement::reinit			();

#ifdef 	DEEP_TEST_SPEED	
	time_next_update				= 0;
#endif

	CMonsterMovement::Init();
}



void CAI_Biting::Die()
{
	inherited::Die( );
	DeinitMemory();

	CSoundPlayer::play(MonsterSpace::eMonsterSoundDie);

	MotionMan.ForceAnimSelect();
	
	Level().SquadMan.RemoveMember((u8)g_Squad(), this);
}

void CAI_Biting::Load(LPCSTR section)
{
	// load parameters from ".ltx" file
	inherited::Load					(section);
	CMonsterMovement::Load			(section);
	
	AS_Load							(section);

	m_pPhysics_support				->in_Load(section);
	
	m_tSelectorApproach->Load		(section,"selector_approach");

	m_fGoingSpeed					= pSettings->r_float	(section, "going_speed");
	m_dwHealth						= pSettings->r_u32		(section,"Health");

	fEntityHealth					= (float)m_dwHealth;
	fire_bone_id					= PKinematics(Visual())->LL_BoneID(pSettings->r_string(section,"bone_fire"));

	// Load shared data
	SHARE_ON_LOAD(_sd_biting);

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
}

void CAI_Biting::LoadShared(LPCSTR section)
{
	// �������� ���������� �� LTX
	_sd->m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");
	_sd->m_fHitPower					= pSettings->r_float (section,"hit_power");

	_sd->m_fImpulseMin					= pSettings->r_float(section,"ImpulseMin");
	_sd->m_fImpulseMax					= pSettings->r_float(section,"ImpulseMax");

	_sd->m_fsTurnNormalAngular			= pSettings->r_float(section,"TurnNormalAngular");
	_sd->m_fsWalkFwdNormal				= pSettings->r_float(section,"WalkFwdNormal");
	_sd->m_fsWalkFwdDamaged				= pSettings->r_float(section,"WalkFwdDamaged");
	_sd->m_fsWalkBkwdNormal				= pSettings->r_float(section,"WalkBkwdNormal");
 	_sd->m_fsWalkAngular				= pSettings->r_float(section,"WalkAngular");
	_sd->m_fsRunFwdNormal				= pSettings->r_float(section,"RunFwdNormal");
	_sd->m_fsRunFwdDamaged				= pSettings->r_float(section,"RunFwdDamaged");
	_sd->m_fsRunAngular					= pSettings->r_float(section,"RunAngular");
	_sd->m_fsDrag						= pSettings->r_float(section,"Drag");
	_sd->m_fsSteal						= pSettings->r_float(section,"Steal");

	_sd->m_dwProbRestWalkFree			= pSettings->r_u32   (section,"ProbRestWalkFree");
	_sd->m_dwProbRestStandIdle			= pSettings->r_u32   (section,"ProbRestStandIdle");
	_sd->m_dwProbRestLieIdle			= pSettings->r_u32   (section,"ProbRestLieIdle");
	_sd->m_dwProbRestTurnLeft			= pSettings->r_u32   (section,"ProbRestTurnLeft");

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

	_sd->m_bUsedSquadAttackAlg			= pSettings->r_u8	(section,"squad_attack_algorithm");

	AddStepSound(section, eAnimWalkFwd,			"step_snd_walk");
	AddStepSound(section, eAnimWalkDamaged,		"step_snd_walk_dmg");
	AddStepSound(section, eAnimRun,				"step_snd_run");
	AddStepSound(section, eAnimRunDamaged,		"step_snd_run_dmg");
	AddStepSound(section, eAnimStandTurnLeft,	"step_snd_turn");
	AddStepSound(section, eAnimStandTurnRight,	"step_snd_turn");
	AddStepSound(section, eAnimSteal,			"step_snd_steal");
	AddStepSound(section, eAnimDragCorpse,		"step_snd_drag");
	AddStepSound(section, eAnimWalkTurnLeft,	"step_snd_walk");
	AddStepSound(section, eAnimWalkTurnRight,	"step_snd_walk");
	AddStepSound(section, eAnimRunTurnLeft,		"step_snd_run");
	AddStepSound(section, eAnimRunTurnRight,	"step_snd_run");

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

	R_ASSERT2 (100 == (_sd->m_dwProbRestWalkFree + _sd->m_dwProbRestStandIdle + _sd->m_dwProbRestLieIdle + _sd->m_dwProbRestTurnLeft), "Probability sum isn't 1");
}


BOOL CAI_Biting::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterBiting			*l_tpSE_Biting	= dynamic_cast<CSE_ALifeMonsterBiting*>(e);
	
	m_body.current.yaw = m_body.target.yaw	= angle_normalize_signed(-l_tpSE_Biting->o_Angle.y);
	
	R_ASSERT2						(ai().get_level_graph() && ai().get_cross_table() && (ai().level_graph().level_id() != u32(-1)),"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!");
	
	// ���������� ����� Visual, ������������� ��������
	MotionMan.UpdateVisual();

	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);

	CMonsterSquad	*pSquad = Level().SquadMan.GetSquad((u8)g_Squad());
	if ((pSquad->GetLeader() == this)) { 
		pSquad->SetupAlgType(ESquadAttackAlg(_sd->m_bUsedSquadAttackAlg));
	}


	m_movement_params.insert(std::make_pair(eVelocityParameterStand,		STravelParams(0.f,						_sd->m_fsTurnNormalAngular	)));
	m_movement_params.insert(std::make_pair(eVelocityParameterWalkNormal,	STravelParams(_sd->m_fsWalkFwdNormal,	_sd->m_fsWalkAngular		)));
	m_movement_params.insert(std::make_pair(eVelocityParameterRunNormal,	STravelParams(_sd->m_fsRunFwdNormal,	_sd->m_fsRunAngular			)));
	m_movement_params.insert(std::make_pair(eVelocityParameterWalkDamaged,	STravelParams(_sd->m_fsWalkFwdDamaged,	_sd->m_fsWalkAngular		)));
	m_movement_params.insert(std::make_pair(eVelocityParameterRunDamaged,	STravelParams(_sd->m_fsRunFwdDamaged,	_sd->m_fsRunAngular			)));
	m_movement_params.insert(std::make_pair(eVelocityParameterSteal,		STravelParams(_sd->m_fsSteal,			_sd->m_fsWalkAngular		)));
	m_movement_params.insert(std::make_pair(eVelocityParameterDrag,			STravelParams(-_sd->m_fsDrag,			_sd->m_fsWalkAngular		)));

	return(TRUE);
}

void CAI_Biting::net_Destroy()
{
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
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible				(TRUE);
	setEnabled				(TRUE);
}

//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////

void CAI_Biting::UpdateCL()
{
	inherited::UpdateCL();
	
	// �������� ��������� �������� (�����)
	if (g_Alive()) {
		AA_CheckHit();

		// step sounds
		float vol = 0.0f, freq = 1.0f;
		GetStepSound(MotionMan.GetCurAnim(), vol, freq);
		
		CMaterialManager::update(Device.fTimeDelta,vol,freq,!!fis_zero(speed()));
	}

	CJumping *pJumping = dynamic_cast<CJumping *>(this);
	if (!pJumping || !pJumping->IsActive())
		PitchCorrection();


	m_pPhysics_support->in_UpdateCL();

#ifdef DEBUG
	HDebug->M_Update	();
#endif
}

void CAI_Biting::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	
	m_pPhysics_support->in_shedule_Update(dt);
}

void CAI_Biting::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	if(m_pPhysics_support->isAlive())inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse);

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
float CAI_Biting::GetRealDistToEnemy()
{
	Fvector enemy_center;
	m_tEnemy.obj->Center(enemy_center);
	
	Fmatrix global_transform;
	global_transform.set(XFORM());
	global_transform.mulB(PKinematics(Visual())->LL_GetBoneInstance(fire_bone_id).mTransform);

	
	Fvector dir; 
	dir.sub(enemy_center, global_transform.c);
	Collide::ray_defs	r_query(global_transform.c, dir, 10.f, CDB::OPT_CULL | CDB::OPT_ONLYNEAREST, Collide::rqtDynamic);
	Collide::rq_results	r_res;

	float ret_val = -1.f;

	setEnabled(false);
	if (m_tEnemy.obj->CFORM()->_RayQuery(r_query, r_res)) 
		if (r_res.r_begin()->O) ret_val = r_res.r_begin()->range;
	setEnabled(true);		
			
	return ret_val;
}


void CAI_Biting::AddStepSound(LPCSTR section, EMotionAnim a, LPCSTR name)
{
	LPCSTR		str;
	str = pSettings->r_string(section,name);

	string16	first,second;
	_GetItem(str,0,first);
	_GetItem(str,1,second);

	SStepSound		step_snd;
	step_snd.vol	= float(atof(first));
	step_snd.freq	= float(atof(second));

	_sd->step_sounds.insert				(mk_pair(a, step_snd));
}

void CAI_Biting::GetStepSound(EMotionAnim a, float &vol, float &freq) 
{
	STEP_SOUND_MAP_IT it = _sd->step_sounds.find(a);
	if (it == _sd->step_sounds.end()) return;
	
	vol		= it->second.vol;
	freq	= it->second.freq; 
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
	CCustomMonster::reload		(section);
	CMonsterMovement::reload	(section);
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



