////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.cpp
//	Created 	: 23.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_rat.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\..\\xr_trims.h"

CAI_Rat::CAI_Rat()
{
	m_tHitDir.set			(0,0,1);
	m_tSavedEnemyPosition.set(0,0,0);
	m_dwHitTime				= 0;
	m_tSavedEnemy			= 0;
	m_tpSavedEnemyNode		= 0;
	m_dwSavedEnemyNodeID	= u32(-1);
	m_dwLostEnemyTime		= 0;
	m_tpCurrentGlobalAnimation = 0;
	m_tpCurrentGlobalBlend	= 0;
	m_bActionStarted		= false;
	m_bFiring				= false;
	m_dwLastVoiceTalk		= 0;
	m_tpSoundBeingPlayed	= 0;
	m_dwLastSoundRefresh	= 0;
	m_dwLastRangeSearch		= 0;
	m_tGoalDir.set			(10.0f*(Random.randF()-Random.randF()),10.0f*(Random.randF()-Random.randF()),10.0f*(Random.randF()-Random.randF()));
	m_tCurrentDir.set		(0,0,1);
	m_tHPB.set				(0,0,0);
	m_fDHeading				= 0;
	m_fGoalChangeTime		= 0.f;
	m_tLastSound.tpEntity	= 0;
	m_tLastSound.dwTime		= 0;
	m_tLastSound.eSoundType	= SOUND_TYPE_NO_SOUND;
	m_bNoWay				= false;
	m_dwMoraleLastUpdateTime = 0;
	m_bStanding				= false;
	m_bActive				= false;
	m_dwStartAttackTime		= 0;
	q_look.o_look_speed		= PI;
	m_pPhysicsShell			= NULL;
	m_saved_impulse			= 0.f;
	m_dwTimeToChange		= 30000;
}

CAI_Rat::~CAI_Rat()
{
	DELETE_SOUNDS			(SND_HIT_COUNT,	m_tpaSoundHit);
	DELETE_SOUNDS			(SND_DIE_COUNT,	m_tpaSoundDie);
	DELETE_SOUNDS			(SND_VOICE_COUNT,	m_tpaSoundVoice);
	xr_delete				(m_pPhysicsShell);
}

void CAI_Rat::Die()
{
	inherited::Death( );
	m_eCurrentState = aiRatDie;
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	
	::Sound->PlayAtPos(m_tpaSoundDie[Random.randI(SND_DIE_COUNT)],this,vPosition);

	vfUpdateMoraleBroadcast(m_fMoraleDeathQuant,m_fMoraleDeathDistance);
	
	CGroup &Group = Level().get_group(g_Team(),g_Squad(),g_Group());
	vfRemoveActiveMember();
	vfRemoveStandingMember();
	Group.m_dwAliveCount--;
	m_eCurrentState = aiRatDie;
	m_dwDeathTime = Level().timeServer();
}

