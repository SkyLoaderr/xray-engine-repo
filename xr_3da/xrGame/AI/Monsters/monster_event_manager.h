#pragma once
#include "monster_event_manager_defs.h"
#include "../../fastdelegate.h"

typedef fastdelegate::FastDelegate1<IEventData*> typeEvent;

class CMonsterEventManager {
	DEFINE_VECTOR	(typeEvent*,	EVENT_VECTOR, EVENT_VECTOR_IT);
	DEFINE_MAP		(EEventType,	EVENT_VECTOR, EVENT_MAP, EVENT_MAP_IT);

	EVENT_MAP		m_event_storage;
public:
				CMonsterEventManager	();
				~CMonsterEventManager	();

	typeEvent	*get_binder				(EEventType);
	void		raise					(EEventType, IEventData *data = 0);

private:
	void		clear		();
};


