////////////////////////////////////////////////////////////////////////////
//	Module 		: hit_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Hit memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace MemorySpace {
	struct CHitObject;
};

class CEntityAlive;
class CCustomMonster;

class CHitMemoryManager {
public:
	typedef MemorySpace::CHitObject CHitObject;
	typedef xr_vector<CHitObject>	HITS;

private:
	CCustomMonster			*m_object;
	CAI_Stalker				*m_stalker;
	HITS					*m_hits;
	u32						m_max_hit_count;
	const CHitObject		*m_selected_hit;

public:
	IC						CHitMemoryManager	(CCustomMonster *object, CAI_Stalker *stalker);
	virtual					~CHitMemoryManager	();
	virtual	void			Load				(LPCSTR section);
	virtual	void			reinit				();
	virtual	void			reload				(LPCSTR section);
	virtual void			update				();
			void			remove_links		(CObject *object);

public:
			void			add					(const CEntityAlive	*who);
			void			add					(float amount, const Fvector &local_direction, const CObject *who, s16 element);
			void			add					(const CHitObject	&hit_object);

public:
	IC		const HITS		&objects			() const;
	IC		const CHitObject*hit				() const;
	IC		void			set_squad_objects	(HITS *squad_objects);

public:
			bool			hit					(const CEntityAlive *object) const;
			void			enable				(const CObject *object, bool enable);
			CCustomMonster	&object				() const;
};

#include "hit_memory_manager_inline.h"