////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_selectors.h
//	Created 	: 07.05.2002
//  Modified 	: 07.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_ZOMBIE_SELECTORS__
#define __XRAY_AI_ZOMBIE_SELECTORS__

#include "..\\ai_monsters.h"

class CZombieSelectorAttack : public CAISelectorBase
{
	public:
		CZombieSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorDefend : public CAISelectorBase
{
	public:
		CZombieSelectorDefend();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorFindEnemy : public CAISelectorBase
{
	public:
		CZombieSelectorFindEnemy();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorFollowLeader : public CAISelectorBase
{
	public:
		CZombieSelectorFollowLeader();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);

};

class CZombieSelectorFreeHunting : public CAISelectorBase
{
	public:
		CZombieSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorMoreDeadThanAlive : public CAISelectorBase
{
	public:
		CZombieSelectorMoreDeadThanAlive();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorNoWeapon : public CAISelectorBase
{
	public:
		CZombieSelectorNoWeapon();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorPatrol : public CAISelectorBase
{
	public:
		CZombieSelectorPatrol();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorPursuit : public CAISelectorBase
{
	public:
		CZombieSelectorPursuit();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorReload : public CAISelectorBase
{
	public:
		CZombieSelectorReload();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorRetreat : public CAISelectorBase
{
	public:
		CZombieSelectorRetreat();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorSenseSomething : public CAISelectorBase
{
	public:
		CZombieSelectorSenseSomething();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorUnderFire : public CAISelectorBase
{
	public:
		CZombieSelectorUnderFire();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif