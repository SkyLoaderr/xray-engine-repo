////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.h
//	Created 	: 23.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_RAT__
#define __XRAY_AI_RAT__

#include "..\\ai_monsters.h"
#include "..\\..\\CustomMonster.h"

namespace NAI_Rat_Constants
{
	const float TORSO_ANGLE_DELTA				= EPS_L;
	const DWORD LOST_MEMORY_TIME				= 20000;
	const DWORD LOST_RECOIL_TIME				=  2000;
	const float UNDER_FIRE_DISTANCE				= 100.f;
	const DWORD RETREAT_TIME					= 10000;
	const float RETREAT_DISTANCE				= 100.f;
	const float MIN_PROBABILITY					= 0.5f;
	const DWORD ACTION_REFRESH_RATE				= 1000;
	const float ATTACK_STRAIGHT_RUN_DISTANCE	= 10.f;
	const float	MAX_STABLE_DISTANCE				= 20.f;
};

class CAI_Rat : public CCustomMonster  
{
	protected:
		//////////////////////////
		// STRUCTURES
		//////////////////////////
		enum ESoundCcount {
			SND_HIT_COUNT=1,
			SND_DIE_COUNT=1,
			SND_ATTACK_COUNT=1,
			SND_VOICE_COUNT=2,
		};

		enum ERatStates 	{
			aiRatDie = 0,
			aiRatTurn,
			aiRatFreeHuntingActive,
			aiRatFreeHuntingPassive,
			aiRatAttackFire,
			aiRatAttackRun,
			aiRatUnderFire,
			aiRatRetreat,
			aiRatPursuit,
			aiRatFreeRecoil,
			aiRatReturnHome,
			aiRatReturnRecoil,
		};

	
		typedef	CCustomMonster inherited;

		typedef struct tagSNormalGlobalAnimations{
			CMotionDef *tpaDeath[2];
			CMotionDef *tpaAttack[3];
			CMotionDef *tpaIdle[2];
			SAnimState  tWalk;
			SAnimState  tRun;
			CMotionDef *tRunAttack;
			CMotionDef *tpTurnLeft;
			CMotionDef *tpTurnRight;
		}SNormalGlobalAnimations;

		// normal animations
		typedef struct tagSNormalAnimations{
			SNormalGlobalAnimations tGlobal;
		}SNormalAnimations;

		typedef struct tagSRatAnimations{
			SNormalAnimations	tNormal;
		}SRatAnimations;
		//////////////////////////
		// END OF STRUCTURES
		//////////////////////////

		//////////////////////////
		// CLASS MEMBERS
		//////////////////////////
		// ANIMATIONS
		SRatAnimations		m_tRatAnimations;
		CMotionDef*			m_tpCurrentGlobalAnimation;
		CBlend*				m_tpCurrentGlobalBlend;
		
		// SOUNDS
		sound				m_tpaSoundHit[SND_HIT_COUNT];
		sound				m_tpaSoundDie[SND_DIE_COUNT];
		sound				m_tpaSoundAttack[SND_ATTACK_COUNT];
		sound				m_tpaSoundVoice[SND_VOICE_COUNT];
		sound*				m_tpSoundBeingPlayed;
		DWORD				m_dwLastSoundRefresh;
		float				m_fMinVoiceIinterval;
		float				m_fMaxVoiceIinterval;
		float				m_fVoiceRefreshRate;
		DWORD				m_dwLastVoiceTalk;
		
		// ATTACK
		bool				m_bActionStarted;
		bool				m_bFiring;
		DWORD				m_dwStartAttackTime;
		float				m_fAttackSpeed;
		// HIT
		DWORD				m_dwHitTime;
		Fvector				m_tHitDir;
		Fvector				m_tHitPosition;
		float				m_fHitPower;
		DWORD				m_dwHitInterval;
		
		// SOUND BEING FELT
		SSimpleSound		m_tLastSound;
		
