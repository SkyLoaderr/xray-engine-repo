////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_bridge.h
//	Created 	: 27.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction bridge
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "restriction_space.h"

class CSpaceRestrictionBase;

class CSpaceRestrictionBridge : public RestrictionSpace::CTimeIntrusiveBase {
protected:
	CSpaceRestrictionBase			*m_object;

protected:
	IC		CSpaceRestrictionBase	&object						() const;

public:
	IC								CSpaceRestrictionBridge		(CSpaceRestrictionBase *object);
	virtual							~CSpaceRestrictionBridge	();
			void					change_implementation		(CSpaceRestrictionBase *object);
			const xr_vector<u32>	&border						(bool out_restriction) const;
			bool					initialized					() const;
			void					initialize					();
			bool					inside						(const Fvector &position, float radius = EPS_L);
			bool					inside						(u32 level_vertex_id, bool out, float radius = EPS_L);
			ref_str					name						() const;
			u32						accessible_nearest			(const Fvector &position, Fvector &result, bool out_restriction);
			bool					shape						() const;
			bool					default_restrictor			() const;
};

#include "space_restriction_bridge_inline.h"