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
		aiSoldierFollowLeaderPatrol,
		aiSoldierFreeHunting,
		aiSoldierInjuring,
		aiSoldierJumping,
		aiSoldierMoreDeadThanAlive,
		aiSoldierNoWeapon,
		aiSoldierPatrolDetour,
		aiSoldierPursuit,
		aiSoldierReload,
		aiSoldierRetreat,
		aiSoldierSenseSomething,
		aiSoldierUnderFire,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		
		// macroses
		#define MIN_RANGE_SEARCH_TIME_INTERVAL	15000.f
		#define MAX_TIME_RANGE_SEARCH			150000.f
		#define	FIRE_ANGLE						PI/30
		#define	FIRE_SAFETY_ANGLE				PI/30
		#define CUBE(x)							((x)*(x)*(x))
		#define LEFT_NODE(Index)				((Index + 3) & 3)
		#define RIGHT_NODE(Index)				((Index + 5) & 3)
		#define FN(i)							(float(tNode->cover[(i)])/255.f)
		#define	AMMO_NEED_RELOAD				6
		#define	MAX_HEAD_TURN_ANGLE				(PI/3.f)
		#define VECTOR_DIRECTION(i)				(i == 0 ? tLeft : (i == 1 ? tForward : (i == 2 ? tRight : tBack)))
		#define EYE_WEAPON_DELTA				(PI/30.f)
		#define TORSO_ANGLE_DELTA				(PI/30.f)
		#define INIT_SQUAD_AND_LEADER \
			CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];\
			CEntity* Leader = Squad.Leader;\
			if (Leader->g_Health() <= 0)\
				Leader = this;\
			R_ASSERT (Leader);
//#define WRITE_LOG

		// head turns
		static void __stdcall HeadSpinCallback(CBoneInstance*);

		// media
		sound3D			sndHit[SND_HIT_COUNT];
		sound3D			sndDie[SND_DIE_COUNT];
		CMotionDef* 	m_tpaDeathAnimations[2];
		
		// ai
		ESoldierStates	eCurrentState;
		bool			bStopThinking;
		
		// hit data
		DWORD			dwHitTime;
		Fvector			tHitDir;
		
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

		// firing
		bool			m_bFiring;

		// patrol array
		vector<DWORD>	tpaPatrolPoints;
		int				m_iCurrentPoint;
		
		// finite state machine
		stack<ESoldierStates>	tStateStack;
		
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
		void FollowLeaderPatrol();
		void FreeHunting();
		void Injuring();
		void Jumping();
		void MoreDeadThanAlive();
		void NoWeapon();
		void Patrol();
		void Pursuit();
		void Reload();
		void Retreat();
		void SenseSomething();
		void UnderFire();

		// miscellanious funtions	
	IC	bool bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
		bool bfCheckPath(AI::Path &Path);
		void SetLessCoverLook(NodeCompressed *tNode);
		void SetSmartLook(NodeCompressed *tNode, Fvector &tEnemyDirection);
		void vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfBuildPathToDestinationPoint(CSoldierSelectorAttack *S);
		void vfSearchForBetterPosition(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfSearchForBetterPositionWTime(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfAimAtEnemy();
		void vfSaveEnemy();
		bool bfCheckIfCanKillMember();
	IC	bool bfCheckIfCanKillEnemy();
		void vfSetFire(bool bFire, CGroup &Group);
		void vfSetMovementType(bool bCrouched, float fSpeed);
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
		virtual float OnVisible(); 
		virtual objQualifier* GetQualifier	();
		
};
		
#endif