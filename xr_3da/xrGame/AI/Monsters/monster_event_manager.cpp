#include "stdafx.h"
#include "monster_event_manager.h"

CMonsterEventManager::CMonsterEventManager()
{
}

CMonsterEventManager::~CMonsterEventManager()
{
	clear	();
}

void CMonsterEventManager::add_delegate(EEventType event, typeEvent delegate) 
{
	EVENT_MAP_IT it = m_event_storage.find(event);	
	if (it == m_event_storage.end()) {
		std::pair< EVENT_MAP_IT, bool > res;
		res = m_event_storage.insert(mk_pair(event, EVENT_VECTOR()));
		it = res.first;
	}
	
	it->second.push_back(typeEvent(delegate));
}

struct pred_remove {

	typeEvent delegate;

	pred_remove(typeEvent del) : delegate(del) {}

	bool operator() (const typeEvent &del) {
		return (del == delegate);
	}
};


void CMonsterEventManager::remove_delegate(EEventType event, typeEvent delegate) 
{
	EVENT_MAP_IT it = m_event_storage.find(event);	
	if (it == m_event_storage.end()) return;
	
	EVENT_VECTOR_IT it_del = std::remove_if(it->second.begin(),it->second.end(), pred_remove(delegate));
	it->second.erase(it_del,it->second.end());
}


void CMonsterEventManager::raise(EEventType event, IEventData *data)
{
	EVENT_MAP_IT it = m_event_storage.find(event);
	if (it == m_event_storage.end()) return;

	for (EVENT_VECTOR_IT I=it->second.begin(); I != it->second.end(); I++) {
		(*I)(data);
	}
}

void CMonsterEventManager::clear()
{
	for (EVENT_MAP_IT I_map = m_event_storage.begin(); I_map != m_event_storage.end(); I_map++) {
		I_map->second.clear();
	}
	
	m_event_storage.clear();
}


