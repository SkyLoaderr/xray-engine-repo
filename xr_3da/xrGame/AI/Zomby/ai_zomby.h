////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zomby.h
//	Created 	: 07.05.2002
//  Modified 	: 07.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "ßùüèí"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_ZOMBY__
#define __XRAY_AI_ZOMBY__

#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"
#include "ai_zomby_selectors.h"

class CAI_Zomby : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};

	enum EZombyStates 	{
		aiZombyDie = 0,
		aiZombyUnderFire,
		aiZombySenseSomething,
		aiZombyGoInThisDirection,
		aiZombyGoToThisPosition,
		aiZombyWaitOnPosition,
		aiZombyHoldThisPosition,
		aiZombyHoldPositionUnderFire,
		aiZombyFreeHunting,
		aiZombyFollowMe,
		aiZombyAttack,
		aiZombyDefend,
		aiZombyPursuit,
		aiZombyRetreat,
		aiZombyCover,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		// media
		sound3D			sndHit[SND_HIT_COUNT];
		sound3D			sndDie[SND_DIE_COUNT];
		// ai
		EZombyStates		eCurrentState;
		bool			bStopThinking;
		// hit data
		DWORD			dwHitTime;
		Fvector			tHitDir;
		// sense data
		DWORD			dwSenseTime;
		Fvector			tSenseDir;
		// saved enemy
		CEntity*		tSavedEnemy;
		Fvector			tSavedEnemyPosition;
		DWORD			dwLostEnemyTime;
		NodeCompressed* tpSavedEnemyNode;
		DWORD			dwSavedEnemyNodeID;
		bool			bBuildPathToLostEnemy;
		// finite state machine
		stack<EZombyStates>	tStateStack;
		void Die();
		void UnderFire();
		void SenseSomething();
		void GoInThisDirection();
		void GoToThisPosition();
		void WaitOnPosition();
		void HoldThisPosition();
		void HoldPositionUnderFire();
		void FreeHunting();
		void FollowMe();
		void Attack();
		void Defend();
		void Pursuit();
		void Retreat();
		void Cover();
		IC bool bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
		bool bfCheckPath(AI::Path &Path);
	
		CZombySelectorAttack		SelectorAttack;
		CZombySelectorFreeHunting SelectorFreeHunting;
		CZombySelectorFollow		SelectorFollow;
		CZombySelectorPursuit		SelectorPursuit;
		CZombySelectorUnderFire	SelectorUnderFire;

		void SetLessCoverLook(NodeCompressed *tNode);
	public:
					   CAI_Zomby();
		virtual		  ~CAI_Zomby();
		virtual void  Update(DWORD DT);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  SenseSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( CInifile* ini, const char* section );
		virtual void  Think();
		virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);
};
		
#endif