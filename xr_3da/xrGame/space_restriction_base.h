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
			bool					inside					(u32 level_vertex_id, float radius = EPS_L);
	virtual	bool					inside					(const Fvector &position, float radius = EPS_L) = 0;
	virtual	void					initialize				() = 0;
	IC		const xr_vector<u32>	&border					();
	IC		bool					initialized				() const;
	virtual ref_str					name					() const = 0;
	virtual bool					shape					() const = 0;
			u32						accessible_nearest		(const Fvector &position, Fvector &result);
	virtual bool					default_restrictor		() const = 0;
};

#include "space_restriction_base_inline.h"