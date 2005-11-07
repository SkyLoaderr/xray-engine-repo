////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_trader.cpp
//	Created 	: 03.09.2003
//  Modified 	: 03.09.2003
//	Author		: Dmitriy Iassenev
//	Description : ALife trader class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "alife_artefact_order.h"

void CSE_ALifeTrader::on_surge				()
{
	inherited1::on_surge		();
	m_tpOrderedArtefacts.clear	();
}

void CSE_ALifeTrader::spawn_supplies	()
{
	inherited1::spawn_supplies	();
	inherited2::spawn_supplies	();
}

u32	CSE_ALifeTrader::dwfGetItemCost		(CSE_ALifeInventoryItem *tpALifeInventoryItem)
{
#pragma todo("Dima to Dima : correct price for non-artefact objects")
	CSE_ALifeItemArtefact		*l_tpALifeItemArtefact = smart_cast<CSE_ALifeItemArtefact*>(tpALifeInventoryItem);
	if (!l_tpALifeItemArtefact)
		return					(tpALifeInventoryItem->m_dwCost);

	u32							l_dwPurchasedCount = 0;
#pragma todo("Dima to Dima : optimize this cycle by keeping additional data structure with bought items")
	{
		ALife::OBJECT_IT		i = children.begin();
		ALife::OBJECT_IT		e = children.end();
		for ( ; i != e; ++i)
			if (!xr_strcmp(ai().alife().objects().object(*i)->s_name,l_tpALifeItemArtefact->s_name))
				++l_dwPurchasedCount;
	}

	ALife::ARTEFACT_TRADER_ORDER_PAIR_IT	J = m_tpOrderedArtefacts.find(*tpALifeInventoryItem->base()->s_name);
	if ((m_tpOrderedArtefacts.end() != J) && (l_dwPurchasedCount < (*J).second->m_dwTotalCount)) {
		ALife::ARTEFACT_ORDER_IT		I = (*J).second->m_tpOrders.begin();
		ALife::ARTEFACT_ORDER_IT		E = (*J).second->m_tpOrders.end();
		for ( ; I != E; ++I) {
			if (l_dwPurchasedCount <= (*I).m_count)
				return			((*I).m_price);
			else
				l_dwPurchasedCount -= (*I).m_count;
		}
		Debug.fatal				("Data synchronization mismatch");
	}
	return						(tpALifeInventoryItem->m_dwCost);
}

void CSE_ALifeTrader::add_online		(const bool &update_registries)
{
	CSE_ALifeTraderAbstract::add_online	(update_registries);
}

void CSE_ALifeTrader::add_offline		(const xr_vector<ALife::_OBJECT_ID> &saved_children, const bool &update_registries)
{
	CSE_ALifeTraderAbstract::add_offline(saved_children,update_registries);
}
