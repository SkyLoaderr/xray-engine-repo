////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_selectors.h
//	Created 	: 05.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_SELECTORS__
#define __XRAY_AI_HEN_SELECTORS__

#include "..\\ai_monsters.h"

class CHenSelectorAttack : public CAISelectorBase
{
	public:
		CHenSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CHenSelectorFreeHunting : public CAISelectorBase
{
	public:
		CHenSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CHenSelectorFollow : public CAISelectorBase
{
	public:
		CHenSelectorFollow	();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);

};

class CHenSelectorPursuit : public CAISelectorBase
{
	public:
		CHenSelectorPursuit();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CHenSelectorUnderFire : public CAISelectorBase
{
	public:
		CHenSelectorUnderFire();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif