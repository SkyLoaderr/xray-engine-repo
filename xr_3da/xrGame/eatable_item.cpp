////////////////////////////////////////////////////////////////////////////
//	Module 		: eatable_item.cpp
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Yuri Dobronravin
//	Description : Eatable item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "eatable_item.h"

///////////////////////////////////////////
// CEatableItem class 
///////////////////////////////////////////

CEatableItem::CEatableItem()
{
	m_fHealthInfluence = 0;
	m_fPowerInfluence = 0;
	m_fSatietyInfluence = 0;
	m_fRadiationInfluence = 0;

	m_iPortionsNum = -1;
}
CEatableItem::~CEatableItem()
{
}
void CEatableItem::Load(LPCSTR section)
{
	inherited::Load(section);

	m_fHealthInfluence = pSettings->r_float(section, "eat_health");
	m_fPowerInfluence = pSettings->r_float(section, "eat_power");
	m_fSatietyInfluence = pSettings->r_float(section, "eat_satiety");
	m_fRadiationInfluence = pSettings->r_float(section, "eat_radiation");

	m_iPortionsNum = pSettings->r_s32(section, "eat_portions_num");
}

bool CEatableItem::Useful() const
{
	if(!inherited::Useful()) return false;

	//проверить не все ли еще съедено
	if(m_iPortionsNum == 0) return false;

	return true;
}