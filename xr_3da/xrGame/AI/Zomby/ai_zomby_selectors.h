////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zomby_selectors.h
//	Created 	: 07.05.2002
//  Modified 	: 07.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zomby"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_ZOMBY_SELECTORS__
#define __XRAY_AI_ZOMBY_SELECTORS__

#include "..\\ai_monsters.h"

class CZombySelectorAttack : public CAISelectorBase
{
	public:
		CZombySelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombySelectorFreeHunting : public CAISelectorBase
{
	public:
		CZombySelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombySelectorFollow : public CAISelectorBase
{
	public:
		CZombySelectorFollow	();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);

};

class CZombySelectorPursuit : public CAISelectorBase
{
	public:
		CZombySelectorPursuit();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombySelectorUnderFire : public CAISelectorBase
{
	public:
		CZombySelectorUnderFire();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif