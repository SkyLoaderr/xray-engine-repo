////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_selectors.h
//	Created 	: 07.05.2002
//  Modified 	: 07.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI selectors for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_ZOMBIE_SELECTORS__
#define __XRAY_AI_ZOMBIE_SELECTORS__

#include "..\\ai_monsters.h"

class CZombieSelectorFreeHunting : public CAISelectorBase
{
	public:
		CZombieSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CZombieSelectorAttack : public CAISelectorBase
{
	public:
		CZombieSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif