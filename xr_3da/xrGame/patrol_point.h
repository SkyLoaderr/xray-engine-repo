////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_point.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol point
////////////////////////////////////////////////////////////////////////////

#pragma once

class CPatrolPoint {
protected:
	Fvector					m_position;
	u32						m_level_vertex_id;
	u32						m_flags;
	shared_str					m_name;
#ifdef DEBUG
	bool					m_initialized;
#endif

private:
	IC		void			correct_position	();

public:
							CPatrolPoint		(const Fvector &position, u32 level_vertex_id, u32 flags, shared_str name);
							CPatrolPoint		();
	IC		const Fvector	&position			() const;
	IC		u32				level_vertex_id		() const;
	IC		u32				flags				() const;
	IC		shared_str			name				() const;
			CPatrolPoint	&load				(IReader &stream);
};


#include "patrol_point_inline.h"