////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie.h
//	Created 	: 07.05.2002
//  Modified 	: 07.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_ZOMBIE__
#define __XRAY_AI_ZOMBIE__

#include "ai_zombie_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"

class CAI_Zombie : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};

	enum EZombieStates 	{
		aiZombieDie = 0,
		
		aiZombieJumping,
		
		aiZombieAttackFire,
		aiZombieAttackRun,
		aiZombieFreeHunting,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		
		////////////////////////////////////////////////////////////////////////////
		// normal animations
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSNormalGlobalAnimations{
			CMotionDef* tpaDeath[2];
			CMotionDef* tpaAttack[2];
			CMotionDef* tpIdle;
			SAnimState  tWalk;
			CMotionDef *tpTurnLeft;
			CMotionDef *tpTurnRight;
		}SNormalGlobalAnimations;

		typedef struct tagSNormalTorsoAnimations{
			CMotionDef *tpDamageLeft;
			CMotionDef *tpDamageRight;
		}SNormalTorsoAnimations;

		// normal animations
		typedef struct tagSNormalAnimations{
			SNormalGlobalAnimations tGlobal;
			SNormalTorsoAnimations tTorso;
		}SNormalAnimations;

		////////////////////////////////////////////////////////////////////////////
		// zombie animations
		////////////////////////////////////////////////////////////////////////////

		typedef struct tagSZombieAnimations{
			SNormalAnimations	tNormal;
		}SZombieAnimations;

		SZombieAnimations	tZombieAnimations;
		CMotionDef*			m_tpCurrentGlobalAnimation;
		CMotionDef*			m_tpCurrentTorsoAnimation;
		CMotionDef*			m_tpCurrentLegsAnimation;
		CBlend*				m_tpCurrentGlobalBlend;
		CBlend*				m_tpCurrentTorsoBlend;
		CBlend*				m_tpCurrentLegsBlend;
		
		// head turns
		static void __stdcall HeadSpinCallback(CBoneInstance*);
		static void __stdcall SpineSpinCallback(CBoneInstance*);
		
		// media
		sound			sndHit[SND_HIT_COUNT];
		sound			sndDie[SND_DIE_COUNT];
		
		// events
		EVENT			m_tpEventSay;
		EVENT			m_tpEventAssignPath;
		
		// ai
		EZombieStates	eCurrentState;
		EZombieStates	m_ePreviousState;
		bool			bStopThinking;
		
				// action data
		bool			m_bActionStarted;
		bool			m_bJumping;
		
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
		float			m_fLateralMutliplier;
		
		float			m_fHitPower;
		DWORD			m_dwHitInterval;

		DWORD			m_dwStartAttackTime;
		
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

		// finite state machine
		stack<EZombieStates>	tStateStack;
		bool					m_bStateChanged;

		CZombieSelectorFreeHunting	SelectorFreeHunting;
		CZombieSelectorAttack		SelectorAttack;
		
		void Die();
		
		void Jumping();
		
		void FreeHunting();
		void AttackFire();
		void AttackRun();
		
		// miscellanious funtions	
	IC  CGroup getGroup() {return Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];};
		bool bfCheckForVisibility(CEntity* tpEntity);
		void vfLoadSounds();
		void vfLoadSelectors(LPCSTR section);
		void vfAssignBones(CInifile *ini, const char *section);
		void vfLoadAnimations();
		bool bfCheckPath(AI::Path &Path);
		void SetLessCoverLook(NodeCompressed *tNode, bool bSpine = true);
		void SetDirectionLook();
		void vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		//void vfBuildPathToDestinationPoint(CZombieSelectorAttack *S);
		void vfBuildPathToDestinationPoint(CZombieSelectorFreeHunting *S);
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
					   CAI_Zombie();
		virtual		  ~CAI_Zombie();
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
		virtual BOOL  Spawn( BOOL bLocal, int sid, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
		virtual objQualifier* GetQualifier();
};
		
#endif
