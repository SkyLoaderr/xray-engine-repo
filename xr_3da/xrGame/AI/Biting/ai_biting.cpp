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
	m_tpCurAnim						= 0;
	m_tCurGP						= _GRAPH_ID(-1);
	m_tNextGP						= _GRAPH_ID(-1);
	m_fGoingSpeed					= 0.f;
	m_dwTimeToChange				= 0;
	
	m_dwLastRangeSearch				= 0;

	// Инициализация параметров анимации
	m_tAnim							= m_tAnimPrevFrame = DEFAULT_ANIM;

	m_tPathState					= PATH_STATE_SEARCH_NODE;

	m_fAttackSuccessProbability[0]	= .8f;
	m_fAttackSuccessProbability[1]	= .6f;
	m_fAttackSuccessProbability[2]	= .4f;
	m_fAttackSuccessProbability[3]	= .2f;

	bShowDeath						= false;
	
	InitMemory						(10000,10000);

	Motion.Init						(this);

	m_dwPathBuiltLastTime			= 0;
	
	AI_Path.TravelPath.clear		();
	AI_Path.Nodes.clear				();
	AI_Path.TravelStart				= 0;
	AI_Path.DestNode				= u32(-1);
	m_pPhysics_support				->in_Init();

	m_tAttackAnim.Clear				();

	m_dwAnimStarted					= 0;
	anim_i3							= 0;

	m_tLockedAnims.clear			();
	m_tAnimPlaying					= DEFAULT_ANIM;
}

void CAI_Biting::Die()
{
	inherited::Die( );

	DeinitMemory();

	Fvector	dir;
	
	bShowDeath = true;
	SelectAnimation(dir,dir,0.f);

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

	m_ftrStandTurnRSpeed			= pSettings->r_float(section,"StandTurnRSpeed");
	m_ftrWalkSpeed					= pSettings->r_float(section,"WalkSpeed");
	m_ftrWalkTurningSpeed			= pSettings->r_float(section,"WalkTurningSpeed");
	m_ftrWalkRSpeed					= pSettings->r_float(section,"WalkRSpeed");
	m_ftrWalkTurnRSpeed				= pSettings->r_float(section,"WalkTurnRSpeed");
	m_ftrWalkMinAngle				= pSettings->r_float(section,"WalkMinAngle");
	m_ftrRunAttackSpeed				= pSettings->r_float(section,"RunAttackSpeed");
	m_ftrRunAttackTurnSpeed			= pSettings->r_float(section,"RunAttackTurnSpeed");
	m_ftrRunAttackTurnRSpeed		= pSettings->r_float(section,"RunAttackTurnRSpeed");
	m_ftrRunRSpeed					= pSettings->r_float(section,"RunRSpeed");
	m_ftrRunAttackMinAngle			= pSettings->r_float(section,"RunAttackMinAngle");
	m_ftrAttackFastRSpeed			= pSettings->r_float(section,"AttackFastRSpeed");
	m_ftrAttackFastRSpeed2			= pSettings->r_float(section,"AttackFastRSpeed2");
	m_ftrScaredRSpeed				= pSettings->r_float(section,"ScaredRSpeed");

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
	m_pPhysics_support				->in_Load(section);
	R_ASSERT2 ((m_dwProbRestWalkFree + m_dwProbRestStandIdle + m_dwProbRestLieIdle + m_dwProbRestTurnLeft) == 100, "Probability sum isn't 1");

	LoadAttackAnim					();

}

BOOL CAI_Biting::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterBiting			*l_tpSE_Biting	= dynamic_cast<CSE_ALifeMonsterBiting*>(e);
	
	r_current.yaw = r_target.yaw = r_torso_current.yaw = r_torso_target.yaw	= angle_normalize_signed(-l_tpSE_Biting->o_Angle.y);

	cNameVisual_set					(l_tpSE_Biting->get_visual());
	
	m_tNextGP						= m_tCurGP = getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
	
	// loading animations
	CBitingAnimations::Load			(PKinematics(Visual()));

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
	CheckAttackHit();

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
}


