////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_selectors.h
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_SOLDIER_SELECTORS__
#define __XRAY_AI_SOLDIER_SELECTORS__

#include "..\\ai_monsters.h"

class CSoldierSelectorAttack : public CAISelectorBase
{
	public:
		CSoldierSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorDefend : public CAISelectorBase
{
	public:
		CSoldierSelectorDefend();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorFindEnemy : public CAISelectorBase
{
	public:
		CSoldierSelectorFindEnemy();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorFollowLeader : public CAISelectorBase
{
	public:
		CSoldierSelectorFollowLeader();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);

};

class CSoldierSelectorFreeHunting : public CAISelectorBase
{
	public:
		CSoldierSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorMoreDeadThanAlive : public CAISelectorBase
{
	public:
		CSoldierSelectorMoreDeadThanAlive();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorNoWeapon : public CAISelectorBase
{
	public:
		CSoldierSelectorNoWeapon();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorPursuit : public CAISelectorBase
{
	public:
		CSoldierSelectorPursuit();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorReload : public CAISelectorBase
{
	public:
		CSoldierSelectorReload();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorRetreat : public CAISelectorBase
{
	public:
		CSoldierSelectorRetreat();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorSenseSomething : public CAISelectorBase
{
	public:
		CSoldierSelectorSenseSomething();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CSoldierSelectorUnderFire : public CAISelectorBase
{
	public:
		CSoldierSelectorUnderFire();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif