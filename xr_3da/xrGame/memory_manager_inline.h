////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	ERelationType	CMemoryManager::get_relation(CEntityAlive *tpEntityAlive)
{
	CEntityAlive			*self = dynamic_cast<CEntityAlive*>(this);
	VERIFY					(self);
	if (tpEntityAlive->g_Team() != self->g_Team())
		return				(eRelationTypeEnemy);
	else
		return				(eRelationTypeFriend);
}

IC	bool CMemoryManager::visible	(const CObject *object) const
{
	xr_vector<CVisibleObject>::const_iterator	I = std::find(memory_visible_objects().begin(),memory_visible_objects().end(),object_id(object));
	if (memory_visible_objects().end() == I)
		return				(false);

	return					((*I).visible());
}

IC	const CObject *CMemoryManager::item	() const
{
	return					(CItemManager::selected());
}

IC	const CEntityAlive *CMemoryManager::enemy	() const
{
	return					(CEnemyManager::selected());
}