////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_holder.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction holder
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSpaceRestrictionBridge;
class CSpaceRestrictor;

template <typename _1, typename _2> class intrusive_ptr;

namespace RestrictionSpace {
	struct CTimeIntrusiveBase;
	enum EDefaultRestrictorTypes;
};

namespace SpaceRestrictionHolder {
	typedef intrusive_ptr<CSpaceRestrictionBridge,RestrictionSpace::CTimeIntrusiveBase> CBaseRestrictionPtr;
};

class CSpaceRestrictionHolder {
public:
	typedef xr_map<ref_str,CSpaceRestrictionBridge*>	RESTRICTIONS;

private:
	xr_vector<ref_str>				m_temp;
	string256						m_temp_string;
	RESTRICTIONS					m_restrictions;
	ref_str							m_default_out_restrictions;
	ref_str							m_default_in_restrictions;

protected:
	IC		ref_str					normalize_string				(ref_str space_restrictors);
	IC		void					collect_garbage					();
	IC		ref_str					default_out_restrictions		() const;
	IC		ref_str					default_in_restrictions			() const;
	virtual void					on_default_restrictions_changed	(const RestrictionSpace::EDefaultRestrictorTypes &restrictor_type, ref_str old_restrictions, ref_str new_restrictions) = 0;

public:
	IC								CSpaceRestrictionHolder			();
	virtual							~CSpaceRestrictionHolder		();
			SpaceRestrictionHolder::CBaseRestrictionPtr	restriction	(ref_str space_restrictors);
			void					register_restrictor				(CSpaceRestrictor *space_restrictor, const RestrictionSpace::EDefaultRestrictorTypes &restrictor_type);
};

#include "space_restriction_holder_inline.h"