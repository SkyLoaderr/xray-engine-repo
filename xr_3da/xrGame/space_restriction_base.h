////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_base.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction base
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSpaceRestrictionBase {
protected:
	xr_vector<u32>					m_border;
	bool							m_initialized;

protected:
	IC		void					process_borders			();

public:
									CSpaceRestrictionBase	();
			bool					inside					(u32 level_vertex_id, bool partially_inside);
			bool					inside					(u32 level_vertex_id, bool partially_inside, float radius);
			bool					inside					(const Fvector &position);
	virtual	bool					inside					(const Fvector &position, float radius) = 0;
	virtual	void					initialize				() = 0;
	IC		const xr_vector<u32>	&border					();
	IC		bool					initialized				() const;
	virtual shared_str					name					() const = 0;
	virtual bool					shape					() const = 0;
	virtual bool					default_restrictor		() const = 0;
};

#include "space_restriction_base_inline.h"