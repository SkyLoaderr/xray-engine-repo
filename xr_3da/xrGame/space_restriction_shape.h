////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_shape.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction shape
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "space_restriction_base.h"

class CSpaceRestrictor;

class CSpaceRestrictionShape : public CSpaceRestrictionBase {
	friend struct CBorderMergePredicate;
public:
	using CSpaceRestrictionBase::inside;

protected:
	CSpaceRestrictor	*m_restrictor;
	bool				m_default;

protected:
	IC			Fvector	position				(const CCF_Shape::shape_def &data) const;
	IC			float	radius					(const CCF_Shape::shape_def &data) const;
				void	build_border			();
				void	fill_shape				(const CCF_Shape::shape_def &shape);

public:
	IC					CSpaceRestrictionShape	(CSpaceRestrictor *space_restrictor, bool default_restrictor);
	IC	virtual void	initialize				();
		virtual bool	inside					(const Fvector &position, float radius);
		virtual shared_str	name					() const;
	IC	virtual bool	shape					() const;
	IC	virtual bool	default_restrictor		() const;
};

#include "space_restriction_shape_inline.h"