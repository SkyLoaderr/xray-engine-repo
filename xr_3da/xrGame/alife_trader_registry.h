////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_trader_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife trader registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Objects_ALife_Monsters.h"

class CSE_ALifeHumanAbstract;

class CALifeTraderRegistry {
public:
	typedef xr_vector<CSE_ALifeTrader*> TRADER_REGISTRY;

public:
	struct CCompareTraderRanksPredicate {
		IC	bool					operator()	(const CSE_ALifeTrader *tpALifeTrader1, const CSE_ALifeTrader *tpALifeTrader2) const
		{
			return					(tpALifeTrader1->m_tRank < tpALifeTrader2->m_tRank);
		};
	};

protected:
	TRADER_REGISTRY					m_traders;

public:
	virtual							~CALifeTraderRegistry	();
			CSE_ALifeTrader			*trader_nearest			(CSE_ALifeHumanAbstract *human);
			void					add						(CSE_ALifeDynamicObject *object);
	IC		const TRADER_REGISTRY	&traders				() const;
};

#include "alife_trader_registry_inline.h"