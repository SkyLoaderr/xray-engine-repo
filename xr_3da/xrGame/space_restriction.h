////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSpaceRestrictor;

class CSpaceRestriction {
public:
	typedef xr_vector<CSpaceRestriction*> RESTRICTIONS;

protected:
	RESTRICTIONS					m_restrictions;
	xr_vector<u32>					m_border;
	ref_str							m_space_restrictors;
	CSpaceRestrictor				*m_restrictor;
	bool							m_initialized;
	bool							m_applied;

protected:
	IC		Fvector					position			(const CCF_Shape::shape_def &data) const;
	IC		float					radius				(const CCF_Shape::shape_def &data) const;
			void					build_border		();
			void					process_borders		();
			void					merge				(CSpaceRestriction *restriction);

public:
									CSpaceRestriction	(ref_str space_restrictors);
									CSpaceRestriction	(CSpaceRestrictor *space_restrictor);
	virtual							~CSpaceRestriction	();
			bool					inside				(const Fvector &position, float radius = EPS_L);
			void					initialize			();
			void					add_border			();
			void					remove_border		();
	IC		const xr_vector<u32>	&border				();
	IC		ref_str					space_restrictors	() const;
};

#include "space_restriction_inline.h"