#include "stdafx.h"
#include "monster_event_manager.h"

CMonsterEventManager::CMonsterEventManager()
{
}

CMonsterEventManager::~CMonsterEventManager()
{
	clear	();
}

typeEvent *CMonsterEventManager::get_binder(EEventType event) 
{
	EVENT_MAP_IT it = m_event_storage.find(event);	

	if (it == m_event_storage.end()) {
		std::pair< EVENT_MAP_IT, bool > res;
		res = m_event_storage.insert(mk_pair(event, EVENT_VECTOR()));
		it = res.first;
	}

	typeEvent *new_callback = xr_new<typeEvent>();		
	it->second.push_back(new_callback);
	return new_callback;
}

void CMonsterEventManager::raise(EEventType event, IEventData *data)
{
	EVENT_MAP_IT it = m_event_storage.find(event);
	if (it == m_event_storage.end()) return;

	for (EVENT_VECTOR_IT I=it->second.begin(); I != it->second.end(); I++) {
		(*(*I))(data);
	}
}

void CMonsterEventManager::clear()
{
	for (EVENT_MAP_IT I_map = m_event_storage.begin(); I_map != m_event_storage.end(); I_map++) {
		for (EVENT_VECTOR_IT I_vec = I_map->second.begin(); I_vec != I_map->second.end(); I_vec++) {
			xr_delete((*I_vec));
		}
	}
}

