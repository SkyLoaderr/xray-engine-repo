////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_dog.cpp
//	Created 	: 23.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Dog"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_dog.h"
#include "..\\ai_monsters_misc.h"

CAI_Dog::CAI_Dog()
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
}

CAI_Dog::~CAI_Dog()
{
	DELETE_SOUNDS(SND_HIT_COUNT,	m_tpaSoundHit);
	DELETE_SOUNDS(SND_DIE_COUNT,	m_tpaSoundDie);
	DELETE_SOUNDS(SND_VOICE_COUNT,	m_tpaSoundVoice);
}

void CAI_Dog::Die()
{
	inherited::Death( );
	eCurrentState = aiDogDie;
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	
	::Sound->PlayAtPos(m_tpaSoundDie[Random.randI(SND_DIE_COUNT)],this,vPosition);

	vfUpdateMoraleBroadcast(m_fMoraleDeathQuant,m_fMoraleDeathDistance);
	
	CGroup &Group = Level().get_group(g_Team(),g_Squad(),g_Group());
	vfRemoveActiveMember();
	vfRemoveStandingMember();
	Group.m_dwAliveCount--;
	eCurrentState = aiDogDie;
	m_dwDeathTime = Level().timeServer();
}

void CAI_Dog::OnDeviceCreate()
{
	inherited::OnDeviceCreate();
	vfLoadAnimations();
}

void CAI_Dog::Load(LPCSTR section)
{ 
	// load parameters from ".ini" file
	inherited::Load(section);
	
	// initialize start position
	Fvector	P						= vPosition;
	P.x								+= ::Random.randF();
	P.z								+= ::Random.randF();
	
	vfLoadSounds();
	
	// sounds
	m_fMinVoiceIinterval			= pSettings->ReadFLOAT (section,"MinVoiceInterval");
	m_fMaxVoiceIinterval			= pSettings->ReadFLOAT (section,"MaxVoiceInterval");
	m_fVoiceRefreshDoge				= pSettings->ReadFLOAT (section,"VoiceRefreshDoge");
	
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
	m_dwActionRefreshDoge	 		= pSettings->ReadINT   (section,"ActionRefreshDoge");

	m_fMaxHealthValue	 			= pSettings->ReadFLOAT (section,"MaxHealthValue");
	m_fSoundThreshold				= pSettings->ReadFLOAT (section,"SoundThreshold");

	m_bEatMemberCorpses				= pSettings->ReadBOOL  (section,"EatMemberCorpses");
	m_bCannibalism					= pSettings->ReadBOOL  (section,"Cannibalism");
	m_dwEatCorpseInterval			= pSettings->ReadINT   (section,"EatCorpseInterval");

	m_dwActiveScheduleMin			= shedule_Min;
	m_dwActiveScheduleMax			= shedule_Max;
}

BOOL CAI_Dog::net_Spawn	(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))	return FALSE;
	
	//////////////////////////////////////////////////////////////////////////
	xrSE_Dog *tpSE_Dog = (xrSE_Dog *)DC;
	// model
	cNameVisual_set					(tpSE_Dog->caModel);
	// personal characteristics
	eye_fov							= tpSE_Dog->fEyeFov;
	eye_range						= tpSE_Dog->fEyeRange;
	fHealth							= tpSE_Dog->fHealth;
	m_fMinSpeed						= tpSE_Dog->fMinSpeed;
	m_fMaxSpeed						= tpSE_Dog->fMaxSpeed;
	m_fAttackSpeed					= tpSE_Dog->fAttackSpeed;
	m_fMaxPursuitRadius				= tpSE_Dog->fMaxPursuitRadius;
	m_fMaxHomeRadius				= tpSE_Dog->fMaxHomeRadius;
	// morale
	m_fMoraleSuccessAttackQuant		= tpSE_Dog->fMoraleSuccessAttackQuant;
	m_fMoraleDeathQuant				= tpSE_Dog->fMoraleDeathQuant;
	m_fMoraleFearQuant				= tpSE_Dog->fMoraleFearQuant;
	m_fMoraleRestoreQuant			= tpSE_Dog->fMoraleRestoreQuant;
	m_dwMoraleRestoreTimeInterval	= tpSE_Dog->u16MoraleRestoreTimeInterval;
	m_fMoraleMinValue				= tpSE_Dog->fMoraleMinValue;
	m_fMoraleMaxValue				= tpSE_Dog->fMoraleMaxValue;
	m_fMoraleNormalValue			= tpSE_Dog->fMoraleNormalValue;
	// attack
	m_fHitPower						= tpSE_Dog->fHitPower;
	m_dwHitInterval					= tpSE_Dog->u16HitInterval;
	m_fAttackDistance				= tpSE_Dog->fAttackDistance;
	m_fAttackAngle					= tpSE_Dog->fAttackAngle/180.f*PI;
	m_fAttackSuccessProbability		= tpSE_Dog->fAttackSuccessProbability;
	//////////////////////////////////////////////////////////////////////////

	m_fCurSpeed						= m_fMaxSpeed;

	m_tOldPosition.set(vPosition);
	m_tSpawnPosition.set(Level().get_squad(g_Team(),g_Squad()).Leader->Position());
	m_tSafeSpawnPosition.set(m_tSpawnPosition);
	tStateStack.push(eCurrentState = aiDogFreeHuntingActive);
	vfAddActiveMember(true);
	m_bStateChanged = true;

	r_torso_current = r_current;
	r_torso_target = r_target;
	m_tHPB.x = r_torso_current.yaw;
	m_tHPB.y = r_torso_current.pitch;
	m_tHPB.z = 0;
	
	return TRUE;
}

void CAI_Dog::Exec_Movement	( float dt )
{
	AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
}

void CAI_Dog::net_Export(NET_Packet& P)
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
}

void CAI_Dog::net_Import(NET_Packet& P)
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

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible				(TRUE);
	setEnabled				(TRUE);
}
