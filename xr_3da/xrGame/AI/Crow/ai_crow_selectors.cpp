////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow_selectors.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\entity.h"
#include "..\\..\\CustomMonster.h"
#include "ai_crow_selectors.h"

CCrowSelectorFreeHunting::CCrowSelectorFreeHunting()
{ 
	Name = "selector_free_hunting"; 
}

float CCrowSelectorFreeHunting::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddDeviationFromPreviousDirectionCost();
	CHECK_RESULT;
	//vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}
