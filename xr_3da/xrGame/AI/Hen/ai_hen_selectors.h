////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.h
//	Created 	: 05.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_SELECTORS__
#define __XRAY_AI_HEN_SELECTORS__

class CHenSelectorBase : public AI::NodeEstimator 
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