////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

CAI_Biting::CAI_Biting()
{
	Movement.AllocateCharacterObject(CPHMovementControl::CharacterType::ai_stalker);
}

CAI_Biting::~CAI_Biting()
{
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

	m_fAttackSuccessProbability0	= .8f;
	m_fAttackSuccessProbability1	= .6f;
	m_fAttackSuccessProbability2	= .4f;
	m_fAttackSuccessProbability3	= .2f;

	bShowDeath						= false;
	
	InitMemory						(10000,10000);


	Motion.Init						();

	m_dwPathBuiltLastTime			= 0;
	ZeroMemory						(&m_tAttack,sizeof(m_tAttack));
	
	AI_Path.TravelPath.clear		();
	AI_Path.Nodes.clear				();
	AI_Path.TravelStart				= 0;
	AI_Path.DestNode				= u32(-1);

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
	eye_range						= pSettings->r_float(section,"EyeRange");
	m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	m_dwHealth						= pSettings->r_u32   (section,"Health");
	m_fHitPower						= pSettings->r_float (section,"HitPower");
	// temp
	//m_fHitPower						= 1.f;
	fHealth							= (float)m_dwHealth;

	// prefetching
	cNameVisual_set					(pSettings->r_string(section,"visual"));

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

#ifndef NO_PHYSICS_IN_AI_MOVE
	Movement.CreateCharacter();
	Movement.SetPhysicsRefObject(this);
#endif
	Movement.SetPosition	(Position());
	Movement.SetVelocity	(0,0,0);

	return(TRUE);
}

void CAI_Biting::net_Destroy()
{
	inherited::net_Destroy();
	Init();
	Movement.DestroyCharacter();
	if(m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		m_pPhysicsShell->ZeroCallbacks();
	}
	xr_delete(m_pPhysicsShell);
}

void CAI_Biting::net_Export(NET_Packet& P) 
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_float_q16			(fHealth,-1000,1000);
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
	P.r_float_q16			(fHealth,-1000,1000);
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
	//SetText();
	inherited::UpdateCL();

	if(m_pPhysicsShell&&m_pPhysicsShell->bActive&&!m_pPhysicsShell->bActivating)
	{
		//XFORM().set(m_pPhysicsShell->mXFORM);
		m_pPhysicsShell->InterpolateGlobalTransform(&(XFORM()));
	}
}

void CAI_Biting::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	////physics/////////////////////////////////////////////////////////////////////////////////////
	if(m_pPhysicsShell)
	{	
		if(m_pPhysicsShell->bActive)
		{

		//	if(m_saved_impulse!=0.f)
		//	{
			//	m_pPhysicsShell->applyImpulseTrace(m_saved_hit_position,m_saved_hit_dir,m_saved_impulse*1.f,m_saved_element);
			//	m_saved_impulse=0.f;
		//	}


		//	if(skel_ddelay==0)
		//	{
		//		m_pPhysicsShell->set_JointResistance(5.f*hinge_force_factor1);//5.f*hinge_force_factor1
				//m_pPhysicsShell->SetAirResistance()

		//	}
			//if(skel_ddelay==-10)
			//{
			//m_pPhysicsShell->set_JointResistance(5.f*hinge_force_factor1);//5.f*hinge_force_factor1
			//m_pPhysicsShell->SetAirResistance()

			//}

		//	skel_ddelay--;



		}

	}
	else if (!g_Alive())
	{

		CreateSkeleton();
#ifndef NO_PHYSICS_IN_AI_MOVE

		Movement.DestroyCharacter();
		PHSetPushOut();
#endif
	}

}

void CAI_Biting::CreateSkeleton()
{
	if(m_pPhysicsShell) return;
#ifndef NO_PHYSICS_IN_AI_MOVE
	Movement.GetDeathPosition	(Position());
	//Position().y+=.1f;
	//#else
	//Position().y+=0.1f;
#endif

	if (!Visual())
		return;
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(PKinematics(Visual()));
	m_pPhysicsShell->mXFORM.set(XFORM());
	//m_pPhysicsShell->SetAirResistance(0.002f*skel_airr_lin_factor,
	//	0.3f*skel_airr_ang_factor);
	m_pPhysicsShell->SmoothElementsInertia(0.3f);
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->Activate(true);
	PKinematics(Visual())->Calculate();
}