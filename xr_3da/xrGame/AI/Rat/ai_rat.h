////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.h
//	Created 	: 23.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_RAT__
#define __XRAY_AI_RAT__

#include "ai_rat_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"

class CAI_Rat : public CCustomMonster  
{
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
	};
	
	typedef	CCustomMonster inherited;

	protected:
		
		SSimpleSound			m_tLastSound;
		DWORD					m_dwMaxDynamicSoundsCount;
		float					m_fSensetivity;
		int						m_iSoundIndex;
		DWORD					m_dwLastUpdate;
		////////////////////////////////////////////////////////////////////////////
		// Test propeties
		////////////////////////////////////////////////////////////////////////////
		Fvector			m_tOldPosition;
		// parameters block
		Fvector			m_tGoalDir;
		Fvector			m_tCurrentDir;
		Fvector			m_tHPB;
		float			m_fDHeading;
		// constants
		float			m_fGoalChangeDelta;
		float			m_fSpeed;
		float			m_fSafeSpeed;
		float			m_fASpeed;
		float			m_fMinHeight;
		Fvector			m_tVarGoal;
		float			m_fIdleSoundDelta;
		// variables
		float			m_fGoalChangeTime;
		//
		Fvector			m_tSpawnPosition;
		Fvector			m_tSafeSpawnPosition;
		DWORD			m_dwStandLookTime;
		////////////////////////////////////////////////////////////////////////////
		// end of test properties
		////////////////////////////////////////////////////////////////////////////
		
		////////////////////////////////////////////////////////////////////////////
		// normal animations
		////////////////////////////////////////////////////////////////////////////
		// global animations
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

		////////////////////////////////////////////////////////////////////////////
		// rat animations
		////////////////////////////////////////////////////////////////////////////

		typedef struct tagSRatAnimations{
			SNormalAnimations	tNormal;
		}SRatAnimations;

		SRatAnimations		tRatAnimations;
		CMotionDef*			m_tpCurrentGlobalAnimation;
		CBlend*				m_tpCurrentGlobalBlend;
		
		// head turns
		static void __stdcall HeadSpinCallback(CBoneInstance*);
		static void __stdcall SpineSpinCallback(CBoneInstance*);
		
		// media
		sound			sndHit[SND_HIT_COUNT];
		sound			sndDie[SND_DIE_COUNT];
		sound			sndVoices[SND_VOICE_COUNT];
		sound*			m_tpSoundBeingPlayed;
		DWORD			m_dwLastSoundRefresh;
		float			m_fMinVoiceIinterval;
		float			m_fMaxVoiceIinterval;
		float			m_fVoiceRefreshRate;
		DWORD			m_dwLastVoiceTalk;
		
		// events
		EVENT			m_tpEventSay;
		EVENT			m_tpEventAssignPath;
		
		// ai
		ERatStates		eCurrentState;
		ERatStates		m_ePreviousState;
		bool			bStopThinking;
		
				// action data
		bool			m_bActionStarted;
		bool			m_bFiring;
		
		// hit data
		DWORD			dwHitTime;
		Fvector			tHitDir;
		Fvector			tHitPosition;
		
		// sense data
		DWORD			dwSenseTime;
		Fvector			tSenseDir;

		// visual data
		objSET			tpaVisibleObjects;
		
		// movement data
		vector<SSubNode> tpSubNodes;
		
		// saved enemy
		SEnemySelected	Enemy;
		CEntity*		tSavedEnemy;
		Fvector			tSavedEnemyPosition;
		DWORD			dwLostEnemyTime;
		NodeCompressed* tpSavedEnemyNode;
		DWORD			dwSavedEnemyNodeID;
		bool			bBuildPathToLostEnemy;
		
		// performance data
		DWORD			m_dwLastRangeSearch;
		DWORD			m_dwLastSuccessfullSearch;
		
		// visibility constants
		DWORD			m_dwMovementIdleTime;
		float			m_fMaxInvisibleSpeed;
		float			m_fMaxViewableSpeed;
		float			m_fMovementSpeedWeight;
		float			m_fDistanceWeight;
		float			m_fSpeedWeight;
		float			m_fCrouchVisibilityMultiplier;
		float			m_fLieVisibilityMultiplier;
		float			m_fVisibilityThreshold;
		float			m_fLateralMultiplier;
		float			m_fShadowWeight;
		
		float			m_fHitPower;
		DWORD			m_dwHitInterval;
		
		// patrol structures
		CLevel::SPath	*m_tpPath;
		bool			m_bLessCoverLook;
		DWORD			m_dwLoopCount;
		float			m_fMinPatrolDistance;
		float			m_fMaxPatrolDistance;
		DWORD			m_dwStartAttackTime;
		DWORD			m_dwLostEnemyTime;

		// finite state machine
		stack<ERatStates>	tStateStack;
		bool				m_bStateChanged;
		
		CRatSelectorAttack			SelectorAttack;
		CRatSelectorFreeHunting		SelectorFreeHunting;

		
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
		
		void vfComputeNewPosition();
		/**
		void SenseSomething();
		void UnderFire();
		/**/
		void Die();
		void FreeHunting();
		void AttackFire();
		void AttackRun();
		void Turn();
		
		// miscellanious funtions	
	IC  CGroup getGroup() {return Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];};
		bool bfCheckForVisibility(CEntity* tpEntity);
		void vfLoadSounds();
		void vfLoadSelectors(LPCSTR section);
		void vfLoadAnimations();
		void vfAssignBones(CInifile *ini, const char *section);
		bool bfCheckPath(AI::Path &Path);
		//void SetLessCoverLook(NodeCompressed *tNode, bool bSpine = true);
		void SetDirectionLook();
		void vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfBuildPathToDestinationPoint(CRatSelectorAttack *S);
		void vfSearchForBetterPosition(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfSearchForBetterPositionWTime(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfAimAtEnemy();
		void vfSaveEnemy();
		void vfSetFire(bool bFire, CGroup &Group);
		void vfSetMovementType(char cBodyState, float fSpeed);
		void vfCheckForSavedEnemy();
	IC  bool bfInsideSubNode(const Fvector &tCenter, const SSubNode &tpSubNode);
	IC  bool bfInsideSubNode(const Fvector &tCenter, const float fRadius, const SSubNode &tpSubNode);
	IC  bool bfInsideNode(const Fvector &tCenter, const NodeCompressed *tpNode);
	IC  float ffComputeCost(Fvector tLeaderPosition,SSubNode &tCurrentNeighbour);
	IC  float ffGetY(NodeCompressed &tNode, float X, float Z);
	IC  bool bfNeighbourNode(const SSubNode &tCurrentSubNode, const SSubNode &tMySubNode);
		int  ifDivideNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes);
		int  ifDivideNearestNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes);
		void GoToPointViaSubnodes(Fvector &tLeaderPosition);
		void vfUpdateDynamicObjects();

	public:
					   CAI_Rat();
		virtual		  ~CAI_Rat();
		virtual BOOL  ShadowReceive	()			{ return FALSE;	}
		virtual void  Update(DWORD DT);
		virtual BOOL  net_Spawn(LPVOID DC);
		virtual void  net_Export(NET_Packet& P);
		virtual void  net_Import(NET_Packet& P);
		virtual void  HitSignal(float amount, Fvector& vLocalDir, CObject* who);
		virtual void  SenseSignal(float amount, Fvector& vLocalDir, CObject* who);
		virtual void  Death();
		virtual void  Load( LPCSTR section );
		virtual void  Think();
		virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed );
		virtual void  Exec_Movement(float dt);
		virtual void  Exec_Action(float dt);
		virtual void  OnEvent(EVENT E, DWORD P1, DWORD P2);
		virtual objQualifier* GetQualifier();
		virtual	void  feel_sound_new(CObject* who, int type, Fvector& Position, float power);
};
		
#endif
