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

CAI_Biting::CAI_Biting()
{
	m_PhysicMovementControl.AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);

	m_tSelectorApproach				= xr_new<PathManagers::CVertexEvaluator<aiSearchRange | aiEnemyDistance>  >();
	m_tSelectorGetAway				= xr_new<PathManagers::CVertexEvaluator<aiSearchRange | aiEnemyDistance>  >();
	m_tSelectorWalkAround			= xr_new<PathManagers::CVertexEvaluator<aiSearchRange | aiEnemyDistance>  >();
	m_tSelectorCommon				= xr_new<PathManagers::CVertexEvaluator<aiSearchRange | aiEnemyDistance>  >();
}

CAI_Biting::~CAI_Biting()
{
	xr_delete(m_pPhysics_support);
	xr_delete(m_tSelectorApproach);
	xr_delete(m_tSelectorGetAway);
	xr_delete(m_tSelectorWalkAround);
	xr_delete(m_tSelectorCommon);
}

void CAI_Biting::Init()
{
	inherited::Init					();

	// initializing class members
	m_tCurGP						= _GRAPH_ID(-1);
	m_tNextGP						= _GRAPH_ID(-1);
	m_fGoingSpeed					= 0.f;
	m_dwTimeToChange				= 0;
	
	m_previous_query_time				= 0;

	m_tPathState					= PATH_STATE_SEARCH_NODE;

	m_fAttackSuccessProbability[0]	= .8f;
	m_fAttackSuccessProbability[1]	= .6f;
	m_fAttackSuccessProbability[2]	= .4f;
	m_fAttackSuccessProbability[3]	= .2f;

	InitMemory						(10000,10000);

	m_dwPathBuiltLastTime			= 0;
	
	CDetailPathManager::Init();

	m_pPhysics_support				->in_Init();

	flagEatNow						= false;

	CMonsterSound::Init				(this);
	// Attack-stops init
	AS_Init							();

	// ������������� ���������� ��������	
	MotionMan.Init					(this);

}

void CAI_Biting::Die()
{
	inherited::Die( );
	DeinitMemory();

	SetSoundOnce(SND_TYPE_DIE, m_dwCurrentTime);

	MotionMan.ForceAnimSelect();
}