		// VISIBILITY
		objSET				m_tpaVisibleObjects;
		
		// ENEMY
		SEnemySelected		m_Enemy;
		CEntity*			m_tSavedEnemy;
		Fvector				m_tSavedEnemyPosition;
		DWORD				m_dwLostEnemyTime;
		NodeCompressed* 	m_tpSavedEnemyNode;
		DWORD				m_dwSavedEnemyNodeID;
		
		// PERFORMANCE
		DWORD				m_dwLastRangeSearch;
		
		// BEHAVIOUR
		Fvector				m_tGoalDir;
		Fvector				m_tCurrentDir;
		Fvector				m_tHPB;
		float				m_fDHeading;
		// constants
		float				m_fGoalChangeDelta;
		float				m_fSpeed;
		float				m_fSafeSpeed;
		float				m_fASpeed;
		float				m_fMinHeight;
		Fvector				m_tVarGoal;
		float				m_fIdleSoundDelta;
		Fvector				m_tSpawnPosition;
		Fvector				m_tSafeSpawnPosition;
		DWORD				m_dwStandLookTime;
		// variables
		float				m_fGoalChangeTime;
		Fvector				m_tOldPosition;
		bool				m_bNoWay;

		// Morale
		float				m_fMoraleSuccessAttackQuant;
		float				m_fMoraleDeathQuant;
		float				m_fMoraleFearQuant;
		float				m_fMoraleRestoreQuant;
		DWORD				m_dwMoraleRestoreTimeInterval;
		DWORD				m_dwMoraleLastUpdateTime;
		float				m_fMoraleMinValue;
		float				m_fMoraleMaxValue;
		float				m_fMoraleNormalValue;
		float				m_fMoraleNullRadius;
		// FSM
		stack<ERatStates>	tStateStack;
		ERatStates			eCurrentState;
		ERatStates			m_ePreviousState;
		bool				bStopThinking;
		bool				m_bStateChanged;

		// active
		float				m_fChangeActiveStateProbability;
		DWORD				m_dwActiveCountPercent;
		DWORD				m_dwActiveScheduleMin;
		DWORD				m_dwActiveScheduleMax;
		DWORD				m_dwPassiveScheduleMin;
		DWORD				m_dwPassiveScheduleMax;
		DWORD				m_dwStandingCountPercent;
		bool				m_bStanding;
		bool				m_bActive;

		// attack parameters
		float				m_fAttackDistance;
		float				m_fAttackAngle;

		//////////////////////////
		// INLINE FUNCTIONS
		//////////////////////////
		IC void vfChangeGoal()
		{
			Fvector vP;
			vP.set(m_tSpawnPosition.x,m_tSpawnPosition.y+m_fMinHeight,m_tSpawnPosition.z);
			m_tGoalDir.x = vP.x+m_tVarGoal.x*::Random.randF(-0.5f,0.5f); 
			m_tGoalDir.y = vP.y+m_tVarGoal.y*::Random.randF(-0.5f,0.5f);
			m_tGoalDir.z = vP.z+m_tVarGoal.z*::Random.randF(-0.5f,0.5f);
		}
		
		IC bool bfCheckIfGoalChanged(bool bForceChangeGoal = true)
		{
			if (m_fGoalChangeTime<=0){
				m_fGoalChangeTime += m_fGoalChangeDelta+m_fGoalChangeDelta*::Random.randF(-0.5f,0.5f);
				if (bForceChangeGoal)
					vfChangeGoal();
				return(true);
			}
			return(false);
		};

		IC void vfChooseNewSpeed()
		{
			int iRandom = ::Random.randI(0,2);
			switch (iRandom) {
				case 0 : {
					m_fSpeed = m_fMaxSpeed;
					break;
				}
				case 1 : {
					m_fSpeed = m_fMinSpeed;
					break;
				}
			}
			m_fSafeSpeed = m_fSpeed;
		};

