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

bool CSE_ALifeHumanAbstract::HealthIsGood			(CSE_ALifeSimulator *tpALife)
{
	return(true);
}

bool CSE_ALifeHumanAbstract::bfItemCanTreat			(CSE_ALifeItem *tpALifeItem)
{
	return(false);
}

void CSE_ALifeHumanAbstract::vfUseItem				(CSE_ALifeItem *tpALifeItem)
{
}

bool CSE_ALifeHumanAbstract::CanTreat				(CSE_ALifeSimulator *tpALife)
{
	xr_vector<u16>::iterator	I = children.begin();
	xr_vector<u16>::iterator	E = children.end();
	for ( ; I != E; I++) {
		CSE_ALifeItem			*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(tpALife->tpfGetObjectByID(*I));
		if (l_tpALifeItem && bfItemCanTreat(l_tpALifeItem))
			vfUseItem			(l_tpALifeItem);
	}
	return						(HealthIsGood(tpALife));
}

bool CSE_ALifeHumanAbstract::EnoughMoneyToTreat		(CSE_ALifeSimulator *tpALife)
{
	return(true);
}

bool CSE_ALifeHumanAbstract::EnoughTimeToTreat		(CSE_ALifeSimulator *tpALife)
{
	return(true);
}

bool CSE_ALifeHumanAbstract::EnoughEquipmentToGo	(CSE_ALifeSimulator *tpALife)
{
	return(true);
}

bool CSE_ALifeHumanAbstract::DistanceToTraderIsDanger(CSE_ALifeSimulator *tpALife)
{
	return(false);
}

bool CSE_ALifeHumanAbstract::EnoughMoneyToEquip		(CSE_ALifeSimulator *tpALife)
{
	return(true);
}