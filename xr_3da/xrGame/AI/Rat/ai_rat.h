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
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4,
		SND_VOICE_COUNT=2,
	};

	enum ERatStates 	{
		aiRatDie = 0,
		aiRatTurn,
		aiRatFreeHunting,
		aiRatFollowLeader,
		aiRatAttackFire,
		aiRatAttackRun,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		
		vector<SDynamicSound>	tpaDynamicSounds;
		DWORD					m_dwMaxDynamicSoundsCount;
		float					m_fSensetivity;
		int						m_iSoundIndex;
		DWORD					m_dwLastUpdate;
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
		ERatStates	eCurrentState;
		ERatStates	m_ePreviousState;
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
		vector<Fvector>			m_tpaPatrolPoints;
		vector<Fvector>			m_tpaPointDeviations;
		vector<DWORD>			m_dwaNodes;
		DWORD					m_dwStartPatrolNode;
		DWORD					m_dwCreatePathAttempts;
		bool					m_bLooped;
		DWORD					m_dwPatrolPathIndex;
		DWORD					m_dwLoopCount;
		float					m_fMinPatrolDistance;
		float					m_fMaxPatrolDistance;
		bool					m_bLessCoverLook;
		DWORD					m_dwStartAttackTime;

		// finite state machine
		stack<ERatStates>		tStateStack;
		bool					m_bStateChanged;
		
		CRatSelectorAttack				SelectorAttack;
		CRatSelectorFreeHunting			SelectorFreeHunting;

		/**
		void AttackRun();
		void AttackFire();
		
		void Defend();
		void FindEnemy();
		void Injuring();
		void MoreDeadThanAlive();
		void NoWeapon();

		void StandingUp();
		void Sitting();
		void LyingDown();

		void Patrol();
		void PatrolReturn();
		void PatrolHurt();
		void PatrolHurtAggressiveUnderFire();
		void PatrolHurtNonAggressiveUnderFire();
		void PatrolUnderFire();
		void FollowLeaderPatrol();
		void TurnOver();
		
		void Pursuit();
		void Reload();
		void Retreat();
		void SenseSomething();
		void UnderFire();
		/**/
		void Die();
		void FollowLeader();
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
		virtual void  net_Export(NET_Packet* P);
		virtual void  net_Import(NET_Packet* P);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  SenseSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( LPCSTR section );
		virtual void  Think();
		virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed );
		virtual void  Exec_Movement(float dt);
		virtual void  Exec_Action(float dt);
		virtual void  OnEvent(EVENT E, DWORD P1, DWORD P2);
		virtual BOOL  net_Spawn( BOOL bLocal, int sid, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
		virtual objQualifier* GetQualifier();
};
		
#endif