		IC void vfUpdateTime(float fTimeDelta)
		{
			m_fGoalChangeTime -= fTimeDelta > .1f ? .1f : fTimeDelta;
		};		

		IC void vfAddActiveMember(bool bForceActive = false)
		{
			CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
			if (!m_bActive && (bForceActive || (Group.m_dwAliveCount*m_dwActiveCountPercent/100 >= Group.m_dwActiveCount))) {
				m_bActive = true;
				eCurrentState = aiRatFreeHuntingActive;
				Group.m_dwActiveCount++;
				shedule_Min	= m_dwActiveScheduleMin;
				shedule_Max	= m_dwActiveScheduleMax;
				vfRemoveStandingMember();
			}
			//Msg("* Group : alive[%2d], active[%2d]",Group.m_dwAliveCount,Group.m_dwActiveCount);
		};
		
		IC void vfRemoveActiveMember()
		{
			CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
			if (m_bActive) {
				R_ASSERT(Group.m_dwActiveCount > 0);
				Group.m_dwActiveCount--;
				m_bActive = false;
				eCurrentState = aiRatFreeHuntingPassive;
				shedule_Min	= m_dwPassiveScheduleMin;
				shedule_Max	= m_dwPassiveScheduleMax;
			}
			//Msg("* Group : alive[%2d], active[%2d]",Group.m_dwAliveCount,Group.m_dwActiveCount);
		};
		
		IC void vfAddStandingMember()
		{
			CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
			if ((Group.m_dwAliveCount*m_dwStandingCountPercent/100 >= Group.m_dwStandingCount) && (!m_bStanding)) {
				Group.m_dwStandingCount++;
				m_bStanding = true;
			}
		};
		
		IC void vfRemoveStandingMember()
		{
			CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
			if (m_bStanding) {
				R_ASSERT(Group.m_dwStandingCount > 0);
				Group.m_dwStandingCount--;
				m_bStanding = false;
			}
		};

		IC bool bfCheckIfSoundFrightful()
		{
			return(((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_RICOCHET) == SOUND_TYPE_WEAPON_BULLET_RICOCHET) || ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING));
		};
		
		//////////////////////////
		// MISCELLANIOUS FUNCTIONS
		//////////////////////////
		void	vfComputeNextDirectionPosition();
		void	vfUpdateMorale();
		void	vfComputeNewPosition();
		void	vfLoadSounds();
		void	vfLoadAnimations();
		void	SetDirectionLook();
		void	vfAimAtEnemy();
		void	vfSaveEnemy();
		void	vfSetFire(bool bFire, CGroup &Group);
		void	vfSetMovementType(char cBodyState, float fSpeed);
		void	vfUpdateDynamicObjects() {};
		//////////////////////////
		// FSM STATES
		//////////////////////////
		void	Death();
		void	FreeHuntingActive();
		void	FreeHuntingPassive();
		void	AttackFire();
		void	AttackRun();
		void	Turn();
		void	UnderFire();
		void	Retreat();
		void	Pursuit();
		void	FreeRecoil();
		void	ReturnHome();
		void	ReturnRecoil();
	public:
					   CAI_Rat();
		virtual		  ~CAI_Rat();
		virtual BOOL  ShadowReceive	()			{ return FALSE;	}
		virtual BOOL  net_Spawn(LPVOID DC);
		virtual void  net_Export(NET_Packet& P);
		virtual void  net_Import(NET_Packet& P);
		virtual void  HitSignal(float amount, Fvector& vLocalDir, CObject* who);
		virtual void  Die();
		virtual void  Load( LPCSTR section );
		virtual void  Think();
		virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed );
		virtual void  Exec_Movement(float dt);
		virtual void  Exec_Action(float dt);
		virtual objQualifier* GetQualifier();
		virtual	void  feel_sound_new(CObject* who, int type, Fvector& Position, float power);
		virtual void  OnDeviceCreate();
};
		
#endif
