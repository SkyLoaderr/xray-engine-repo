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
	inherited::Die( );
	m_eCurrentState = aiDogDie;
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	
	::Sound->play_at_pos(m_tpaSoundDie[Random.randI(SND_DIE_COUNT)],this,vPosition);

	vfUpdateMoraleBroadcast(m_fMoraleDeathQuant,m_fMoraleDeathDistance);
	
	CGroup &Group = Level().get_group(g_Team(),g_Squad(),g_Group());
	vfRemoveActiveMember();
	vfRemoveStandingMember();
	Group.m_dwAliveCount--;
	m_eCurrentState = aiDogDie;
	m_dwDeathTime = Level().timeServer();
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
	m_fMinVoiceIinterval			= pSettings->r_float (section,"MinVoiceInterval");
	m_fMaxVoiceIinterval			= pSettings->r_float (section,"MaxVoiceInterval");
	m_fVoiceRefreshDoge				= pSettings->r_float (section,"VoiceRefreshDoge");
	
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
	m_dwActionRefreshDoge	 		= pSettings->r_s32   (section,"ActionRefreshDoge");

	m_fMaxHealthValue	 			= pSettings->r_float (section,"MaxHealthValue");
	m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	m_bEatMemberCorpses				= pSettings->r_bool  (section,"EatMemberCorpses");
	m_bCannibalism					= pSettings->r_bool  (section,"Cannibalism");
	m_dwEatCorpseInterval			= pSettings->r_s32   (section,"EatCorpseInterval");

	m_dwActiveScheduleMin			= shedule.t_min;
	m_dwActiveScheduleMax			= shedule.t_max;
}

BOOL CAI_Dog::net_Spawn	(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))	return FALSE;
	
	//////////////////////////////////////////////////////////////////////////
	CAbstractServerObject					*e	= (CAbstractServerObject*)(DC);
	CALifeMonsterDog						*tpSE_Dog	= dynamic_cast<CALifeMonsterDog*>(e);
	// model
	cNameVisual_set					(tpSE_Dog->get_visual());
	// personal characteristics
	r_torso_current.yaw				= r_torso_target.yaw	= -tpSE_Dog->o_Angle.y;
	r_torso_current.pitch			= r_torso_target.pitch	= 0;

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
	m_tStateStack.push(m_eCurrentState = aiDogFreeHuntingActive);
	vfAddActiveMember(true);
	m_bStateChanged = true;

	r_torso_current = r_current;
	r_torso_target = r_target;
	m_tHPB.x = r_torso_current.yaw;
	m_tHPB.y = r_torso_current.pitch;
	m_tHPB.z = 0;
	
	vfLoadAnimations();

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
	P.w_float_q16		(fHealth,-1000,1000);
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
	P.r_float_q16		(fHealth,-1000,1000);
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
