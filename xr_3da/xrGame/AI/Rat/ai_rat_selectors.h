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

class CRatSelectorFreeHunting : public CAISelectorBase
{
	public:
		CRatSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorAttack : public CAISelectorBase
{
	public:
		CRatSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif