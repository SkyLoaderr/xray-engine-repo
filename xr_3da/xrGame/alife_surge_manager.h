////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_surge_manager.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator surge manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_simulator_base.h"
#include "xrserver_space.h"

class CSE_ALifeTrader;

class CALifeSurgeManager : public virtual CALifeSimulatorBase, CRandom {
protected:
	typedef CALifeSimulatorBase inherited;

protected:
	ALife::ORGANIZATION_ORDER_MAP	m_tpSoldArtefacts;
	ALife::ITEM_COUNT_MAP			m_tpTraderItems;
	xr_vector<ALife::_OBJECT_ID>	m_temp_objects;
	xr_vector<ALife::_SPAWN_ID>		m_temp_spawns;

public:
	DWORD_VECTOR					m_tpTempPath;

private:
	IC		bool			redundant					(CSE_ALifeDynamicObject *object);
	IC		void			process_spawns				();

protected:
			void			surge						();
			void			generate_anomaly_map		();
			void			sell_artefacts				(CSE_ALifeTrader &trader);
			void			buy_supplies				(CSE_ALifeTrader &trader);
			void			give_military_bribe			(CSE_ALifeTrader &trader);
			void			kill_creatures				();
			void			update_tasks				();
			void			assign_stalker_customers	();
			void			remove_redundant_objects	();
			void			spawn_new_objects			();
			void			update_objects_before_surge	();
			void			update_traders				();
			void			update_organizations		();
			void			setup_next_surge_time		();
			float			distance					(const DWORD_VECTOR &path) const;
			void			fill_redundant_spawns		();
			void			fill_redundant_objects		();
			void			release_redundant_objects	();
			void			fill_new_spawns				();
			void			spawn_new_spawns			();

public:
	IC						CALifeSurgeManager			(xrServer *server, LPCSTR section);
	virtual					~CALifeSurgeManager			();
};

#include "alife_surge_manager_inline.h"