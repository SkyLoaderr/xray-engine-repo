////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_selectors.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Selectors for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

CStalkerSelectorFreeHunting::CStalkerSelectorFreeHunting()
{
	Name = "selector_free_hunting"; 
}

float CStalkerSelectorFreeHunting::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	return(0.0f);
}