void CAI_Biting::Load(LPCSTR section)
{
	// load parameters from ".ltx" file
	inherited::Load		(section);
	
	// ������ ���� ����� ����� � �����������
	m_tpaTerrain.clear				();
	LPCSTR							S = pSettings->r_string(section,"terrain");
	u32								N = _GetItemCount(S);
	R_ASSERT						(!(N % (LOCATION_TYPE_COUNT + 2)) && N);
	STerrainPlace					tTerrainPlace;
	tTerrainPlace.tMask.resize		(LOCATION_TYPE_COUNT);
	m_tpaTerrain.reserve			(32);
	string16						I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; ++j, ++i)
			tTerrainPlace.tMask[j]	= _LOCATION_ID(atoi(_GetItem(S,i,I)));
		tTerrainPlace.dwMinTime		= atoi(_GetItem(S,i++,I))*1000;
		tTerrainPlace.dwMaxTime		= atoi(_GetItem(S,i++,I))*1000;
		m_tpaTerrain.push_back		(tTerrainPlace);
	}

	AS_Load							(section);

	m_pPhysics_support				->in_Load(section);
	
	m_tSelectorApproach->Load		(section,"selector_approach");
	m_tSelectorGetAway->Load		(section,"selector_getaway");
	m_tSelectorWalkAround->Load		(section,"selector_walk_around");

	LoadSounds						(section);

	eye_fov							= pSettings->r_float(section,"EyeFov");
	eye_range						= pSettings->r_float(section,"eye_range");

	// prefetching
	cNameVisual_set					(pSettings->r_string(section,"visual"));
	
	m_fGoingSpeed					= pSettings->r_float	(section, "going_speed");
	m_dwHealth						= pSettings->r_u32		(section,"Health");

	fEntityHealth					= (float)m_dwHealth;

	
	// Load shared data
	SHARE_ON_LOAD(_sd_biting);

	m_fCurMinAttackDist				= _sd->m_fMinAttackDist;
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
	_sd->m_fsWalkTurn					= pSettings->r_float(section,"WalkTurn");
	_sd->m_fsWalkAngular				= pSettings->r_float(section,"WalkAngular");
	_sd->m_fsWalkTurnAngular			= pSettings->r_float(section,"WalkTurnAngular");
	_sd->m_fsRunFwdNormal				= pSettings->r_float(section,"RunFwdNormal");
	_sd->m_fsRunFwdDamaged				= pSettings->r_float(section,"RunFwdDamaged");
	_sd->m_fsRunTurn					= pSettings->r_float(section,"RunTurn");
	_sd->m_fsRunTurnAngular				= pSettings->r_float(section,"RunTurnAngular");
	_sd->m_fsRunAngular					= pSettings->r_float(section,"RunAngular");
	_sd->m_fsDrag						= pSettings->r_float(section,"Drag");
	_sd->m_fsSteal						= pSettings->r_float(section,"Steal");

	_sd->m_timeLieIdleMin				= pSettings->r_u32   (section,"LieIdleTimeMin");
	_sd->m_timeLieIdleMax				= pSettings->r_u32   (section,"LieIdleTimeMax");
	_sd->m_timeStandIdleMin				= pSettings->r_u32   (section,"StandIdleTimeMin");
	_sd->m_timeStandIdleMax				= pSettings->r_u32   (section,"StandIdleTimeMax");
	_sd->m_timeFreeWalkMin				= pSettings->r_u32   (section,"FreeWalkTimeMin");
	_sd->m_timeFreeWalkMax				= pSettings->r_u32   (section,"FreeWalkTimeMax");
	_sd->m_timeSleepMin					= pSettings->r_u32   (section,"SleepTimeMin");
	_sd->m_timeSleepMax					= pSettings->r_u32   (section,"SleepTimeMax");

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

	R_ASSERT2 (100 == (_sd->m_dwProbRestWalkFree + _sd->m_dwProbRestStandIdle + _sd->m_dwProbRestLieIdle + _sd->m_dwProbRestTurnLeft), "Probability sum isn't 1");
}


BOOL CAI_Biting::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterBiting			*l_tpSE_Biting	= dynamic_cast<CSE_ALifeMonsterBiting*>(e);
	
	m_head.current.yaw = m_head.target.yaw = m_body.current.yaw = m_body.target.yaw	= angle_normalize_signed(-l_tpSE_Biting->o_Angle.y);
	
	R_ASSERT2						(ai().get_level_graph() && ai().get_cross_table() && (ai().level_graph().level_id() != u32(-1)),"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!");
	m_tNextGP						= m_tCurGP = ai().cross_table().vertex(level_vertex_id()).game_vertex_id();
	
	// ���������� ����� Visual, ������������� ��������
	MotionMan.UpdateVisual();

	m_pPhysics_support->in_NetSpawn();

	m_PhysicMovementControl.SetPosition	(Position());
	m_PhysicMovementControl.SetVelocity	(0,0,0);

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

	P.r						(&m_tNextGP,				sizeof(m_tNextGP));
	P.r						(&m_tCurGP,					sizeof(m_tCurGP));

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
	}

	m_pPhysics_support->in_UpdateCL();
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

void CAI_Biting::MoraleBroadcast(float fValue)
{
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	for (u32 i=0; i<Group.Members.size(); ++i) {
		CEntityAlive *pE = dynamic_cast<CEntityAlive *>(Group.Members[i]);
		if (!pE) continue;
		
		if (pE->g_Alive() && (pE->Position().distance_to(Position()) < _sd->m_fMoraleBroadcastDistance)) pE->ChangeEntityMorale(fValue);
	}
}
