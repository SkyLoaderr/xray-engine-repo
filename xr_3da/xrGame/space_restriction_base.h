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
	xr_vector<u32>					m_out_border;
	xr_vector<u32>					m_in_border;
	bool							m_initialized;

protected:
	IC		void					process_borders			();
	IC		void					process_borders			(xr_vector<u32> &border);

public:
									CSpaceRestrictionBase	();
			bool					inside					(u32 level_vertex_id, bool out, float radius = EPS_S);
	virtual	bool					inside					(const Fvector &position, float radius = EPS_L) = 0;
	virtual	void					initialize				() = 0;
	IC		const xr_vector<u32>	&border					(bool out_restriction);
	IC		bool					initialized				() const;
	virtual ref_str					name					() const = 0;
	virtual bool					shape					() const = 0;
			u32						accessible_nearest		(const Fvector &position, Fvector &result, bool out_restriction);
	virtual bool					default_restrictor		() const = 0;
};

#include "space_restriction_base_inline.h"