////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.h
//	Created 	: 05.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN__
#define __XRAY_AI_HEN__

#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"
#include "ai_hen_selectors.h"

class CAI_Hen : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};

	enum EHenStates 	{
		aiHenDie = 0,
		aiHenUnderFire,
		aiHenSenseSomething,
		aiHenGoInThisDirection,
		aiHenGoToThisPosition,
		aiHenWaitOnPosition,
		aiHenHoldThisPosition,
		aiHenHoldPositionUnderFire,
		aiHenFreeHunting,
		aiHenFollowMe,
		aiHenAttack,
		aiHenDefend,
		aiHenPursuit,
		aiHenRetreat,
		aiHenCover,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		// media
		sound			sndHit[SND_HIT_COUNT];
		sound			sndDie[SND_DIE_COUNT];
		// ai
		EHenStates		eCurrentState;
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
		stack<EHenStates>	tStateStack;
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
	
		CHenSelectorAttack		SelectorAttack;
		CHenSelectorFreeHunting SelectorFreeHunting;
		CHenSelectorFollow		SelectorFollow;
		CHenSelectorPursuit		SelectorPursuit;
		CHenSelectorUnderFire	SelectorUnderFire;

		void SetLessCoverLook(NodeCompressed *tNode);
	public:
					   CAI_Hen();
		virtual		  ~CAI_Hen();
		virtual void  Update(DWORD DT);
		virtual void  HitSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  SenseSignal(int amount, Fvector& vLocalDir, CEntity* who);
		virtual void  Death();
		virtual void  Load( LPCSTR section );
		virtual void  Think();
		virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);
};
		
#endif