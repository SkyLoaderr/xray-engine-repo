////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_human_.cpp
//	Created 	: 24.07.2003
//  Modified 	: 24.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation of humans
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"

bool CSE_ALifeHumanAbstract::HealthIsGood			()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::CanTreat				()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::EnoughMoneyToTreat		()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::EnoughTimeToTreat		()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::EnoughEquipmentToGo	()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::DistanceToTraderIsDanger()
{
	return(false);
}

bool CSE_ALifeHumanAbstract::EnoughMoneyToEquip		()
{
	return(true);
}