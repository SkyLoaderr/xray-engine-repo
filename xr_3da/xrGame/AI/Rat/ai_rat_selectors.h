////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_selectors.h
//	Created 	: 23.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_RAT_SELECTORS__
#define __XRAY_AI_RAT_SELECTORS__

#include "..\\ai_monsters.h"

class CRatSelectorAttack : public CAISelectorBase
{
	public:
		CRatSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorDefend : public CAISelectorBase
{
	public:
		CRatSelectorDefend();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorFindEnemy : public CAISelectorBase
{
	public:
		CRatSelectorFindEnemy();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorFollowLeader : public CAISelectorBase
{
	public:
		CRatSelectorFollowLeader();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);

};

class CRatSelectorFreeHunting : public CAISelectorBase
{
	public:
		CRatSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorMoreDeadThanAlive : public CAISelectorBase
{
	public:
		CRatSelectorMoreDeadThanAlive();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorNoWeapon : public CAISelectorBase
{
	public:
		CRatSelectorNoWeapon();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorPatrol : public CAISelectorBase
{
	public:
		CRatSelectorPatrol();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorPursuit : public CAISelectorBase
{
	public:
		CRatSelectorPursuit();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorReload : public CAISelectorBase
{
	public:
		CRatSelectorReload();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorRetreat : public CAISelectorBase
{
	public:
		CRatSelectorRetreat();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorSenseSomething : public CAISelectorBase
{
	public:
		CRatSelectorSenseSomething();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorUnderFire : public CAISelectorBase
{
	public:
		CRatSelectorUnderFire();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif