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

class CAI_Soldier : public CCustomMonster
{
	enum ESoundCcount {
		SND_HIT_COUNT=4,
		SND_DIE_COUNT=4
	};

	enum ESoldierStates 	{
		/**
		aiSoldierAttackRun = 0,
		aiSoldierAttackFire,
		aiSoldierDefend,
		aiSoldierFindEnemy,
		aiSoldierFollowLeader,
		aiSoldierFreeHunting,
		aiSoldierInjuring,
		
		aiSoldierSitting,

		aiSoldierMoreDeadThanAlive,
		
		aiSoldierPatrolHurtAggressiveUnderFire,
		aiSoldierPatrolHurtNonAggressiveUnderFire,
		aiSoldierPatrolUnderFire,

		aiSoldierPursuit,
		aiSoldierRetreat,
		aiSoldierUnderFire,
		
		aiSoldierJumping,
		/**/
		aiSoldierDie,
		aiSoldierPatrolReturnToRoute,
		aiSoldierPatrolRoute,
		aiSoldierFollowLeaderPatrol,
		aiSoldierSenseSomethingAlone,
		aiSoldierTurnOver,
		aiSoldierLookingOver,
		aiSoldierAttackFireAlone,
		aiSoldierRecharge,
		aiSoldierNoWeapon,
		aiSoldierSteal,
		aiSoldierWaitForAnimation,
		aiSoldierWaitForTime,
		//aiSoldierStandingUp,
		//aiSoldierLyingDown,
		aiSoldierAttackAim,
		aiSoldierPointAtSmth,
		aiSoldierPatrolHurt,
		aiSoldierDefendFireAlone,
		aiSoldierHurtAloneDefend,
		aiSoldierPatrolDanger,
		aiSoldierPursuitAlone,
	};
	
	enum EGestureStates {
		GESTURE_STATE_NONE=0,
		GESTURE_STATE_POINT,
		GESTURE_STATE_RAISE_HAND,
		GESTURE_STATE_GO_AHEAD,
	};

	enum EMovementTypes {
		WALK_FORWARD_0=char(0),
		WALK_FORWARD_1,
		WALK_FORWARD_2,
		WALK_FORWARD_3,
		WALK_FORWARD_4,
		RUN_FORWARD_0,
		RUN_FORWARD_1,
		RUN_FORWARD_2,
		RUN_FORWARD_3,
		WALK_BACK_0,
		WALK_BACK_1,
		WALK_BACK_2,
		WALK_LEFT,
		WALK_RIGHT,
		WALK_NO,
	};

	typedef	CCustomMonster inherited;

	protected:
		
		vector<SDynamicObject>	tpaDynamicObjects;
		vector<SDynamicSound>	tpaDynamicSounds;
		DWORD					m_dwMaxDynamicObjectsCount;
		DWORD					m_dwMaxDynamicSoundsCount;
		float					m_fSensetivity;
		int						m_iSoundIndex;

		////////////////////////////////////////////////////////////////////////////
		// normal animations
		////////////////////////////////////////////////////////////////////////////

/************************************************************************
/**/

/************************************************************************/
		
		// global animations
		typedef struct tagSNormalGlobalAnimations{
			CMotionDef* tpaDeath[5];
			CMotionDef* tpaRunForward[4];
			CMotionDef* tpaWalkForward[5];
			CMotionDef* tpaWalkBack[3];
			CMotionDef* tpaLieDown[2];
			CMotionDef* tpWalkLeft;
			CMotionDef* tpWalkRight;
			CMotionDef* tpPointSign; // !
			CMotionDef* tpIdle;
			CMotionDef* tpTurnLeft;
			CMotionDef* tpTurnRight;
			CMotionDef* tpAim;
			CMotionDef* tpAttack;
			CMotionDef* tpReload;
		}SNormalGlobalAnimations;

		// torso animations
		typedef struct tagSNormalTorsoAnimations{
			CMotionDef* tpDamageLeft;
			CMotionDef* tpDamageRight;
			CMotionDef* tpRaiseHandSign;
			CMotionDef* tpGoAheadSign;
		}SNormalTorsoAnimations;

