////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_manager.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "space_restriction_holder.h"
#include "alife_space.h"

namespace RestrictionSpace {
	struct CTimeIntrusiveBase;
};

template <typename _1, typename _2> class intrusive_ptr;

class CSpaceRestriction;

class CSpaceRestrictionManager : public CSpaceRestrictionHolder {
#ifndef DEBUG
protected:
#else
public:
#endif
	typedef intrusive_ptr<CSpaceRestriction,RestrictionSpace::CTimeIntrusiveBase> CRestrictionPtr;
	typedef xr_map<ref_str,CSpaceRestriction*>				SPACE_RESTRICTIONS;
	typedef xr_map<ALife::_OBJECT_ID,CRestrictionPtr>		CLIENT_RESTRICTIONS;

private:
	mutable	string4096			m_temp;
	mutable	string4096			m_temp1;
	mutable	string4096			m_temp2;

protected:
	SPACE_RESTRICTIONS			m_space_restrictions;
	CLIENT_RESTRICTIONS			*m_clients;

public:
	using CSpaceRestrictionHolder::restriction;

protected:
	template <bool add>
	IC		void				update_restrictions				(ref_str &restrictions, ref_str update);
	
	template <bool add>
	IC		void				update_restrictions				(ALife::_OBJECT_ID id, ref_str out_restrictions, ref_str in_restrictions);
	
	IC		bool				restriction_presented			(ref_str restrictions, ref_str restriction) const;
	IC		CRestrictionPtr		restriction						(ALife::_OBJECT_ID id);
			CRestrictionPtr		restriction						(ref_str out_restrictors, ref_str in_restrictors);
	IC		void				collect_garbage					();
	virtual void				on_default_restrictions_changed	(const RestrictionSpace::EDefaultRestrictorTypes &restrictor_type, ref_str old_restrictions, ref_str new_restrictions);
	IC		bool				no_default_restrictions			(ref_str restrictions);
	IC		void				restrict						(ALife::_OBJECT_ID id, CSpaceRestrictionManager::CRestrictionPtr restriction);

public:
								CSpaceRestrictionManager		();
	virtual						~CSpaceRestrictionManager		();
	IC		void				restrict						(ALife::_OBJECT_ID id, ref_str out_restrictors, ref_str in_restrictors);
	template <typename T1, typename T2>
	IC		void				add_border						(ALife::_OBJECT_ID id, T1 p1, T2 p2);

			void				add_restrictions				(ALife::_OBJECT_ID id, ref_str out_restrictions, ref_str in_restrictions);
			void				remove_restrictions				(ALife::_OBJECT_ID id, ref_str out_restrictions, ref_str in_restrictions);
			void				change_restrictions				(ALife::_OBJECT_ID id, ref_str add_out_restrictions, ref_str add_in_restrictions, ref_str remove_out_restrictions, ref_str remove_in_restrictions);
			void				remove_border					(ALife::_OBJECT_ID id);
			bool				accessible						(ALife::_OBJECT_ID id, const Fvector &position, float radius = EPS_L);
			bool				accessible						(ALife::_OBJECT_ID id, u32 level_vertex_id, float radius = EPS_L);
			u32					accessible_nearest				(ALife::_OBJECT_ID id, const Fvector &position, Fvector &result);
			ref_str				in_restrictions					(ALife::_OBJECT_ID id);
			ref_str				out_restrictions				(ALife::_OBJECT_ID id);

#ifdef DEBUG
	IC		const SPACE_RESTRICTIONS &restrictions				() const;
#endif
};

#include "space_restriction_manager_inline.h"