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

bool CSE_ALifeHumanAbstract::bfHealthIsGood			()
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

bool CSE_ALifeHumanAbstract::bfCanTreat				()
{
	xr_vector<u16>::iterator	I = children.begin();
	xr_vector<u16>::iterator	E = children.end();
	for ( ; I != E; I++) {
		CSE_ALifeItem			*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(m_tpALife->tpfGetObjectByID(*I));
		if (l_tpALifeItem && bfItemCanTreat(l_tpALifeItem))
			vfUseItem			(l_tpALifeItem);
	}
	return						(bfHealthIsGood());
}

bool CSE_ALifeHumanAbstract::bfEnoughMoneyToTreat		()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::bfEnoughTimeToTreat		()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::bfEnoughEquipmentToGo	()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::bfDistanceToTraderIsDanger()
{
	return(false);
}

bool CSE_ALifeHumanAbstract::bfEnoughMoneyToEquip		()
{
	return(true);
}