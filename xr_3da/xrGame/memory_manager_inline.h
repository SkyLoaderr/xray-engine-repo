////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	ALife::ERelationType CMemoryManager::get_relation(const CEntityAlive *tpEntityAlive) const
{
	const CEntityAlive		*self = smart_cast<const CEntityAlive*>(this);
	VERIFY					(self);
	if (tpEntityAlive->g_Team() != self->g_Team())
		return				(ALife::eRelationTypeEnemy);
	else
		return				(ALife::eRelationTypeFriend);
}

IC	bool CMemoryManager::visible	(const CObject *object) const
{
	xr_vector<CVisibleObject>::const_iterator	I = std::find(memory_visible_objects().begin(),memory_visible_objects().end(),object_id(object));
	if (memory_visible_objects().end() == I)
		return				(false);

	return					((*I).visible());
}

IC	const CGameObject *CMemoryManager::item	() const
{
	return					(CItemManager::selected());
}

IC	const CEntityAlive *CMemoryManager::enemy	() const
{
	return					(CEnemyManager::selected());
}

IC	void CMemoryManager::enable		(const CObject *object, bool enable)
{
	CVisualMemoryManager::enable	(object,enable);
	CSoundMemoryManager::enable		(object,enable);
	CHitMemoryManager::enable		(object,enable);
}

template <typename T, typename _predicate>
IC	void CMemoryManager::fill_enemies	(const xr_vector<T> &objects, const _predicate &predicate) const
{
	xr_vector<T>::const_iterator	I = objects.begin();
	xr_vector<T>::const_iterator	E = objects.end();
	for ( ; I != E; ++I) {
		if (!(*I).m_enabled)
			continue;

		const CEntityAlive			*entity_alive = smart_cast<const CEntityAlive*>((*I).m_object);
		if (entity_alive && CEnemyManager::useful(entity_alive))
			predicate				(entity_alive);
	}
}

template <typename _predicate>
IC	void CMemoryManager::fill_enemies	(const _predicate &predicate) const
{
	fill_enemies					(memory_visible_objects(),predicate);
	fill_enemies					(sound_objects(),predicate);
	fill_enemies					(hit_objects(),predicate);
}
