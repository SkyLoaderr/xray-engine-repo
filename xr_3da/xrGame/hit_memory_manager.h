////////////////////////////////////////////////////////////////////////////
//	Module 		: hit_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Hit memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "memory_space_impl.h"
#include "entity_alive.h"

class CHitMemoryManager {
private:
	xr_vector<CHitObject>		*m_hits;
	u32							m_max_hit_count;
	const CHitObject			*m_selected_hit;

	struct CHitObjectPredicate {
		const CObject *m_object;
					CHitObjectPredicate			(const CObject *object) :
						m_object(object)
		{
		}

		bool		operator()					(const CHitObject &hit_object) const
		{
			if (!m_object)
				return			(!hit_object.m_object);
			if (!hit_object.m_object)
				return			(false);
			return				(m_object->ID() == hit_object.m_object->ID());
		}
	};
public:
					CHitMemoryManager			();
	virtual			~CHitMemoryManager			();
			void	Init						();
	virtual	void	Load						(LPCSTR				section);
	virtual	void	reinit						();
	virtual	void	reload						(LPCSTR				section);
	IC		void	add_hit_object				(const CEntityAlive	*who);
			void	add_hit_object				(float				amount,		const Fvector &vLocalDir, const CObject *who, s16 element);
			void	add_hit_object				(const CHitObject	&hit_object);
	virtual void	update						();
	IC		const xr_vector<CHitObject> &hit_objects() const;
	IC		const CHitObject *hit				() const;
	IC		void	set_squad_objects			(xr_vector<CHitObject> *squad_objects);
	IC		bool	hit							(const CEntityAlive *object) const;
	IC		void	enable						(const CObject *object, bool enable);
};

#include "hit_memory_manager_inline.h"