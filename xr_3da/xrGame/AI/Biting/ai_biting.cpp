////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\..\\PhysicsShell.h"
#include "..\\..\\CharacterPhysicsSupport.h"

CAI_Biting::CAI_Biting()
{
	Movement.AllocateCharacterObject(CPHMovementControl::CharacterType::ai_stalker);
	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);
}

CAI_Biting::~CAI_Biting()
{
	xr_delete(m_pPhysics_support);
}

void CAI_Biting::Init()
{
	
	// initializing class members
	m_tCurGP						= _GRAPH_ID(-1);
	m_tNextGP						= _GRAPH_ID(-1);
	m_fGoingSpeed					= 0.f;
	m_dwTimeToChange				= 0;
	
	m_dwLastRangeSearch				= 0;

	// Инициализация параметров анимации
	m_tPathState					= PATH_STATE_SEARCH_NODE;

	m_fAttackSuccessProbability[0]	= .8f;
	m_fAttackSuccessProbability[1]	= .6f;
	m_fAttackSuccessProbability[2]	= .4f;
	m_fAttackSuccessProbability[3]	= .2f;

	InitMemory						(10000,10000);

	m_dwPathBuiltLastTime			= 0;
	
	AI_Path.TravelPath.clear		();
	AI_Path.Nodes.clear				();
	AI_Path.TravelStart				= 0;
	AI_Path.DestNode				= u32(-1);
	m_pPhysics_support				->in_Init();

	flagEatNow						= false;
}

void CAI_Biting::Die()
{
	inherited::Die( );

	DeinitMemory();

	SET_SOUND_ONCE(SND_TYPE_DIE);
}

void CAI_Biting::Load(LPCSTR section)
{
	// load parameters from ".ltx" file
	inherited::Load		(section);
	
	// группы маск точек графа с параметрами
	m_tpaTerrain.clear				();
	LPCSTR							S = pSettings->r_string(section,"terrain");
	u32								N = _GetItemCount(S);
	R_ASSERT						(((N % (LOCATION_TYPE_COUNT + 2)) == 0) && (N));
	STerrainPlace					tTerrainPlace;
	tTerrainPlace.tMask.resize		(LOCATION_TYPE_COUNT);
	m_tpaTerrain.reserve			(32);
	string16						I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; j++, i++)
			tTerrainPlace.tMask[j]	= _LOCATION_ID(atoi(_GetItem(S,i,I)));
		tTerrainPlace.dwMinTime		= atoi(_GetItem(S,i++,I))*1000;
		tTerrainPlace.dwMaxTime		= atoi(_GetItem(S,i++,I))*1000;
		m_tpaTerrain.push_back		(tTerrainPlace);
	}
	m_fGoingSpeed					= pSettings->r_float	(section, "going_speed");

	m_tSelectorFreeHunting.Load		(section,"selector_free_hunting");
	m_tSelectorCover.Load			(section,"selector_cover");

	m_tSelectorGetAway.Load			(section,"selector_getaway");
	m_tSelectorApproach.Load		(section,"selector_approach");

	m_tSelectorHearSnd.Load			(section,"selector_hear_sound");	 // like _free hunting


	// loading frustum parameters
	eye_fov							= pSettings->r_float(section,"EyeFov");
	eye_range						= pSettings->r_float(section,"eye_range");
	m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	m_dwHealth						= pSettings->r_u32   (section,"Health");
	m_fHitPower						= pSettings->r_float (section,"hit_power");
	// temp
	///m_fHitPower						= 1.f;
	fEntityHealth							= (float)m_dwHealth;

	// prefetching
	cNameVisual_set					(pSettings->r_string(section,"visual"));

	m_fImpulseMin					= pSettings->r_float(section,"ImpulseMin");
	m_fImpulseMax					= pSettings->r_float(section,"ImpulseMax");

	m_fsTurnNormalAngular			= pSettings->r_float(section,"TurnNormalAngular");
	m_fsWalkFwdNormal				= pSettings->r_float(section,"WalkFwdNormal");
	m_fsWalkBkwdNormal				= pSettings->r_float(section,"WalkBkwdNormal");
	m_fsWalkTurn					= pSettings->r_float(section,"WalkTurn");
	m_fsWalkAngular					= pSettings->r_float(section,"WalkAngular");
	m_fsWalkTurnAngular				= pSettings->r_float(section,"WalkTurnAngular");
	m_fsRunFwdNormal				= pSettings->r_float(section,"RunFwdNormal");
	m_fsRunTurn						= pSettings->r_float(section,"RunTurn");
	m_fsRunTurnAngular				= pSettings->r_float(section,"RunTurnAngular");
	m_fsRunAngular					= pSettings->r_float(section,"RunAngular");
	m_fsDrag						= pSettings->r_float(section,"Drag");
	m_fsSteal						= pSettings->r_float(section,"Steal");

	m_timeLieIdleMin				= pSettings->r_u32   (section,"LieIdleTimeMin");
	m_timeLieIdleMax				= pSettings->r_u32   (section,"LieIdleTimeMax");
	m_timeStandIdleMin				= pSettings->r_u32   (section,"StandIdleTimeMin");
	m_timeStandIdleMax				= pSettings->r_u32   (section,"StandIdleTimeMax");
	m_timeFreeWalkMin				= pSettings->r_u32   (section,"FreeWalkTimeMin");
	m_timeFreeWalkMax				= pSettings->r_u32   (section,"FreeWalkTimeMax");
	m_timeSleepMin					= pSettings->r_u32   (section,"SleepTimeMin");
	m_timeSleepMax					= pSettings->r_u32   (section,"SleepTimeMax");
	
	m_dwProbRestWalkFree			= pSettings->r_u32   (section,"ProbRestWalkFree");
	m_dwProbRestStandIdle			= pSettings->r_u32   (section,"ProbRestStandIdle");
	m_dwProbRestLieIdle				= pSettings->r_u32   (section,"ProbRestLieIdle");
	m_dwProbRestTurnLeft			= pSettings->r_u32   (section,"ProbRestTurnLeft");

	m_dwDayTimeBegin				= pSettings->r_u32	(section,"DayTime_Begin");
	m_dwDayTimeEnd					= pSettings->r_u32	(section,"DayTime_End");		
	m_fMinSatiety					= pSettings->r_float(section,"Min_Satiety");
	m_fMaxSatiety					= pSettings->r_float(section,"Max_Satiety");

	m_fDistToCorpse					= pSettings->r_float(section,"distance_to_corpse");
	m_fMinAttackDist				= pSettings->r_float(section,"MinAttackDist");
	m_fMaxAttackDist				= pSettings->r_float(section,"MaxAttackDist");

	LoadSounds						(section);

	m_pPhysics_support				->in_Load(section);
	R_ASSERT2 ((m_dwProbRestWalkFree + m_dwProbRestStandIdle + m_dwProbRestLieIdle + m_dwProbRestTurnLeft) == 100, "Probability sum isn't 1");
}

