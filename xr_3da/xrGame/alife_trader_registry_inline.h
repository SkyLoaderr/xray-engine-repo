////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_trader_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife trader registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CALifeTraderRegistry::TRADER_REGISTRY &CALifeTraderRegistry::traders	() const
{
	return					(m_traders);
}

IC	void CALifeTraderRegistry::add	(CSE_ALifeDynamicObject *object)
{
	CSE_ALifeTrader			*trader = smart_cast<CSE_ALifeTrader*>(object);
	if (trader) {
		m_traders.push_back	(trader);
		std::sort			(m_traders.begin(),m_traders.end(),CCompareTraderRanksPredicate());
	}
}
