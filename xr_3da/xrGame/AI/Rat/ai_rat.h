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
#include "..\\..\\..\\bodyinstance.h"

class CAI_Rat : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};

	enum ERatStates 	{
		aiRatDie = 0,
		aiRatJumping,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		
		// macroses
		#define MIN_RANGE_SEARCH_TIME_INTERVAL	15000.f
		#define MAX_TIME_RANGE_SEARCH			150000.f
		#define	FIRE_ANGLE						PI/10
		#define	FIRE_SAFETY_ANGLE				PI/10
		#define LEFT_NODE(Index)				((Index + 3) & 3)
		#define RIGHT_NODE(Index)				((Index + 5) & 3)
		#define FN(i)							(float(tNode->cover[(i)])/255.f)
		#define	AMMO_NEED_RELOAD				6
		#define	MAX_HEAD_TURN_ANGLE				(PI/3.f)
		#define EYE_WEAPON_DELTA				(0*PI/30.f)
		#define TORSO_ANGLE_DELTA				(PI/30.f)
		#define NEXT_POINT(m_iCurrentPoint)		(m_iCurrentPoint) == tpaPatrolPoints.size() - 1 ? 0 : (m_iCurrentPoint) + 1
		#define PREV_POINT(m_iCurrentPoint)		(m_iCurrentPoint) == 0 ? tpaPatrolPoints.size() - 1 : (m_iCurrentPoint) - 1
		#define MAX_PATROL_DISTANCE				6.f
		#define MIN_PATROL_DISTANCE				1.f
		#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
		#define COMPUTE_DISTANCE_2D(t,p)		(sqrtf(_sqr((t).x - (p).x) + _sqr((t).z - (p).z)))
		#define MIN_COVER_MOVE					120
		#define MAX_NEIGHBOUR_COUNT				9
		////////////////////////////////////////////////////////////////////////////
		// normal animations
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSNormalGlobalAnimations{
			CMotionDef* tpaDeath[3];
			CMotionDef* tpaAttack[2];
			CMotionDef* tpIdle;
			SAnimState  tWalk;
			CMotionDef* tpDamageLeft;
			CMotionDef* tpDamageRight;
		}SNormalGlobalAnimations;

		typedef struct tagSNormalLegsAnimations{
			CMotionDef *tpTurn;
		}SNormalLegsAnimations;

		typedef struct tagSNormalTorsoAnimations{
			CMotionDef *tpDamageLeft;
			CMotionDef *tpDamageRight;
		}SNormalTorsoAnimations;

		// normal animations
		typedef struct tagSNormalAnimations{
			SNormalGlobalAnimations tGlobal;
			SNormalTorsoAnimations tTorso;
			SNormalLegsAnimations tLegs;
		}SNormalAnimations;

		////////////////////////////////////////////////////////////////////////////
		// rat animations
		////////////////////////////////////////////////////////////////////////////

		typedef struct tagSRatAnimations{
			SNormalAnimations	tNormal;
		}SRatAnimations;

		SRatAnimations	tRatAnimations;
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
		sound3D			sndHit[SND_HIT_COUNT];
		sound3D			sndDie[SND_DIE_COUNT];
		
		// events
		EVENT			m_tpEventSay;
		EVENT			m_tpEventAssignPath;
		
		// ai
		ERatStates	eCurrentState;
		ERatStates	m_ePreviousState;
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
		CRatSelectorFollowLeader		SelectorFollowLeader;
		CRatSelectorPursuit				SelectorPursuit;
		CRatSelectorUnderFire			SelectorUnderFire;

		void AttackRun();
		void AttackFire();
		
		void Die();
		void Defend();
		void FindEnemy();
		void FollowLeader();
		void FreeHunting();
		void Injuring();
		void Jumping();
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
		
		// miscellanious funtions	
	IC  CGroup getGroup() {return Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];};
		bool bfCheckForVisibility(CEntity* tpEntity);
		void vfLoadSounds();
		void vfLoadSelectors(CInifile *ini, const char *section);
		void vfAssignBones(CInifile *ini, const char *section);
		void vfLoadAnimations();
		bool bfCheckPath(AI::Path &Path);
		void SetLessCoverLook(NodeCompressed *tNode, bool bSpine = true);
		void SetDirectionLook();
		void SetSmartLook(NodeCompressed *tNode, Fvector &tEnemyDirection);
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
		virtual void  Update(DWORD DT);
		virtual void  net_Export(NET_Packet* P);
		virtual void  net_Import(NET_Packet* P);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  SenseSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( CInifile* ini, const char* section );
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
