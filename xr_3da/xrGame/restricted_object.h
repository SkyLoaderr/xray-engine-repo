////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"

class CSpaceRestrictor;

namespace RestrictedObject {
	enum ERestrictionType {
		eRestrictionTypeIn		= u32(0),
		eRestrictionTypeOut,
		eRestrictionTypeDummy	= u32(-1),
	};

	struct CInRestriction {
		virtual bool	accessible				(const Fvector &position) const = 0;
	};

	class CShapeRestriction : public CInRestriction {
	private:
		CSpaceRestrictor	*m_restrictor;

	public:
		IC				CShapeRestriction		(CSpaceRestrictor *restrictor)
		{
			m_restrictor	= restrictor;
			VERIFY			(m_restrictor);
		}

		virtual	bool	accessible				(const Fvector &position) const;
	};

	class CDynamicRestriction  : public CInRestriction {
	private:
		Fvector				m_position;
		float				m_radius_sqr;

	public:
		IC				CDynamicRestriction		(const Fvector &position, float radius)
		{
			m_position		= position;
			m_radius_sqr	= _sqr(radius);
		}

		IC	virtual	bool	accessible			(const Fvector &position) const
		{
			return			(m_position.distance_to_sqr(position) > m_radius_sqr);
		}
	};

	class CRestrictionContainer {
	protected:
		typedef xr_map<u32,RestrictedObject::CInRestriction*>	RESTRICTIONS;

	protected:
		RESTRICTIONS	m_restrictions;

	private:
		u32				m_id_seed;

	public:
						CRestrictionContainer	();
	virtual				~CRestrictionContainer	();
			void		add_restriction			(CSpaceRestrictor *restrictor);
	IC		void		add_restriction			(u32 restriction_id, const Fvector &position, float radius);
	IC		u32			add_restriction			(const Fvector &position, float radius);
	IC		void		remove_restriction		(u32 restriction_id);
	IC		bool		accessible				(const Fvector &position) const;
	IC		bool		empty					() const;
	};
};

class CRestrictedObject : virtual public CGameObject {
private:
	typedef CGameObject inherited;

protected:
	RestrictedObject::CRestrictionContainer	*m_in_restrictions;
	RestrictedObject::CRestrictionContainer	*m_out_restrictions;

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
	IC		RestrictedObject::CRestrictionContainer &container(RestrictedObject::ERestrictionType type) const;
};

#include "restricted_object_inline.h"