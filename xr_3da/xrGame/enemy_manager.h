////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_manager.h"
#include "entity_alive.h"
#include "custommonster.h"
#include "script_callback_ex.h"

class CEnemyManager : public CObjectManager<const CEntityAlive> {
public:
	typedef CObjectManager<const CEntityAlive>	inherited;
	typedef xr_vector<const CEntityAlive*>		ENEMIES;
	typedef CScriptCallbackEx<bool>				USEFULE_CALLBACK;

private:
	CCustomMonster			*m_object;
	float					m_ignore_monster_threshold;
	float					m_max_ignore_distance;
	mutable bool			m_ready_to_save;
	mutable bool			m_visible_now;
	u32						m_last_enemy_time;
	USEFULE_CALLBACK		m_useful_callback;

protected:
			bool			expedient			(const CEntityAlive *object) const;

public:
	IC						CEnemyManager		(CCustomMonster *object);
	virtual void			reload				(LPCSTR section);
	virtual bool			useful				(const CEntityAlive *object) const;
	virtual bool			is_useful			(const CEntityAlive *object) const;
	virtual	float			evaluate			(const CEntityAlive *object) const;
	virtual	float			do_evaluate			(const CEntityAlive *object) const;
	virtual void			update				();
	virtual void			set_ready_to_save	();
	IC		u32				last_enemy_time		() const;
	IC		USEFULE_CALLBACK&useful_callback	();
};

#include "enemy_manager_inline.h"