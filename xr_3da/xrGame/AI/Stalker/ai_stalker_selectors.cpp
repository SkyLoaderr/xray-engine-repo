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
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	return(m_tEnemyPosition.distance_to(m_tCurrentPosition)*1000.f);
}

CStalkerSelectorReload::CStalkerSelectorReload()
{ 
	Name = "selector_reload"; 
}

float CStalkerSelectorReload::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	vfAddCoverFromEnemyCost();
	return(m_fResult);
}

