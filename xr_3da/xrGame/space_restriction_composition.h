////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_composition.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction composition
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "space_restriction_base.h"
#include "space_restriction_holder.h"
#include "space_restriction_bridge.h"

class CSpaceRestrictionBridge;
class CSpaceRestrictionHolder;

class CSpaceRestrictionComposition : public CSpaceRestrictionBase {
protected:
	typedef SpaceRestrictionHolder::CBaseRestrictionPtr CBaseRestrictionPtr;
	typedef xr_vector<CBaseRestrictionPtr> RESTRICTIONS;

protected:
	RESTRICTIONS			m_restrictions;
	ref_str					m_space_restrictors;
	CSpaceRestrictionHolder	*m_space_restriction_holder;
	
protected:
	IC			void		merge							(CBaseRestrictionPtr restriction);

public:
	IC						CSpaceRestrictionComposition	(CSpaceRestrictionHolder *space_restriction_holder, ref_str space_restrictors);
		virtual void		initialize						();
		virtual bool		inside							(const Fvector &position, float radius = EPS_L);
	IC	virtual ref_str		name							() const;
	IC	virtual bool		shape							() const;
	IC	virtual bool		default_restrictor				() const;
};

#include "space_restriction_composition_inline.h"