		// legs animations
		typedef struct tagSNormalLegsAnimations{
			CMotionDef* tpJumpBegin;
			CMotionDef* tpJumpIdle;
		}SNormalLegsAnimations;

		// normal animations
		typedef struct tagSNormalAnimations{
			SNormalGlobalAnimations tGlobal;
			SNormalTorsoAnimations  tTorso;
			SNormalLegsAnimations	tLegs;
		}SNormalAnimations;

		////////////////////////////////////////////////////////////////////////////
		// crouch animations
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSCrouchGlobalAnimations{
			CMotionDef* tpDeath;
			CMotionDef* tpTurnLeft;
			CMotionDef* tpTurnRight;
			CMotionDef* tpIdle;
			CMotionDef* tpaLieDown[2];
			CMotionDef* tpaWalkForward[2];
			CMotionDef* tpaWalkBack[2];
			CMotionDef* tpWalkLeft;
			CMotionDef* tpWalkRight;
			CMotionDef* tpPointSign;
			CMotionDef* tpAim;
			CMotionDef* tpAttack;
			CMotionDef* tpReload;
		}SCrouchGlobalAnimations;

		// torso animations
		typedef struct tagSCrouchTorsoAnimations{
			CMotionDef* tpDamageLeft;
			CMotionDef* tpDamageRight;
			CMotionDef* tpRaiseHandSign;
			CMotionDef* tpGoAheadSign;
		}SCrouchTorsoAnimations;

		// legs animations
		typedef struct tagSCrouchLegsAnimations{
			CMotionDef* tpJumpBegin;
			CMotionDef* tpJumpIdle;
		}SCrouchLegsAnimations;

		// crouch animations
		typedef struct tagSCrouchAnimations{
			SCrouchGlobalAnimations	tGlobal;
			SCrouchTorsoAnimations  tTorso;
			SCrouchLegsAnimations	tLegs;
		}SCrouchAnimations;

		////////////////////////////////////////////////////////////////////////////
		// lie animations
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSLieGlobalAnimations{
			CMotionDef* tpDeath;
			CMotionDef* tpStandUp;
			CMotionDef* tpIdle;
			CMotionDef* tpReload;
			CMotionDef* tpAttack;
			CMotionDef* tpDamage;
			CMotionDef* tpTurnLeft;
			CMotionDef* tpTurnRight;
			CMotionDef* tpWalkForward;
			CMotionDef* tpWalkBack;
			CMotionDef* tpWalkLeft;
			CMotionDef* tpWalkRight;
			CMotionDef* tpRaiseHandSign;
			CMotionDef* tpGoAheadSign;
			CMotionDef* tpPointSign;
		}SLieGlobalAnimations;

		// lie animations
		typedef struct tagSLieAnimations{
			SLieGlobalAnimations	tGlobal;
		}SLieAnimations;

		////////////////////////////////////////////////////////////////////////////
		// soldier animations
		////////////////////////////////////////////////////////////////////////////

		typedef struct tagSSoldierAnimations{
			SNormalAnimations	tNormal;
			SCrouchAnimations	tCrouch;
			SLieAnimations		tLie;
		} SSoldierAnimations;

		SSoldierAnimations	tSoldierAnimations;
		CMotionDef*			m_tpCurrentGlobalAnimation;
		CMotionDef*			m_tpCurrentTorsoAnimation;
		CMotionDef*			m_tpCurrentLegsAnimation;
		CMotionDef*			m_tpAnimationBeingWaited;
		CBlend*				m_tpCurrentGlobalBlend;
		CBlend*				m_tpCurrentTorsoBlend;
		CBlend*				m_tpCurrentLegsBlend;
		char				m_cGestureState;
		//float				m_fAddAngle;
		DWORD				m_dwTimeBeingWaited;
		char				m_cMovementType;
		CMotionDef*			m_tpaMovementAnimations[3][WALK_NO];
		
		// head turns
		static void __stdcall HeadSpinCallback(CBoneInstance*);
		static void __stdcall SpineSpinCallback(CBoneInstance*);
		
		// mea
		sound			sndHit[SND_HIT_COUNT];
		sound			sndDie[SND_DIE_COUNT];
		
		// events
		EVENT			m_tpEventSay;
		EVENT			m_tpEventAssignPath;
		
