////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"

namespace RestrictionSpace {
	enum ERestrictorTypes;
};

template <bool add> struct CRestrictionPredicate;

class CRestrictedObject : virtual public CGameObject {
	friend struct CRestrictionPredicate<true>;
	friend struct CRestrictionPredicate<false>;
private:
	typedef CGameObject inherited;

private:
	mutable bool		m_applied;
	mutable bool		m_removed;

protected:
	template <typename P, bool value>
	IC		void		construct_restriction_string	(LPSTR temp_restrictions, const xr_vector<ALife::_OBJECT_ID> &restrictions, shared_str current_restrictions, const P &p);
	IC		void		add_object_restriction			(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes &restrictor_type);
	IC		void		remove_object_restriction		(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes &restrictor_type);

public:
	IC					CRestrictedObject				();
	virtual				~CRestrictedObject				();
	virtual BOOL		net_Spawn						(LPVOID data);
			void		add_border						(u32 start_vertex_id, float radius) const;
			void		add_border						(const Fvector &start_position, const Fvector &dest_position) const;
			void		add_border						(u32 start_vertex_id, u32 dest_vertex_id) const;
			void		remove_border					() const;
			u32			accessible_nearest				(const Fvector &position, Fvector &result) const;
			bool		accessible						(const Fvector &position) const;
			bool		accessible						(const Fvector &position, float radius) const;
			bool		accessible						(u32 level_vertex_id) const;
			bool		accessible						(u32 level_vertex_id, float radius) const;
			void		add_restrictions				(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions);
			void		remove_restrictions				(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions);
			void		remove_all_restrictions			();
			shared_str	in_restrictions					() const;
			shared_str	out_restrictions				() const;
	IC		bool		applied							() const;
};

#include "restricted_object_inline.h"