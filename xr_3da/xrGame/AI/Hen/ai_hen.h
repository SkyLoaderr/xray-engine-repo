////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.h
//	Created 	: 05.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN__
#define __XRAY_AI_HEN__

#include "..\\..\\CustomMonster.h"
#include "group.h"

namespace AI {

	//#define WRITE_LOG

	enum HenStates 	{
		aiHenDie = 0,
		aiHenUnderFire,
		aiSenseSomething,
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
		aiLAST,
		aiFORCEDWORD = DWORD(-1)
	};
	
	struct EnemySelected
	{
		CEntity*	E;
		BOOL		Visible;
		float		Cost;
	};

	class HenState
	{
		// hit data
		DWORD	hitTime;
		Fvector	hitDir;
		// sense data
		DWORD	senseTime;
		Fvector	senseDir;

		BOOL	bBuildPathToLostEnemy;
		public:
			CEntity*	EnemySaved;			// жертва
			HenStates	Type;
			DWORD		svTime_Create;
			CEntity*	victim;				// жертва
			Fvector		PositionPredicted;
			Fvector		savedPosition;
			DWORD		savedTime;
			DWORD		savedNode;
			BOOL		bDirectPathBuilded;
			
			State(AI_States T) : Type(T);
			static State* Create(DWORD type);
			virtual BOOL Parse(CCustomMonster* Me) = 0;
			virtual void Hit(Fvector &dir);
			virtual void Sense(Fvector &dir);
			IC virtual float EnemyHeuristics(CCustomMonster* Me, CEntity* E)
			virtual void SelectEnemy(EnemySelected& S, CCustomMonster* Me)
	};

};

class CAI_Hen : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};
	
	typedef	CCustomMonster inherited;
	friend	class AI::HenState;

protected:
	// media
	sound3D				sndHit[SND_HIT_COUNT];
	sound3D				sndDie[SND_DIE_COUNT];
public:
	virtual void		Update					(DWORD DT);
	virtual void		HitSignal				(int amount, Fvector& vLocalDir, CEntity* who);
	virtual void		SenseSignal				(int amount, Fvector& vLocalDir, CEntity* who);
	virtual void		Die						();
	virtual void		Load					( CInifile* ini, const char* section );

	CAI_Hen			();
	virtual ~CAI_Hen	();
};

class HenSelectorBase : public AI::NodeEstimator 
{
	public:
		LPSTR		Name;
		float		fTravelWeight,
					fLightWeight,
					fCoverFromLeaderWeight,
					fCoverFromMembersWeight,
					fCoverFromEnemyWeight,
					fOptimalLeaderDistance,
					fLeaderDistanceWeight,
					fOptimalMemberDistance,
					fMemberDistanceWeight,
					fOptimalEnemyDistance,
					fEnemyDistanceWeight,
					fMaxHeightDistance,
					fMaxHeightDistanceWeight,
					fMemberViewDeviationWeight,
					fEnemyViewDeviationWeight,
					fTotalViewVectorWeight,
					fLaziness;
		CEntity		*tMe;
		EntityVec	taMembers;
		CEntity		*tEnemy;

		virtual	void Load(CInifile* ini, const char* section);
		//IC float h_factor(int s1, int s2, float f1, float f2, BYTE* data);
		//IC float h_slerp(Fvector& T, Fvector& S, BYTE* data);
};

#endif