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
	enum ERestrictorTypes;
};

namespace SpaceRestrictionHolder {
	typedef intrusive_ptr<CSpaceRestrictionBridge,RestrictionSpace::CTimeIntrusiveBase> CBaseRestrictionPtr;
};

class CSpaceRestrictionHolder {
public:
	typedef xr_map<shared_str,CSpaceRestrictionBridge*>	RESTRICTIONS;

private:
	xr_vector<shared_str>				m_temp;
	RESTRICTIONS						m_restrictions;
	shared_str							m_default_out_restrictions;
	shared_str							m_default_in_restrictions;

protected:
	IC		shared_str					normalize_string				(shared_str space_restrictors);
	IC		void						collect_garbage					();
	IC		shared_str					default_out_restrictions		() const;
	IC		shared_str					default_in_restrictions			() const;
	virtual void						on_default_restrictions_changed	(const RestrictionSpace::ERestrictorTypes &restrictor_type, shared_str old_restrictions, shared_str new_restrictions) = 0;

public:
	IC								CSpaceRestrictionHolder			();
	virtual							~CSpaceRestrictionHolder		();
			SpaceRestrictionHolder::CBaseRestrictionPtr	restriction	(shared_str space_restrictors);
			void					register_restrictor				(CSpaceRestrictor *space_restrictor, const RestrictionSpace::ERestrictorTypes &restrictor_type);
};

#include "space_restriction_holder_inline.h"