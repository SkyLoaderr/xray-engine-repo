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

template <typename _1, typename _2> class CIntrusivePtr;

namespace RestrictionSpace {
	struct CTimeIntrusiveBase;
};

namespace SpaceRestrictionHolder {
	typedef CIntrusivePtr<CSpaceRestrictionBridge,RestrictionSpace::CTimeIntrusiveBase> CBaseRestrictionPtr;
};

class CSpaceRestrictionHolder {
public:
	typedef xr_map<ref_str,CSpaceRestrictionBridge*>	RESTRICTIONS;

private:
	xr_vector<ref_str>				m_temp;
	string256						m_temp_string;
	RESTRICTIONS					m_restrictions;

protected:
	IC		ref_str					normalize_string			(ref_str space_restrictors);
	IC		void					remove_unused				();

public:
	IC								CSpaceRestrictionHolder		();
	virtual							~CSpaceRestrictionHolder	();
			SpaceRestrictionHolder::CBaseRestrictionPtr	restriction	(ref_str space_restrictors);
			void					register_restrictor			(CSpaceRestrictor *space_restrictor);
};

#include "space_restriction_holder_inline.h"