BOOL CAI_Biting::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterBiting			*l_tpSE_Biting	= dynamic_cast<CSE_ALifeMonsterBiting*>(e);
	
	r_current.yaw = r_target.yaw = r_torso_current.yaw = r_torso_target.yaw	= angle_normalize_signed(-l_tpSE_Biting->o_Angle.y);

	cNameVisual_set					(l_tpSE_Biting->get_visual());
	
	R_ASSERT2						(getAI().bfCheckIfMapLoaded() && getAI().bfCheckIfGraphLoaded() && getAI().bfCheckIfCrossTableLoaded() && (getAI().m_dwCurrentLevelID != u32(-1)),"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!");
	m_tNextGP						= m_tCurGP = getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
	
	// loading animation stuff
#pragma	todo("Jim to Jim: Bring all motion loading stuff off from Net_Spawn")
	MotionMan.Init					(this, PKinematics(Visual()));

	m_pPhysics_support->in_NetSpawn();

	Movement.SetPosition	(Position());
	Movement.SetVelocity	(0,0,0);

	return(TRUE);
}

void CAI_Biting::net_Destroy()
{
	inherited::net_Destroy();
	Init();
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

	P.w						(&m_tNextGP,				sizeof(m_tNextGP));
	P.w						(&m_tCurGP,					sizeof(m_tCurGP));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	float					f1;
	f1						= Position().distance_to		(getAI().m_tpaGraph[m_tCurGP].tLocalPoint);
	P.w						(&f1,						sizeof(f1));
	f1						= Position().distance_to		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
	P.w						(&f1,						sizeof(f1));
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

void CAI_Biting::Exec_Movement		(float dt)
{
	AI_Path.Calculate				(this,Position(),Position(),m_fCurSpeed,dt);
}


//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////

void CAI_Biting::UpdateCL()
{
	inherited::UpdateCL();
	
	// Проверка состояния анимации (атака)
	if (g_Alive()) CheckAttackHit();

	MotionMan.ProcessJump();

	m_pPhysics_support->in_UpdateCL();

}

void CAI_Biting::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	////physics/////////////////////////////////////////////////////////////////////////////////////
	m_pPhysics_support->in_shedule_Update(dt);

}

void CAI_Biting::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	if(m_pPhysics_support->isAlive())inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse);

	if (g_Alive()) SET_SOUND_ONCE(SND_TYPE_TAKE_DAMAGE);
}

CBoneInstance *CAI_Biting::GetBone(LPCTSTR bone_name)
{
	int bone = PKinematics(Visual())->LL_BoneID(bone_name);
	return (&PKinematics(Visual())->LL_GetInstance(u16(bone)));
}
CBoneInstance *CAI_Biting::GetBone(int bone_id)
{
	return (&PKinematics(Visual())->LL_GetInstance(u16(bone_id)));
}

