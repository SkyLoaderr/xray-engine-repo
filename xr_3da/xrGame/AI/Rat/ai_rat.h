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

class CBaseFunction;

namespace NAI_Rat_Constants
{
	const float TORSO_ANGLE_DELTA		= EPS_L;
	const float ATTACK_DISTANCE			= .5f;
	const float ATTACK_ANGLE			= PI_DIV_6;
	const DWORD LOST_MEMORY_TIME		= 30000;
	const DWORD UNDER_FIRE_TIME			= 10000;
	const float UNDER_FIRE_DISTANCE		= 20.f;
	const DWORD RETREAT_TIME			= 10000;
	const float RETREAT_DISTANCE		= 20.f;
	const float MIN_PROBABILITY			= 0.5f;
	const DWORD ACTION_REFRESH_RATE		= 1000;
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
			SND_VOICE_COUNT=2,
		};

		enum ERatStates 	{
			aiRatDie = 0,
			aiRatTurn,
			aiRatFreeHunting,
			aiRatAttackFire,
			aiRatAttackRun,
			aiRatUnderFire,
			aiRatRetreat,
		};

	
		typedef	CCustomMonster inherited;

		typedef struct tagSNormalGlobalAnimations{
			CMotionDef* tpaDeath[2];
			CMotionDef* tpaAttack[3];
			CMotionDef* tpaIdle[2];
			SAnimState  tWalk;
			SAnimState  tRun;
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

		// FSM
		stack<ERatStates>	tStateStack;
		ERatStates			eCurrentState;
		ERatStates			m_ePreviousState;
		bool				bStopThinking;
		bool				m_bStateChanged;

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
		
		IC bool bfCheckIfGoalChanged()
		{
			if (m_fGoalChangeTime<=0){
				m_fGoalChangeTime += m_fGoalChangeDelta+m_fGoalChangeDelta*::Random.randF(-0.5f,0.5f);
				vfChangeGoal();
				return(true);
			}
			return(false);
		};

		IC void vfChooseNewSpeed()
		{
			int iRandom = ::Random.randI(0,3);
			switch (iRandom) {
				case 0 : {
					m_fSpeed = m_fMaxSpeed;
					break;
				}
				case 1 : {
					m_fSpeed = m_fMinSpeed;
					break;
				}
				case 2 : {
					if (::Random.randI(0,4) == 0)
						m_fSpeed = EPS_S;
					break;
				}
			}
			m_fSafeSpeed = m_fSpeed;
		};

		IC void vfUpdateTime(float fTimeDelta)
		{
			m_fGoalChangeTime -= fTimeDelta > .1f ? .1f : fTimeDelta;
		};		
		//////////////////////////
		// MISCELLANIOUS FUNCTIONS
		//////////////////////////
		void	vfComputeNewPosition();
		DWORD	dwfChooseAction(DWORD a1, DWORD a2, DWORD a3);
		bool	bfGetActionSuccessProbability(EntityVec &Members, objVisible &VisibleEnemies, CBaseFunction &tBaseFunction);
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
		void	Die();
		void	FreeHunting();
		void	AttackFire();
		void	AttackRun();
		void	Turn();
		void	UnderFire();
		void	Retreat();
	public:
					   CAI_Rat();
		virtual		  ~CAI_Rat();
		virtual BOOL  ShadowReceive	()			{ return FALSE;	}
		virtual BOOL  net_Spawn(LPVOID DC);
		virtual void  net_Export(NET_Packet& P);
		virtual void  net_Import(NET_Packet& P);
		virtual void  HitSignal(float amount, Fvector& vLocalDir, CObject* who);
		virtual void  Death();
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
