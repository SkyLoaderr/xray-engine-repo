////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

using namespace AI_Biting;

CAI_Biting::CAI_Biting()
{
	//shedule.t_min	=	50;
	//shedule.t_max	=	150;
	
	Movement.AllocateCharacterObject(CPHMovementControl::CharacterType::ai_stalker);

	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this);
	stateEat			= xr_new<CBitingEat>		(this);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this);
	stateExploreDE		= xr_new<CBitingExploreDE>	(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateFindEnemy		= xr_new<CFindEnemy>		(this);
	CurrentState		= stateRest;

	Init();
}

CAI_Biting::~CAI_Biting()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreDNE);
	xr_delete(stateExploreDE);
	xr_delete(stateExploreNDE);
	xr_delete(stateFindEnemy);

	DELETE_SOUNDS			(SND_HIT_COUNT,	m_tpaSoundHit);
	DELETE_SOUNDS			(SND_DIE_COUNT,	m_tpaSoundDie);
	DELETE_SOUNDS			(SND_ATTACK_COUNT,	m_tpaSoundDie);
	DELETE_SOUNDS			(SND_VOICE_COUNT, m_tpaSoundVoice);
}

void CAI_Biting::Init()
{
	// initializing class members
	m_tpCurrentGlobalAnimation		= 0;
	m_tCurGP						= _GRAPH_ID(-1);
	m_tNextGP						= _GRAPH_ID(-1);
	m_fGoingSpeed					= 0.f;
	m_dwTimeToChange				= 0;
	
	m_dwLastRangeSearch				= 0;

	// »нициализаци€ параметров анимации
	m_tAnim							= DEFAULT_ANIM;

	m_tPathType						= ePathTypeStraight;
	m_tPathState					= ePathStateSearchNode;

	m_fAttackSuccessProbability0	= .8f;
	m_fAttackSuccessProbability1	= .6f;
	m_fAttackSuccessProbability2	= .4f;
	m_fAttackSuccessProbability3	= .2f;

	m_dwLostEnemyTime				= 0;

	m_dwActionStartTime				= 0;

	m_dwAnimFrameDelay				= 100;
	m_dwAnimLastSetTime				= 0;
	m_bActionFinished				= true;

	bPlayDeath						= false;
	bStartPlayDeath					= false;
	bTurning						= false;
	m_tpSoundBeingPlayed			= 0;
	
	
	bShowDeath						= false;
	
	m_dwEatInterval					= 500;
	m_dwLastTimeEat					= 0;

	m_dwLieIndex					= 0;
	

	m_dwPointCheckLastTime			= 0;
	m_dwPointCheckInterval			= 1500;
	
	m_dwActionIndex					= 0;

	m_AttackLastTime				= 0;			
	m_AttackInterval				= 500;
	m_AttackLastPosition.set		(0,0,0);		

	InitMemory						(10000,10000);

	m_dwAttackMeleeTime				= 0;
	m_dwAttackActorMeleeTime		= 0;

	Motion.Init						();

	m_dwPathBuiltLastTime			= 0;

	m_fEyeShiftYaw					= PI_DIV_6;
	ZeroMemory						(&m_tAttack,sizeof(m_tAttack));

	CurrentState					= stateRest;
	CurrentState->Reset				();
	
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

	::Sound->play_at_pos(m_tpaSoundDie[::Random.randI(SND_DIE_COUNT)],this,eye_matrix.c);
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
	m_tSelectorRetreat.Load			(section,"selector_retreat");
	m_tSelectorCover.Load			(section,"selector_cover");

	// loading frustum parameters
	eye_fov							= pSettings->r_float(section,"EyeFov");
	eye_range						= pSettings->r_float(section,"EyeRange");
	m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	m_bCannibalism					= pSettings->r_bool  (section,"Cannibalism");
	m_bEatMemberCorpses				= pSettings->r_bool  (section,"EatMemberCorpses");
	m_dwEatCorpseInterval			= pSettings->r_s32   (section,"EatCorpseInterval");

	m_dwHealth						= pSettings->r_u32   (section,"Health");
	m_fHitPower						= pSettings->r_float (section,"HitPower");
	// temp
	//m_fHitPower						= 1.f;
	fHealth							= (float)m_dwHealth;


	vfLoadSounds();

	m_fMinVoiceIinterval			= pSettings->r_float (section,"MinVoiceInterval");
	m_fMaxVoiceIinterval			= pSettings->r_float (section,"MaxVoiceInterval");
	m_fVoiceRefreshRate				= pSettings->r_float (section,"VoiceRefreshRate");

	vfSetFireBones				(pSettings,section);

	// prefetching
	cNameVisual_set					("monsters\\flesh\\flesh_ik1");
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
	SetText();
	inherited::UpdateCL();

	if(m_pPhysicsShell&&m_pPhysicsShell->bActive&&!m_pPhysicsShell->bActivating)
	{

		//XFORM().set(m_pPhysicsShell->mXFORM);
		m_pPhysicsShell->InterpolateGlobalPosition(&(XFORM().c));
	}
	// ѕроверка состо€ни€ анимации (атака)
	TTime cur_time = Level().timeServer();
	
	VisionElem ve;
	if (!GetEnemy(ve)) return;
	CObject *obj = dynamic_cast<CObject *>(ve.obj);


	if (m_tAttack.time_started != 0) {

		if ((m_tAttack.time_started + m_tAttack.time_from < cur_time) && 
			(m_tAttack.time_started + m_tAttack.time_to > cur_time) && 
			(m_tAttack.LastAttack + 1000 < cur_time)) {

			// трассировка нужна?
			if (m_tAttack.b_fire_anyway) {
				DoDamage(ve.obj); // не нужна
				m_tAttack.LastAttack = cur_time;
			}
			else if (m_tAttack.b_attack_rat) {
				
				// TestIntersection конуса(копыта) и сферы(крысы)
				bool Intersected = false;

				float angle = PI_DIV_6;					// угол конуса
				Fvector fromV = m_tAttack.TraceFrom;	// вершина конуса
				Fvector dir;							// направление конуса
				dir.set(0.f,-1.f,0.f);

				float fInvSin = 1.0f/_sin(angle);
				float fCosSqr = _cos(angle)*_cos(angle);

				Fvector vC;		ve.obj->Center(vC);		// центр сферы
				Fvector kCmV;	kCmV.sub(vC,fromV);
				Fvector kD		= kCmV;
				Fvector tempV	= dir;
				tempV.mul(ve.obj->Radius()* fInvSin);
				kD.add(tempV);

				float fDSqrLen = kD.square_magnitude();
				float fE = kD.dotproduct(dir);
				if ( fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr )
				{
					float fSinSqr = _sin(angle)*_sin(angle);

					fDSqrLen = kCmV.square_magnitude();
					fE = -kCmV.dotproduct(dir);
					if ( fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr )
					{
						float fRSqr = ve.obj->Radius()*ve.obj->Radius();
						Intersected =  fDSqrLen <= fRSqr;
					}else Intersected = true;
				} else Intersected = false;
					
				if (Intersected) {
					DoDamage(ve.obj);
					m_tAttack.LastAttack = cur_time;
				}

			} else 	{ // нужна
				this->setEnabled(false);
				Collide::ray_query	l_rq;
				
				if (Level().ObjectSpace.RayPick(m_tAttack.TraceFrom, Direction(), m_tAttack.dist, l_rq)) {
					if ((l_rq.O == obj) && (l_rq.range < m_tAttack.dist)) {
						DoDamage(ve.obj);
						m_tAttack.LastAttack = cur_time;
					}
				}

				this->setEnabled(true);			
			}

			if (!ve.obj->g_Alive()) AddCorpse(ve);
		}
	}


}

// Load sounds
void CAI_Biting::vfLoadSounds()
{
	::Sound->create(m_tpaSoundHit[0],TRUE,"monsters\\flesh\\test_1",SOUND_TYPE_MONSTER_INJURING_ANIMAL);
	::Sound->create(m_tpaSoundDie[0],TRUE,"monsters\\flesh\\test_0",SOUND_TYPE_MONSTER_DYING_ANIMAL);
	::Sound->create(m_tpaSoundAttack[0],TRUE,"monsters\\flesh\\test_2",SOUND_TYPE_MONSTER_ATTACKING_ANIMAL);
	::Sound->create(m_tpaSoundVoice[0],TRUE,"monsters\\flesh\\test_3",SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->create(m_tpaSoundVoice[1],TRUE,"monsters\\flesh\\test_3",SOUND_TYPE_MONSTER_TALKING_ANIMAL);
}

void CAI_Biting::vfSetFireBones(CInifile *ini, const char *section)
{
	m_iLeftFireBone = PKinematics(Visual())->LL_BoneID(ini->r_string(section,"LeftFireBone"));
	m_iRightFireBone = PKinematics(Visual())->LL_BoneID(ini->r_string(section,"RightFireBone"));
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