void CAI_Rat::Load(LPCSTR section)
{ 
	// load parameters from ".ini" file
	inherited::Load(section);

	/*
	CVisual*	temp;
	temp = ::Render->model_Create("aaa");	::Render->model_Delete(temp);
	temp = ::Render->model_Create("bbb");	::Render->model_Delete(temp);
	temp = ::Render->model_Create("ccc");	::Render->model_Delete(temp);
	*/

	// initialize start position
	Fvector	P						= vPosition;
	P.x								+= ::Random.randF();
	P.z								+= ::Random.randF();
	
	vfLoadSounds();
	
	// sounds
	m_fMinVoiceIinterval			= pSettings->ReadFLOAT (section,"MinVoiceInterval");
	m_fMaxVoiceIinterval			= pSettings->ReadFLOAT (section,"MaxVoiceInterval");
	m_fVoiceRefreshRate				= pSettings->ReadFLOAT (section,"VoiceRefreshRate");
	
	// active\passive
	m_fChangeActiveStateProbability = pSettings->ReadFLOAT (section,"ChangeActiveStateProbability");
	m_dwPassiveScheduleMin			= pSettings->ReadINT   (section,"PassiveScheduleMin");
	m_dwPassiveScheduleMax			= pSettings->ReadINT   (section,"PassiveScheduleMax");
	m_dwActiveCountPercent			= pSettings->ReadINT   (section,"ActiveCountPercent");
	m_dwStandingCountPercent		= pSettings->ReadINT   (section,"StandingCountPercent");

	// eye shift
	m_tEyeShift.y					= pSettings->ReadFLOAT (section,"EyeYShift");

	// former constants
	m_dwLostMemoryTime				= pSettings->ReadINT   (section,"LostMemoryTime");
	m_dwLostRecoilTime				= pSettings->ReadINT   (section,"LostRecoilTime");
	m_fUnderFireDistance			= pSettings->ReadFLOAT (section,"UnderFireDistance");
	m_dwRetreatTime					= pSettings->ReadINT   (section,"RetreatTime");
	m_fRetreatDistance				= pSettings->ReadFLOAT (section,"RetreatDistance");
	m_fAttackStraightDistance		= pSettings->ReadFLOAT (section,"AttackStraightDistance");
	m_fStableDistance				= pSettings->ReadFLOAT (section,"StableDistance");
	m_fWallMinTurnValue				= pSettings->ReadFLOAT (section,"WallMinTurnValue")/180.f*PI;
	m_fWallMaxTurnValue				= pSettings->ReadFLOAT (section,"WallMaxTurnValue")/180.f*PI;

	m_fAngleSpeed					= pSettings->ReadFLOAT (section,"AngleSpeed");
	m_fSafeGoalChangeDelta	 		= pSettings->ReadFLOAT (section,"GoalChangeDelta");
	m_tGoalVariation	  			= pSettings->ReadVECTOR(section,"GoalVariation");

	m_fMoraleDeathDistance	 		= pSettings->ReadFLOAT (section,"MoraleDeathDistance");
	m_dwActionRefreshRate	 		= pSettings->ReadINT   (section,"ActionRefreshRate");

	m_fMaxHealthValue	 			= pSettings->ReadFLOAT (section,"MaxHealthValue");
	m_fSoundThreshold				= pSettings->ReadFLOAT (section,"SoundThreshold");

	m_bEatMemberCorpses				= pSettings->ReadBOOL  (section,"EatMemberCorpses");
	m_bCannibalism					= pSettings->ReadBOOL  (section,"Cannibalism");
	m_dwEatCorpseInterval			= pSettings->ReadINT   (section,"EatCorpseInterval");

	m_phMass						= pSettings->ReadFLOAT (section,"corp_mass");
	m_dwActiveScheduleMin			= shedule_Min;
	m_dwActiveScheduleMax			= shedule_Max;

	m_tpaTerrain.clear				();
	LPCSTR							S = pSettings->ReadSTRING(section,"terrain");
	u32								N = _GetItemCount(S);
	R_ASSERT						(((N % (LOCATION_TYPE_COUNT + 2)) == 0) && (N));
	STerrainPlace					tTerrainPlace;
	tTerrainPlace.tMask.resize		(LOCATION_TYPE_COUNT);
	string16						I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; j++, i++)
			tTerrainPlace.tMask[j] = _LOCATION_ID(atoi(_GetItem(S,i,I)));
		tTerrainPlace.dwMinTime		= atoi(_GetItem(S,i++,I))*1000;
		tTerrainPlace.dwMaxTime		= atoi(_GetItem(S,i++,I))*1000;
		m_tpaTerrain.push_back(tTerrainPlace);
	}
	m_fGoingSpeed					= pSettings->ReadFLOAT	(section, "going_speed");
}

BOOL CAI_Rat::net_Spawn	(LPVOID DC)
{
	//////////////////////////////////////////////////////////////////////////
	xrSE_Rat						*tpSE_Rat = (xrSE_Rat *)DC;
	// model
	cNameVisual_set					(tpSE_Rat->caModel);
	if (!inherited::net_Spawn(DC))
		return(FALSE);
	// personal characteristics
	eye_fov							= tpSE_Rat->fEyeFov;
	eye_range						= tpSE_Rat->fEyeRange;
	fHealth							= tpSE_Rat->fHealth;
	m_fMinSpeed						= tpSE_Rat->fMinSpeed;
	m_fMaxSpeed						= tpSE_Rat->fMaxSpeed;
	m_fAttackSpeed					= tpSE_Rat->fAttackSpeed;
	m_fMaxPursuitRadius				= tpSE_Rat->fMaxPursuitRadius;
	m_fMaxHomeRadius				= tpSE_Rat->fMaxHomeRadius;
	// morale
	m_fMoraleSuccessAttackQuant		= tpSE_Rat->fMoraleSuccessAttackQuant;
	m_fMoraleDeathQuant				= tpSE_Rat->fMoraleDeathQuant;
	m_fMoraleFearQuant				= tpSE_Rat->fMoraleFearQuant;
	m_fMoraleRestoreQuant			= tpSE_Rat->fMoraleRestoreQuant;
	m_dwMoraleRestoreTimeInterval	= tpSE_Rat->u16MoraleRestoreTimeInterval;
	m_fMoraleMinValue				= tpSE_Rat->fMoraleMinValue;
	m_fMoraleMaxValue				= tpSE_Rat->fMoraleMaxValue;
	m_fMoraleNormalValue			= tpSE_Rat->fMoraleNormalValue;
	// attack
	m_fHitPower						= tpSE_Rat->fHitPower;
	m_dwHitInterval					= tpSE_Rat->u16HitInterval;
	m_fAttackDistance				= tpSE_Rat->fAttackDistance;
	m_fAttackAngle					= tpSE_Rat->fAttackAngle/180.f*PI;
	m_fAttackSuccessProbability		= tpSE_Rat->fAttackSuccessProbability;
	m_tCurGP						= tpSE_Rat->m_tGraphID;
	m_tNextGP						= tpSE_Rat->m_tNextGraphID;
	//////////////////////////////////////////////////////////////////////////

	m_fCurSpeed						= m_fMaxSpeed;

	m_tOldPosition.set				(vPosition);
	m_tSpawnPosition.set			(Level().get_squad(g_Team(),g_Squad()).Leader->Position());
	m_tSafeSpawnPosition.set		(m_tSpawnPosition);
	m_tStateStack.push				(m_eCurrentState = aiRatFreeHuntingActive);
	vfAddActiveMember				(true);
	m_bStateChanged					= true;

	r_torso_current					= r_current;
	r_torso_target					= r_target;
	m_tHPB.x						= r_torso_current.yaw;
	m_tHPB.y						= r_torso_current.pitch;
	m_tHPB.z						= 0;

	INIT_SQUAD_AND_LEADER;

	vfLoadAnimations				();

	return							(TRUE);
}

