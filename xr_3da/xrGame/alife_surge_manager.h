////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_surge_manager.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator surge manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_simulator_base.h"

class CSE_ALifeTrader;

class CALifeSurgeManager : public virtual CALifeSimulatorBase, CRandom {
protected:
	typedef CALifeSimulatorBase inherited;

protected:
	xr_vector<bool>					m_alive_spawn_objects;
	ALife::ORGANIZATION_ORDER_MAP	m_tpSoldArtefacts;
	ALife::_TIME_ID					m_last_surge_time;
	ALife::_TIME_ID					m_next_surge_time;
	ALife::ITEM_COUNT_MAP			m_tpTraderItems;

public:
	DWORD_VECTOR					m_tpTempPath;

protected:
			void			surge						();
			void			generate_anomalies			();
			void			generate_anomaly_map		();
			void			sell_artefacts				(CSE_ALifeTrader &trader);
			void			buy_supplies				(CSE_ALifeTrader &trader);
			void			give_military_bribe			(CSE_ALifeTrader &trader);
			void			kill_creatures				();
			void			ballance_creatures			();
			void			update_tasks				();
			void			assign_stalker_customers	();
			float			distance					(const DWORD_VECTOR &path) const;

public:
	IC						CALifeSurgeManager			(xrServer *server, LPCSTR section);
	virtual					~CALifeSurgeManager			();
	IC		ALife::_TIME_ID	last_surge_time				() const;
	IC		ALife::_TIME_ID	next_surge_time				() const;
};

#include "alife_surge_manager_inline.h"