////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.h
//	Created 	: 23.04.2002
//  Modified 	: 23.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_RAT__
#define __XRAY_AI_RAT__

#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"

#include "ai_rat_selectors.h"

const DWORD _FB_hit_RelevantTime	= 10;
const DWORD _FB_sense_RelevantTime	= 10;
const float _FB_look_speed			= PI;
const float _FB_invisible_hscale	= 2.f;

struct SEnemySelected
{
	CEntity*	Enemy;
	bool		bVisible;
	float		fCost;
};

class CAI_Rat : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};

	enum EHenStates 	{
		aiRatDie = 0,
		aiRatUnderFire,
		aiRatSenseSomething,
		aiRatGoInThisDirection,
		aiRatGoToThisPosition,
		aiRatWaitOnPosition,
		aiRatHoldThisPosition,
		aiRatHoldPositionUnderFire,
		aiRatFreeHunting,
		aiRatFollowMe,
		aiRatAttack,
		aiRatDefend,
		aiRatPursuit,
		aiRatRetreat,
		aiRatCover,
	};
	
	typedef	CCustomMonster inherited;

	protected:
		// media
		sound3D			sndHit[SND_HIT_COUNT];
		sound3D			sndDie[SND_DIE_COUNT];
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
	
		CRatSelectorAttack		SelectorAttack;
		CRatSelectorFreeHunting SelectorFreeHunting;
		CRatSelectorFollow		SelectorFollow;
		CRatSelectorPursuit		SelectorPursuit;
		CRatSelectorUnderFire	SelectorUnderFire;

		void SetLessCoverLook(NodeCompressed *tNode);
	public:
					  CAI_Rat();
		virtual		  ~CAI_Rat();
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