void CAI_Rat::Exec_Movement	( float dt )
{
	AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
}

void CAI_Rat::net_Export(NET_Packet& P)
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);
	P.w_float				(N.fHealth);

	P.w						(&m_tNextGP,				sizeof(m_tNextGP));
	P.w						(&m_tCurGP,					sizeof(m_tCurGP));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	float					f1;
	f1						= vPosition.distance_to		(getAI().m_tpaGraph[m_tCurGP].tLocalPoint);
	P.w						(&f1,						sizeof(f1));
	f1						= vPosition.distance_to		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
	P.w						(&f1,						sizeof(f1));
}

void CAI_Rat::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;
	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);
	P.r_float				(N.fHealth);

	P.r						(&m_tNextGP,				sizeof(m_tNextGP));
	P.r						(&m_tCurGP,					sizeof(m_tCurGP));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible				(TRUE);
	setEnabled				(TRUE);
}
void CAI_Rat::CreateSkeleton(){
	
	if (!pVisual)
		return;
	CPhysicsElement* element=P_create_Element();
	Fobb box;
	box.m_rotate.identity();
	box.m_translate.set(0,0.1f,-0.15f);
	box.m_halfsize.set(0.10f,0.085f,0.25f);
	element->add_Box(box);
	//Fsphere sphere;
	//sphere.P.set(0,0,0);
	//sphere.R=0.25;
	//element->add_Sphere(sphere);
	element->setMass(m_phMass);
	element->SetMaterial("creatures\\rat");
	m_pPhysicsShell=P_create_Shell();
	m_pPhysicsShell->add_Element(element);
	m_pPhysicsShell->Activate(svXFORM(),0,svXFORM());
	if(m_saved_impulse!=0.f){

		m_pPhysicsShell->applyImpulseTrace(m_saved_hit_position,m_saved_hit_dir,m_saved_impulse);
	}
	/*
	CKinematics* M		= PKinematics(pVisual);			VERIFY(M);
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->set_Kinematics(M);
	//get bone instance
	int id=M->LL_BoneID("bip01_pelvis");
	CBoneInstance& instance=M->LL_GetInstance				(id);

	//create root element
	CPhysicsElement* element=P_create_Element				();
	element->mXFORM.identity();
	instance.set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	Fobb box;
	box.m_rotate.identity();
	box.m_translate.set(0,0,0);
	box.m_halfsize.set(0.10f,0.085f,0.25f);
	element->add_Box(box);

	element->setMass(200.f);
	m_pPhysicsShell->add_Element(element);
	element->SetMaterial("materials\\skel1");

	//set shell start position
	Fmatrix m;
	m.set(mRotate);
	m.c.set(vPosition);
	m_pPhysicsShell->mXFORM.set(m);
	*/
}

void CAI_Rat::Update(u32 dt){

	inherited::Update( dt);
	if(m_pPhysicsShell){
		m_pPhysicsShell->Update();
		mRotate.set(m_pPhysicsShell->mXFORM);
		mRotate.c.set(0,0,0);
		UpdateTransform					();
		vPosition.set(m_pPhysicsShell->mXFORM.c);
		svTransform.set(m_pPhysicsShell->mXFORM);
		
	//	CKinematics* M		= PKinematics(pVisual);			VERIFY(M);
	//	int id=M->LL_BoneID("bip01_pelvis");
	//	CBoneInstance& instance=M->LL_GetInstance				(id);
	//	instance.mTransform.set(m_pPhysicsShell->mXFORM);
			
	}
	
	
}

void CAI_Rat::UpdateCL(){

	 inherited::UpdateCL();
	if(m_pPhysicsShell){
		m_pPhysicsShell->Update();
		clTransform.set(m_pPhysicsShell->mXFORM);
		
	}
	else
		if (fHealth <= 0)
			CreateSkeleton();
}

void CAI_Rat::Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse){
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
	if(!m_pPhysicsShell){
		m_saved_impulse=impulse;
		m_saved_hit_dir.set(dir);
		m_saved_hit_position.set(p_in_object_space);
	}
}