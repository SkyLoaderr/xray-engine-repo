////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.h
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_SOLDIER__
#define __XRAY_AI_SOLDIER__

#include "ai_soldier_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"
#include "..\\..\\..\\bodyinstance.h"

class CAI_Soldier : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};

	enum ESoldierStates 	{
		aiSoldierAttackRun = 0,
		aiSoldierAttackFire,
		aiSoldierDefend,
		aiSoldierDie,
		aiSoldierFindEnemy,
		aiSoldierFollowLeader,
		aiSoldierFreeHunting,
		aiSoldierInjuring,
		aiSoldierJumping,
		
		aiSoldierStandingUp,
		aiSoldierSitting,
		aiSoldierLyingDown,

		aiSoldierMoreDeadThanAlive,
		aiSoldierNoWeapon,
		
		aiSoldierPatrolReturnToRoute,
		aiSoldierPatrolRoute,
		aiSoldierFollowLeaderPatrol,
		aiSoldierPatrolHurt,
		aiSoldierPatrolHurtAggressiveUnderFire,
		aiSoldierPatrolHurtNonAggressiveUnderFire,
		aiSoldierPatrolUnderFire,
		aiSoldierTurnOver,

		aiSoldierPursuit,
		aiSoldierRecharge,
		aiSoldierRetreat,
		aiSoldierSenseSomething,
		aiSoldierUnderFire,
		
		#ifdef TEST_ACTIONS
			aiMonsterTestMicroActions,
			aiMonsterTestMicroActionA,
			aiMonsterTestMicroActionD,
			aiMonsterTestMicroActionQ,
			aiMonsterTestMicroActionE,
			aiMonsterTestMicroActionZ,
			aiMonsterTestMicroActionC,
			aiMonsterTestMicroActionW,
			aiMonsterTestMicroActionS,
			aiMonsterTestMicroActionX,
			aiMonsterTestMicroActionR,
			aiMonsterTestMicroActionF,
			aiMonsterTestMicroActionV,
			aiMonsterTestMicroActionT,
			aiMonsterTestMicroActionG,
			aiMonsterTestMicroActionB,
			aiMonsterTestMicroActionY,
			aiMonsterTestMicroActionH,
			aiMonsterTestMicroActionN,
			aiMonsterTestMicroActionU,
			aiMonsterTestMicroActionJ,
			aiMonsterTestMicroActionM,

			aiMonsterTestMacroActions,
			aiMonsterTestMacroActionA,
			aiMonsterTestMacroActionD,
			aiMonsterTestMacroActionQ,
			aiMonsterTestMacroActionE,
			aiMonsterTestMacroActionZ,
			aiMonsterTestMacroActionC,
			aiMonsterTestMacroActionW,
			aiMonsterTestMacroActionS,
			aiMonsterTestMacroActionX,
			aiMonsterTestMacroActionR,
			aiMonsterTestMacroActionF,
			aiMonsterTestMacroActionV,
			aiMonsterTestMacroActionT,
			aiMonsterTestMacroActionG,
			aiMonsterTestMacroActionB,
			aiMonsterTestMacroActionY,
			aiMonsterTestMacroActionH,
			aiMonsterTestMacroActionN,
			aiMonsterTestMacroActionU,
			aiMonsterTestMacroActionJ,
			aiMonsterTestMacroActionM,
		#endif
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

		////////////////////////////////////////////////////////////////////////////
		// normal animations
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSNormalGlobalAnimations{
			CMotionDef* tpaDeath[2];
			CMotionDef* tpJumpBegin;
			CMotionDef* tpJumpIdle;
		}SNormalGlobalAnimations;

		// torso animations
		typedef struct tagSNormalTorsoAnimations{
			CMotionDef* tpaIdle[2];
			CMotionDef* tpaAim[2];
			CMotionDef* tpaAttack[2];
			CMotionDef* tpDamageLeft;
			CMotionDef* tpDamageRight;
			CMotionDef* tpReload;
		}SNormalTorsoAnimations;

		// legs animations
		typedef struct tagSNormalLegsAnimations{
			SAnimState  tRun;
			SAnimState  tWalk;
			CMotionDef* tpTurn;
			CMotionDef* tpIdle;
		}SNormalLegsAnimations;

		// hands animations
		typedef struct tagSNormalHandsAnimations{
			CMotionDef* tpPointGesture;
			CMotionDef* tpSmokeGesture;
		}SNormalHandsAnimations;

		// normal animations
		typedef struct tagSNormalAnimations{
			SNormalGlobalAnimations tGlobal;
			SNormalTorsoAnimations  tTorso;
			SNormalLegsAnimations	tLegs;
			SNormalHandsAnimations	tHands;
		}SNormalAnimations;

		////////////////////////////////////////////////////////////////////////////
		// crouch animations
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSCrouchGlobalAnimations{
			CMotionDef* tpDeath;
			CMotionDef* tpJumpBegin;
			CMotionDef* tpJumpIdle;
		}SCrouchGlobalAnimations;

		// torso animations
		typedef struct tagSCrouchTorsoAnimations{
			CMotionDef* tpAim;
		}SCrouchTorsoAnimations;

		// legs animations
		typedef struct tagSCrouchLegsAnimations{
			SAnimState  tRun;
			SAnimState  tWalk;
			CMotionDef* tpTurn;
			CMotionDef* tpIdle;
		}SCrouchLegsAnimations;

		// hands animations
		typedef struct tagSCrouchHandsAnimations{
			CMotionDef* tpPointGesture;
		}SCrouchHandsAnimations;

		// crouch animations
		typedef struct tagSCrouchAnimations{
			SCrouchGlobalAnimations	tGlobal;
			SCrouchTorsoAnimations  tTorso;
			SCrouchLegsAnimations	tLegs;
			SCrouchHandsAnimations	tHands;
		}SCrouchAnimations;

		////////////////////////////////////////////////////////////////////////////
		// lie animations
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSLieGlobalAnimations{
			CMotionDef* tpDeath;
			CMotionDef* tpLieDown;
		}SLieGlobalAnimations;

		// torso animations
		typedef struct tagSLieTorsoAnimations{
			CMotionDef* tpIdle;
			CMotionDef* tpReload;
		}SLieTorsoAnimations;

		// legs animations
		typedef struct tagSLieLegsAnimations{
			SAnimState  tWalk;
		}SLieLegsAnimations;

		// hands animations
		typedef struct tagSLieHandsAnimations{
			CMotionDef* tpPointGesture;
		}SLieHandsAnimations;

		// lie animations
		typedef struct tagSLieAnimations{
			SLieGlobalAnimations	tGlobal;
			SLieTorsoAnimations		tTorso;
			SLieLegsAnimations		tLegs;
			SLieHandsAnimations		tHands;
		}SLieAnimations;

		////////////////////////////////////////////////////////////////////////////
		// soldier animations
		////////////////////////////////////////////////////////////////////////////

		typedef struct tagSSoldierAnimations{
			SNormalAnimations	tNormal;
			SCrouchAnimations	tCrouch;
			SLieAnimations		tLie;
		}SSoldierAnimations;

		SSoldierAnimations	tSoldierAnimations;
		CMotionDef*			m_tpCurrentGlobalAnimation;
		CMotionDef*			m_tpCurrentTorsoAnimation;
		CMotionDef*			m_tpCurrentHandsAnimation;
		CMotionDef*			m_tpCurrentLegsAnimation;
		CBlend*				m_tpCurrentGlobalBlend;
		CBlend*				m_tpCurrentTorsoBlend;
		CBlend*				m_tpCurrentHandsBlend;
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
		ESoldierStates	eCurrentState;
		ESoldierStates	m_ePreviousState;
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
		
		// characteristics
		float			m_fAggressiveness;
		float			m_fTimorousness;
		
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
		
		// firing
		bool			m_bFiring;
		DWORD			m_dwStartFireAmmo;
		DWORD			m_dwNoFireTime;
		
		// fire  constants
		DWORD			m_dwFireRandomMin;
		DWORD			m_dwFireRandomMax;
		DWORD			m_dwNoFireTimeMin;
		DWORD			m_dwNoFireTimeMax;
		
		// patrol under fire constants
		DWORD			m_dwPatrolShock;
		DWORD			m_dwUnderFireShock;
		DWORD			m_dwUnderFireReturn;
		// //

		// patrol structures
		vector<Fvector>			m_tpaPatrolPoints;
		vector<Fvector>			m_tpaPointDeviations;
		vector<DWORD>			m_dwaNodes;
		DWORD					m_dwStartPatrolNode;
		bool					m_bLooped;
		DWORD					m_dwPatrolPathIndex;
		DWORD					m_dwLoopCount;
		DWORD					m_dwCreatePathAttempts;
		float					m_fMinPatrolDistance;
		float					m_fMaxPatrolDistance;
		bool					m_bLessCoverLook;

		// finite state machine
		stack<ESoldierStates>	tStateStack;
		bool					m_bStateChanged;
		
		CSoldierSelectorAttack				SelectorAttack;
		CSoldierSelectorDefend				SelectorDefend;
		CSoldierSelectorFindEnemy			SelectorFindEnemy;
		CSoldierSelectorFollowLeader		SelectorFollowLeader;
		CSoldierSelectorFreeHunting			SelectorFreeHunting;
		CSoldierSelectorMoreDeadThanAlive	SelectorMoreDeadThanAlive;
		CSoldierSelectorNoWeapon			SelectorNoWeapon;
		CSoldierSelectorPatrol				SelectorPatrol;
		CSoldierSelectorPursuit				SelectorPursuit;
		CSoldierSelectorReload				SelectorReload;
		CSoldierSelectorRetreat				SelectorRetreat;
		CSoldierSelectorSenseSomething		SelectorSenseSomething;
		CSoldierSelectorUnderFire			SelectorUnderFire;

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
		void Recharge();
		void Retreat();
		void SenseSomething();
		void UnderFire();
		
		// test
		void TestMicroActions();
		#ifdef TEST_ACTIONS
			DECLARE_MICRO_ACTION(A);
			DECLARE_MICRO_ACTION(D);
			DECLARE_MICRO_ACTION(Q);
			DECLARE_MICRO_ACTION(E);
			DECLARE_MICRO_ACTION(Z);
			DECLARE_MICRO_ACTION(C);
			DECLARE_MICRO_ACTION(W);
			DECLARE_MICRO_ACTION(S);
			DECLARE_MICRO_ACTION(X);
			DECLARE_MICRO_ACTION(R);
			DECLARE_MICRO_ACTION(F);
			DECLARE_MICRO_ACTION(V);
			DECLARE_MICRO_ACTION(T);
			DECLARE_MICRO_ACTION(G);
			DECLARE_MICRO_ACTION(B);
			DECLARE_MICRO_ACTION(Y);
			DECLARE_MICRO_ACTION(H);
			DECLARE_MICRO_ACTION(N);
			DECLARE_MICRO_ACTION(U);
			DECLARE_MICRO_ACTION(J);
			DECLARE_MICRO_ACTION(M);
			void TestMacroActions();
			DECLARE_MACRO_ACTION(A);
			DECLARE_MACRO_ACTION(D);
			DECLARE_MACRO_ACTION(Q);
			DECLARE_MACRO_ACTION(E);
			DECLARE_MACRO_ACTION(Z);
			DECLARE_MACRO_ACTION(C);
			DECLARE_MACRO_ACTION(W);
			DECLARE_MACRO_ACTION(S);
			DECLARE_MACRO_ACTION(X);
			DECLARE_MACRO_ACTION(R);
			DECLARE_MACRO_ACTION(F);
			DECLARE_MACRO_ACTION(V);
			DECLARE_MACRO_ACTION(T);
			DECLARE_MACRO_ACTION(G);
			DECLARE_MACRO_ACTION(B);
			DECLARE_MACRO_ACTION(Y);
			DECLARE_MACRO_ACTION(H);
			DECLARE_MACRO_ACTION(N);
			DECLARE_MACRO_ACTION(U);
			DECLARE_MACRO_ACTION(J);
			DECLARE_MACRO_ACTION(M);
		#endif
		
		// miscellanious funtions	
	IC  CGroup getGroup() {return Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];};
		bool bfCheckForVisibility(CEntity* tpEntity);
		void vfLoadSounds();
		void vfLoadSelectors(CInifile *ini, const char *section);
		void vfAssignBones(CInifile *ini, const char *section);
		void vfLoadAnimations();
		bool bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
		bool bfCheckPath(AI::Path &Path);
		void SetLessCoverLook(NodeCompressed *tNode, bool bSpine = true);
		void SetDirectionLook();
		void SetSmartLook(NodeCompressed *tNode, Fvector &tEnemyDirection);
		void vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfBuildPathToDestinationPoint(CSoldierSelectorAttack *S);
		void vfSearchForBetterPosition(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfSearchForBetterPositionWTime(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfAimAtEnemy();
		void vfSaveEnemy();
		bool bfCheckIfCanKillMember();
		bool bfCheckIfCanKillEnemy();
		void vfSetFire(bool bFire, CGroup &Group);
		void vfSetMovementType(char cBodyState, float fSpeed);
		void vfCheckForSavedEnemy();

	public:
					   CAI_Soldier();
		virtual		  ~CAI_Soldier();
		virtual void  Update(DWORD DT);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  SenseSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( CInifile* ini, const char* section );
		virtual void  Think();
		virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed );
		virtual void  g_fireParams(Fvector &fire_pos, Fvector &fire_dir);
		virtual void  OnVisible(); 
		virtual void  Exec_Movement(float dt);
		virtual void  OnEvent(EVENT E, DWORD P1, DWORD P2);
		virtual BOOL  Spawn( BOOL bLocal, int sid, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
		virtual objQualifier* GetQualifier();
};
		
#endif
