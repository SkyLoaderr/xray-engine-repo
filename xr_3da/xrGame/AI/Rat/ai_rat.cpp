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
#include "..\\..\\PhysicsShell.h"
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
	inherited::Die( );
	m_eCurrentState = aiRatDie;

	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(XFORM().k,dir,AI_Path.fSpeed);

	::Sound->play_at_pos(m_tpaSoundDie[Random.randI(SND_DIE_COUNT)],this,Position());

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
	IRender_Visual*	temp;
	temp = ::Render->model_Create("aaa");	::Render->model_Delete(temp);
	temp = ::Render->model_Create("bbb");	::Render->model_Delete(temp);
	temp = ::Render->model_Create("ccc");	::Render->model_Delete(temp);
	*/

	// initialize start position
	Fvector	P						= Position();
	P.x								+= ::Random.randF();
	P.z								+= ::Random.randF();

	vfLoadSounds();

	// sounds
	m_fMinVoiceIinterval			= pSettings->r_float (section,"MinVoiceInterval");
	m_fMaxVoiceIinterval			= pSettings->r_float (section,"MaxVoiceInterval");
	m_fVoiceRefreshRate				= pSettings->r_float (section,"VoiceRefreshRate");

	// active\passive
	m_fChangeActiveStateProbability = pSettings->r_float (section,"ChangeActiveStateProbability");
	m_dwPassiveScheduleMin			= pSettings->r_s32   (section,"PassiveScheduleMin");
	m_dwPassiveScheduleMax			= pSettings->r_s32   (section,"PassiveScheduleMax");
	m_dwActiveCountPercent			= pSettings->r_s32   (section,"ActiveCountPercent");
	m_dwStandingCountPercent		= pSettings->r_s32   (section,"StandingCountPercent");

	// eye shift
	m_tEyeShift.y					= pSettings->r_float (section,"EyeYShift");

	// former constants
	m_dwLostMemoryTime				= pSettings->r_s32   (section,"LostMemoryTime");
	m_dwLostRecoilTime				= pSettings->r_s32   (section,"LostRecoilTime");
	m_fUnderFireDistance			= pSettings->r_float (section,"UnderFireDistance");
	m_dwRetreatTime					= pSettings->r_s32   (section,"RetreatTime");
	m_fRetreatDistance				= pSettings->r_float (section,"RetreatDistance");
	m_fAttackStraightDistance		= pSettings->r_float (section,"AttackStraightDistance");
	m_fStableDistance				= pSettings->r_float (section,"StableDistance");
	m_fWallMinTurnValue				= pSettings->r_float (section,"WallMinTurnValue")/180.f*PI;
	m_fWallMaxTurnValue				= pSettings->r_float (section,"WallMaxTurnValue")/180.f*PI;

	m_fAngleSpeed					= pSettings->r_float (section,"AngleSpeed");
	m_fSafeGoalChangeDelta	 		= pSettings->r_float (section,"GoalChangeDelta");
	m_tGoalVariation	  			= pSettings->r_fvector3(section,"GoalVariation");

	m_fMoraleDeathDistance	 		= pSettings->r_float (section,"MoraleDeathDistance");
	m_dwActionRefreshRate	 		= pSettings->r_s32   (section,"ActionRefreshRate");

	m_fMaxHealthValue	 			= pSettings->r_float (section,"MaxHealthValue");
	m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	m_bEatMemberCorpses				= pSettings->r_bool  (section,"EatMemberCorpses");
	m_bCannibalism					= pSettings->r_bool  (section,"Cannibalism");
	m_dwEatCorpseInterval			= pSettings->r_s32   (section,"EatCorpseInterval");

	m_fNullASpeed					= pSettings->r_float (section,"AngularStandSpeed")/180.f*PI;//PI_MUL_2
	m_fMinASpeed					= pSettings->r_float (section,"AngularMinSpeed")/180.f*PI;//PI_MUL_2
	m_fMaxASpeed					= pSettings->r_float (section,"AngularMaxSpeed")/180.f*PI;//.2f
	m_fAttackASpeed					= pSettings->r_float (section,"AngularAttackSpeed")/180.f*PI;//.15f;

	m_phMass						= pSettings->r_float (section,"corp_mass");
	m_dwActiveScheduleMin			= shedule.t_min;
	m_dwActiveScheduleMax			= shedule.t_max;

	m_tpaTerrain.clear				();
	LPCSTR							S = pSettings->r_string(section,"terrain");
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
	m_fGoingSpeed					= pSettings->r_float	(section, "going_speed");

	// prefetching
	cNameVisual_set					("monsters\\rat\\rat_1");
	cNameVisual_set					("monsters\\rat\\rat_2");
	cNameVisual_set					("monsters\\rat\\rat_3");
}

