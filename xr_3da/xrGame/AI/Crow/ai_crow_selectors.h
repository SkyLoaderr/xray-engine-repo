////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow_selectors.h
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_CROW_SELECTORS__
#define __XRAY_AI_CROW_SELECTORS__

#include "..\\ai_monsters.h"

class CCrowSelectorFreeHunting : public CAISelectorBase
{
	public:
		CCrowSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif