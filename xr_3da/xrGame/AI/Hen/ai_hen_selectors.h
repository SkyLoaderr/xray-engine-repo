////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.h
//	Created 	: 05.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_SELECTORS__
#define __XRAY_AI_HEN_SELECTORS__

#include "ai_hen.h"

class CHenSelectorBase : public AI::NodeEstimator 
{
	public:
		LPSTR		Name;
		float		fTravelWeight,
					fLightWeight,
					fLaziness,
					fTotalViewVectorWeight,
					fCoverFromLeaderWeight,
					fOptLeaderDistance,
					fOptLeaderDistanceWeight,
					fMinLeaderDistance,
					fMinLeaderDistanceWeight,
					fMaxLeaderDistance,
					fMaxLeaderDistanceWeight,
					fLeaderViewDeviationWeight,
					fMaxLeaderHeightDistance,
					fMaxLeaderHeightDistanceWeight,
					fCoverFromMemberWeight,
					fOptMemberDistance,
					fOptMemberDistanceWeight,
					fMinMemberDistance,
					fMinMemberDistanceWeight,
					fMaxMemberDistance,
					fMaxMemberDistanceWeight,
					fMemberViewDeviationWeight,
					fCoverFromEnemyWeight,
					fOptEnemyDistance,
					fOptEnemyDistanceWeight,
					fMinEnemyDistance,
					fMinEnemyDistanceWeight,
					fMaxEnemyDistance,
					fMaxEnemyDistanceWeight,
					fEnemyViewDeviationWeight;

		CAI_Hen		*tMe;
		CEntity		*tLeader;
		CEntity		*tEnemy;
		Fvector		tMyPosition;
		Fvector		tLeaderPosition;
		Fvector		tEnemyPosition;

		virtual	void Load(CInifile* ini, const char* section);
};

class CHenSelectorAttack : public CHenSelectorBase
{
	public:
		CHenSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CHenSelectorFreeHunting : public CHenSelectorBase
{
	public:
		CHenSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CHenSelectorFollow : public CHenSelectorBase
{
	public:
		CHenSelectorFollow	();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);

};

class CHenSelectorPursuit : public CHenSelectorBase
{
	public:
		CHenSelectorPursuit();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CHenSelectorUnderFire : public CHenSelectorBase
{
	public:
		CHenSelectorUnderFire();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif