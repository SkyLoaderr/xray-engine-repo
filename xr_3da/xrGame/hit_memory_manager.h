////////////////////////////////////////////////////////////////////////////
//	Module 		: hit_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Hit memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "memory_space_impl.h"

class CEntityAlive;
class CCustomMonster;

class CHitMemoryManager {
private:
	xr_vector<CHitObject>	*m_hits;
	u32						m_max_hit_count;
	const CHitObject		*m_selected_hit;

private:
	// to minimize dynamic_casts
	CCustomMonster			*m_object;

public:
							CHitMemoryManager			();
	virtual					~CHitMemoryManager			();
			void			init						();
	virtual	void			Load						(LPCSTR				section);
	virtual	void			reinit						();
	virtual	void			reload						(LPCSTR				section);
			void			add_hit_object				(const CEntityAlive	*who);
			void			add_hit_object				(float				amount,		const Fvector &vLocalDir, const CObject *who, s16 element);
			void			add_hit_object				(const CHitObject	&hit_object);
	virtual void			update						();
	IC		const xr_vector<CHitObject> &hit_objects	() const;
	IC		const CHitObject*hit						() const;
	IC		void			set_squad_objects			(xr_vector<CHitObject> *squad_objects);
			bool			hit							(const CEntityAlive *object) const;
	IC		void			enable						(const CObject *object, bool enable);
};

#include "hit_memory_manager_inline.h"