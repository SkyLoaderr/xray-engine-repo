////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "item_manager.h"
#include "entity_alive.h"

class CHitMemoryManager;

class CEnemyManager : public CObjectManager<const CEntityAlive> {
protected:
	typedef CObjectManager<const CEntityAlive> inherited;

protected:
	float				m_ignore_monster_threshold;
	CEntityAlive		*m_self_entity_alive;
	CHitMemoryManager	*m_self_hit_manager;

public:
	virtual void		Load						(LPCSTR section);
	virtual void		reload						(LPCSTR section);
	virtual bool		useful						(const CEntityAlive *object) const;
	virtual	float		evaluate					(const CEntityAlive *object) const;
	IC		const xr_set<const CEntityAlive*> &enemies() const;
			bool		expedient					(const CEntityAlive *object) const;
};

#include "enemy_manager_inline.h"