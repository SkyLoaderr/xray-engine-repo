////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"
#include "restricted_object_space.h"
#include "restriction_container.h"

class CRestrictedObject : virtual public CGameObject {
private:
	typedef CGameObject inherited;

protected:
	CRestrictionContainer	*m_in_restrictions;
	CRestrictionContainer	*m_out_restrictions;

public:
	IC					CRestrictedObject		();
	virtual				~CRestrictedObject		();
	virtual BOOL		net_Spawn				(LPVOID data);
	virtual void		net_Destroy				();
			void		add_border				() const;
			void		remove_border			() const;
			bool		accessible				(const Fvector &position) const;
			bool		accessible				(u32 vertex_id) const;
			u32			accessible_nearest		(const Fvector &position, Fvector &result) const;

public:
	IC		void		add_restriction			(RestrictedObject::ERestrictionType type, CSpaceRestrictor *restrictor);
	IC		void		add_restriction			(RestrictedObject::ERestrictionType type, u32 restriction_id, const Fvector &position, float radius);
	IC		u32			add_restriction			(RestrictedObject::ERestrictionType type, const Fvector &position, float radius);
	IC		void		remove_restriction		(RestrictedObject::ERestrictionType type, u32 restriction_id);

private:
	IC		void		clear					();
	IC		CRestrictionContainer &container	(RestrictedObject::ERestrictionType type) const;
};

#include "restricted_object_inline.h"