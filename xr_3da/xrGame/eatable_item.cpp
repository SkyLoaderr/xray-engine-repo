////////////////////////////////////////////////////////////////////////////
//	Module 		: eatable_item.cpp
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Yuri Dobronravin
//	Description : Eatable item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "eatable_item.h"
#include "xrmessages.h"
#include "net_utils.h"
#include "physic_item.h"
#include "Level.h"

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

	m_physic_item	= 0;
}

CEatableItem::~CEatableItem()
{
}

DLL_Pure *CEatableItem::_construct	()
{
	m_physic_item	= smart_cast<CPhysicItem*>(this);
	return			(inherited::_construct());
}

void CEatableItem::Load(LPCSTR section)
{
	inherited::Load(section);

	m_fHealthInfluence = pSettings->r_float(section, "eat_health");
	m_fPowerInfluence = pSettings->r_float(section, "eat_power");
	m_fSatietyInfluence = pSettings->r_float(section, "eat_satiety");
	m_fRadiationInfluence = pSettings->r_float(section, "eat_radiation");
	m_fWoundsHealPerc	= pSettings->r_float(section, "wounds_heal_perc");
	clamp(m_fWoundsHealPerc, 0.f, 1.f);
	
	m_iStartPortionsNum = pSettings->r_s32(section, "eat_portions_num");
	VERIFY(m_iPortionsNum<10000);
}

BOOL CEatableItem::net_Spawn				(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC)) return FALSE;

	m_iPortionsNum = m_iStartPortionsNum;

	return TRUE;
};

bool CEatableItem::Useful() const
{
	if(!inherited::Useful()) return false;

	//проверить не все ли еще съедено
	if(m_iPortionsNum == 0) return false;

	return true;
}

void CEatableItem::OnH_A_Independent() 
{
	inherited::OnH_A_Independent();
	if(!Useful()) {
		NET_Packet		P;
		object().u_EventGen		(P,GE_DESTROY,object().ID());
		
		//Msg				("ge_destroy: [%d] - %s",ID(),*cName());
		if (object().Local() && OnServer())	object().u_EventSend	(P);
	}	
}

void CEatableItem::OnH_B_Independent()
{
	if(!Useful()) 
	{
		object().setVisible(false);
		object().setEnabled(false);
		if (m_physic_item)
			m_physic_item->m_ready_to_destroy	= true;
	}
	inherited::OnH_B_Independent();
}
