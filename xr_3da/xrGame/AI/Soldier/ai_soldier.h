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
		aiSoldierAttack = 0,
		aiSoldierDie,
		aiSoldierDefend,
		aiSoldierFreeHunting,
		aiSoldierHurt,
		aiSoldierJumping,
		aiSoldierMoreDeadThanAlive,
		aiSoldierPursuit,
		aiSoldierReload,
		aiSoldierRetreat,
		aiSoldierSenseSomething,
		aiSoldierUnderFire,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		
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
		
		// finite state machine
		stack<ESoldierStates>	tStateStack;
		
		CSoldierSelectorAttack			SelectorAttack;
		CSoldierSelectorFindEnemy		SelectorFindEnemy;
		CSoldierSelectorFreeHunting		SelectorFreeHunting;
		CSoldierSelectorFollow			SelectorFollow;
		CSoldierSelectorPursuit			SelectorPursuit;
		CSoldierSelectorUnderFire		SelectorUnderFire;
		CSoldierSelectorWeaponReload	SelectorWeaponReload;

		void Die();
		void UnderFire();
		void SenseSomething();
		void FreeHunting();
		void FollowMe();
		void Attack();
		void Pursuit();
		void WeaponReload();

		// miscellanious funtions	
	IC	bool bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
		bool bfCheckPath(AI::Path &Path);
		void SetLessCoverLook(NodeCompressed *tNode);
		void SetSmartLook(NodeCompressed *tNode, Fvector &tEnemyDirection);
		void vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfBuildPathToDestinationPoint(CSoldierSelectorAttack *S);
		void vfSearchForBetterPosition(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
		void vfAimAtEnemy();
		bool bfCheckIfCanKillMember(CAISelectorBase &S, CEntity* &Leader);
	IC	bool bfCheckIfCanKillEnemy();
		void vfSetFire(bool bFire, CAISelectorBase &S, CEntity* &Leader);
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