		// ai
		ESoldierStates	eCurrentState;
		ESoldierStates	m_ePreviousState;
		bool			bStopThinking;
		DWORD			m_dwLastUpdate;
//		float			m_fAddAngle;
		
		// action data
		bool			m_bJumping;
		
		// hit data
		DWORD			dwHitTime;
		Fvector			tHitDir;
		Fvector			tHitPosition;
		
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
		float			m_fLateralMultiplier;
		float			m_fShadowWeight;
		
		// firing
		bool			m_bFiring;
		DWORD			m_dwStartFireAmmo;
		DWORD			m_dwNoFireTime;
		float			m_fAddWeaponAngle;
		
		// fire  constants
		DWORD			m_dwFireRandomMin;
		DWORD			m_dwFireRandomMax;
		DWORD			m_dwNoFireTimeMin;
		DWORD			m_dwNoFireTimeMax;
		float			m_fMinMissDistance;
		float			m_fMinMissFactor;
		float			m_fMaxMissDistance;
		float			m_fMaxMissFactor;

		
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
		CSoldierSelectorUnderFireCover		SelectorUnderFireCover;
		CSoldierSelectorUnderFireLine		SelectorUnderFireLine;

		/**
		void OnAttackRun();
		void OnAttackFire();
		
		void OnDefend();
		void OnFindEnemy();
		void OnFollowLeader();
		void OnFreeHunting();
		void OnInjuring();
		void OnMoreDeadThanAlive();

		void OnSitting();

		void OnPatrolHurtAggressiveUnderFire();
		void OnPatrolHurtNonAggressiveUnderFire();
		void OnPatrolUnderFire();
		
		void OnRetreat();
		void OnUnderFire();
		void OnJumping();
		/**/
		//void Test();
		void Die();
		void OnWaitingForAnimation();
		void OnWaitingForTime();
		void OnPatrol();
		void OnPatrolReturn();
		void OnFollowLeaderPatrol();
		void OnSenseSomethingAlone();
		void OnTurnOver();
		void OnLookingOver();
		void OnAttackFireAlone();
		void OnRecharge();
		void OnNoWeapon();
		void OnSteal();
		void OnPursuitAlone();
		//void OnLyingDown();
		//void OnStandingUp();
		void OnAttackAim();
		void OnPointAtSmth();
		void OnPatrolHurt();
		void OnDangerAlone();
		void OnDefendFireAlone();
		void OnHurtAloneDefend();

		// miscellanious funtions	
		SRotation tfGetOrientation(CEntity *tpEntity);
		void SelectSound(int &iIndex);
		void vfUpdateSounds(DWORD dwTimeDelta);
	IC  CGroup getGroup() {return Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];};
		bool bfCheckForEntityVisibility(CEntity *tpEntity);
		bool bfCheckForVisibility(CEntity* tpEntity);
		void vfLoadSounds();
		void vfLoadSelectors(LPCSTR section);
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
		void vfCheckForSavedEnemy();
		void vfUpdateDynamicObjects();
		void SetLook(Fvector tPosition);
		void vfSetMovementType(char cMovementType, float fMultiplier = 1.0f);
	IC	void vfSetLookAndFireMovement(bool a, DWORD c, float d, CGroup &Group, DWORD dwCurTime)
		{
			if (!(Group.m_bLessCoverLook)) {
				Group.m_bLessCoverLook = m_bLessCoverLook = true;
				Group.m_dwLastViewChange = dwCurTime;
			}
			else
				if ((m_bLessCoverLook) && (dwCurTime - Group.m_dwLastViewChange > 5000))
					Group.m_bLessCoverLook = m_bLessCoverLook = false;
			if (m_bLessCoverLook)
				SetLessCoverLook(AI_Node);
			else
				SetDirectionLook();
			vfSetFire(a,Group);
			vfSetMovementType(c,d);
		}

	public:
		bool		  m_bActionStarted;
					   CAI_Soldier();
		virtual		  ~CAI_Soldier();
		virtual void  Update(DWORD DT);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( LPCSTR section );
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
		virtual	void  soundEvent(CObject* who, int eType, Fvector& Position, float power);
};
		
#endif