BOOL CAI_Rat::net_Spawn	(LPVOID DC)
{
	//////////////////////////////////////////////////////////////////////////
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterRat						*tpSE_Rat = dynamic_cast<CSE_ALifeMonsterRat*>(e);
	// model
	cNameVisual_set					(tpSE_Rat->get_visual());
	if (!inherited::net_Spawn(DC))
		return(FALSE);
	// personal characteristics
	r_torso_current.yaw				= r_torso_target.yaw	= -tpSE_Rat->o_Angle.y;
	r_torso_current.pitch			= r_torso_target.pitch	= 0;

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

	m_tOldPosition.set				(Position());
	m_tSpawnPosition.set			(Level().get_squad(g_Team(),g_Squad()).Leader->Position());
	m_tSafeSpawnPosition.set		(m_tSpawnPosition);
	m_tStateStack.push				(m_eCurrentState = aiRatFreeHuntingActive);
	vfAddActiveMember				(true);
	m_bStateChanged					= true;
	m_tCurGP						= m_tNextGP = getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;

	r_torso_current					= r_current;
	r_torso_target					= r_target;
	m_tHPB.x						= r_torso_current.yaw;
	m_tHPB.y						= r_torso_current.pitch;
	m_tHPB.z						= 0;

	INIT_SQUAD_AND_LEADER;

	vfLoadAnimations				();

	getGroup()->m_dwLastActionTime		= 0;
	return							(TRUE);
}

void CAI_Rat::net_Destroy()
{
	inherited::net_Destroy();

	if (m_pPhysicsShell) m_pPhysicsShell->Deactivate();
}

void CAI_Rat::Exec_Movement	( float dt )
{
}

void CAI_Rat::net_Export(NET_Packet& P)
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

void CAI_Rat::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;
	P.r_float_q16		(fHealth,-1000,1000);
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
void CAI_Rat::CreateSkeleton(){

	if (!Visual())
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
	element->setDensity(m_phMass);
	element->SetMaterial("creatures\\rat");
	m_pPhysicsShell=P_create_Shell();
	m_pPhysicsShell->add_Element(element);
	m_pPhysicsShell->Activate(XFORM(),0,XFORM());
	if(m_saved_impulse!=0.f){

		m_pPhysicsShell->applyImpulseTrace(m_saved_hit_position,m_saved_hit_dir,m_saved_impulse);
	}
	/*
	CKinematics* M		= PKinematics(Visual());			VERIFY(M);
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

	element->setDensity(200.f);
	m_pPhysicsShell->add_Element(element);
	element->SetMaterial("materials\\skel1");

	//set shell start position
	Fmatrix m;
	m.set(mRotate);
	m.c.set(Position());
	m_pPhysicsShell->mXFORM.set(m);
	*/
}

void CAI_Rat::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);
	if(m_pPhysicsShell)
	{
		m_pPhysicsShell->Update	();
		XFORM().set				(m_pPhysicsShell->mXFORM);
	}
}

void CAI_Rat::UpdateCL(){

	inherited::UpdateCL();
	if(m_pPhysicsShell){
		m_pPhysicsShell->Update();
		XFORM().set(m_pPhysicsShell->mXFORM);

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

void CAI_Rat::feel_touch_new(CObject* O)
{
}
