////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_schedule_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife schedule registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "safe_map_iterator.h"
#include "xrServer_Objects_ALife.h"

class CALifeScheduleRegistry : public CSafeMapIterator<ALife::_OBJECT_ID,CSE_ALifeSchedulable> {
private:
	struct CUpdatePredicate {
		IC	bool	operator()	(_iterator &i, u64 cycle_count, bool) const
		{
			if ((*i).second->m_schedule_counter	== cycle_count)
				return					(false);

			(*i).second->m_schedule_counter	= cycle_count;
			return						(true);
		}

		IC	void	operator()	(_iterator &i, u64 cycle_count) const
		{
			(*i).second->update			();
		}
	};

protected:
	typedef CSafeMapIterator<ALife::_OBJECT_ID,CSE_ALifeSchedulable> inherited;

public:
	virtual							~CALifeScheduleRegistry	();
			void					add						(CSE_ALifeDynamicObject *object);
			void					remove					(CSE_ALifeDynamicObject *object, bool no_assert = false);
	IC		void					update					();
	IC		CSE_ALifeSchedulable	*object					(const ALife::_OBJECT_ID &id, bool no_assert = false) const;
};

#include "alife_schedule_registry_inline.h"