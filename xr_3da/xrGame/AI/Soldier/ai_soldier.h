////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.h
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_SOLDIER__
#define __XRAY_AI_SOLDIER__

#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"
#include "ai_soldier_selectors.h"
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

		aiSoldierPursuit,
		aiSoldierReload,
		aiSoldierRetreat,
		aiSoldierSenseSomething,
		aiSoldierUnderFire,
		
		aiSoldierTestMicroActions,
		aiSoldierTestA,
		aiSoldierTestD,
		aiSoldierTestQ,
		aiSoldierTestE,
		aiSoldierTestZ,
		aiSoldierTestC,
		aiSoldierTestW,
		aiSoldierTestS,
		aiSoldierTestX,
		aiSoldierTestR,
		aiSoldierTestF,
		aiSoldierTestV,
		aiSoldierTestT,
		aiSoldierTestG,
		aiSoldierTestB,
		aiSoldierTestY,
		aiSoldierTestH,
		aiSoldierTestN,
		aiSoldierTestU,
		aiSoldierTestJ,
		aiSoldierTestM,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		
		// macroses
		#define MIN_RANGE_SEARCH_TIME_INTERVAL	15000.f
		#define MAX_TIME_RANGE_SEARCH			150000.f
		#define	FIRE_ANGLE						PI/10
		#define	FIRE_SAFETY_ANGLE				PI/10
		#define CUBE(x)							((x)*(x)*(x))
		#define LEFT_NODE(Index)				((Index + 3) & 3)
		#define RIGHT_NODE(Index)				((Index + 5) & 3)
		#define FN(i)							(float(tNode->cover[(i)])/255.f)
		#define	AMMO_NEED_RELOAD				6
		#define	MAX_HEAD_TURN_ANGLE				(PI/3.f)
		#define EYE_WEAPON_DELTA				(0*PI/30.f)
		#define TORSO_ANGLE_DELTA				(PI/20.f)
		#define NEXT_POINT(m_iCurrentPoint)		(m_iCurrentPoint) == tpaPatrolPoints.size() - 1 ? 0 : (m_iCurrentPoint) + 1
		#define PREV_POINT(m_iCurrentPoint)		(m_iCurrentPoint) == 0 ? tpaPatrolPoints.size() - 1 : (m_iCurrentPoint) - 1
		#define MAX_PATROL_DISTANCE				6.f
		#define MIN_PATROL_DISTANCE				1.f
		#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
		#define ASSIGN_SPINE_BONE				r_spine_target.yaw = fabsf(r_torso_target.yaw - r_target.yaw - PI_DIV_6) < MIN_SPINE_TURN_ANGLE ? r_target.yaw : (2*r_torso_target.yaw + r_target.yaw)/3;
				
		#define INIT_SQUAD_AND_LEADER \
			CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];\
			CEntity* Leader = Squad.Leader;\
			if (Leader->g_Health() <= 0)\
				Leader = this;\
			R_ASSERT (Leader);
		
		#define WRITE_LOG

		#ifdef WRITE_LOG
			#define WRITE_TO_LOG(S) \
				Msg("%s,%s,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",cName(),S,Level().timeServer(),vPosition.x,vPosition.y,vPosition.z,r_current.yaw,r_current.pitch,r_torso_current.yaw,r_torso_current.pitch);\
				bStopThinking = true;
		#else
			#define WRITE_TO_LOG(S) \
				bStopThinking = true;
		#endif

		#define TEST_MICRO_ACTION(A) \
			if (Level().iGetKeyState(DIK_##A)) {\
				if (!m_b##A) {\
					m_b##A = true;\
					tStateStack.push(eCurrentState);\
					eCurrentState = aiSoldierTest##A;\
					return;\
				}\
			}\
			else\
				m_b##A = false;
				
		#define CASE_MICRO_ACTION(A) \
			case aiSoldierTest##A : {\
				Test##A();\
				break;\
			}
				
		#define ADJUST_ANGLE(A) \
			if (A >= PI_MUL_2 - EPS_L)\
				A -= PI_MUL_2;\
			else\
				if (A <= -EPS_L)\
					A += PI_MUL_2;
				
		#define ADJUST_BONE(A) \
			ADJUST_ANGLE(A.yaw);\
			ADJUST_ANGLE(A.pitch);
				
		#define ADJUST_BONE_ANGLES \
			ADJUST_BONE(r_target);\
			ADJUST_BONE(r_current);\
			ADJUST_BONE(r_torso_target);\
			ADJUST_BONE(r_torso_current);\
			ADJUST_BONE(r_spine_target);\
			ADJUST_BONE(r_spine_current);
				
		#define SUB_ANGLE(A,B)\
			A -= B;\
			if (A <= -EPS_L )\
				A += PI_MUL_2;
				
		#define ADD_ANGLE(A,B)\
			A += B;\
			if (A >= PI_MUL_2 - EPS_L)\
				A -= PI_MUL_2;
		
		#define DECLARE_MICRO_ACTION(A)\
			void Test##A();\
			bool m_b##A;
		
		#define CHECK_FOR_STATE_TRANSITIONS(S) \
			WRITE_TO_LOG(S);\
			\
			if (g_Health() <= 0) {\
				eCurrentState = aiSoldierDie;\
				return;\
			}\
			\
			SelectEnemy(Enemy);\
			\
			if (Enemy.Enemy) {\
				tStateStack.push(eCurrentState);\
				eCurrentState = aiSoldierAttackFire;\
				m_dwLastRangeSearch = 0;\
				return;\
			}\
			\
			DWORD dwCurTime = Level().timeServer();\
			\
			if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {\
				tStateStack.push(eCurrentState);\
				eCurrentState = aiSoldierPatrolHurt;\
				m_dwLastRangeSearch = 0;\
				return;\
			}\
			\
			if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {\
				tStateStack.push(eCurrentState);\
				eCurrentState = aiSoldierSenseSomething;\
				m_dwLastRangeSearch = 0;\
				return;\
			}\
			\
			INIT_SQUAD_AND_LEADER;\
			\
			CGroup &Group = Squad.Groups[g_Group()];\
			\
			if ((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime)) {\
				tStateStack.push(eCurrentState);\
				eCurrentState = aiSoldierPatrolUnderFire;\
				m_dwLastRangeSearch = 0;\
				return;\
			}

		/************************************************************************/
		/* 		
		tHitDir = Group.m_tLastHitDirection;\
		dwHitTime = Group.m_dwLastHitTime;\
		tHitPosition = Group.m_tHitPosition;\
		*/
		/************************************************************************/

		#define SET_LOOK_FIRE_MOVEMENT(a,b,c)\
			if (!(Group.m_bLessCoverLook)) {\
				Group.m_bLessCoverLook = m_bLessCoverLook = true;\
				Group.m_dwLastViewChange = dwCurTime;\
			}\
			else\
				if ((m_bLessCoverLook) && (dwCurTime - Group.m_dwLastViewChange > 5000))\
					Group.m_bLessCoverLook = m_bLessCoverLook = false;\
			if (m_bLessCoverLook)\
				SetLessCoverLook(AI_Node);\
			else\
				SetDirectionLook();\
			\
			vfSetFire(a,Group);\
			\
			vfSetMovementType(b,c);

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
		
		EVENT			m_tpEventSay;
		
		// ai
		ESoldierStates	eCurrentState;
		ESoldierStates	m_ePreviousState;
		bool			bStopThinking;
		
		// action data
		bool			m_bActionStarted;
		
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
		DWORD					m_dwStartPatrolNode;
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
		
		void Pursuit();
		void Reload();
		void Retreat();
		void SenseSomething();
		void UnderFire();
		
		// test
		void TestMicroActions();
		
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
		
		// miscellanious funtions	
		bool bfCheckForVisibility(CEntity* tpEntity);
		void vfLoadSounds();
		void vfLoadSelectors(CInifile *ini, const char *section);
		void vfAssignBones(CInifile *ini, const char *section);
		void vfCheckForPatrol();
		void vfLoadAnimations();
		bool bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
		bool bfCheckPath(AI::Path &Path);
		void SetLessCoverLook(NodeCompressed *tNode);
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
