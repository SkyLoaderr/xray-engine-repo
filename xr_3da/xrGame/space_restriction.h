////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "restriction_space.h"
#include "space_restriction_holder.h"
#include "space_restriction_bridge.h"

class CSpaceRestrictionManager;
class CSpaceRestrictionBridge;

class CSpaceRestriction : public RestrictionSpace::CTimeIntrusiveBase {
	friend struct CRemoveMergedFreeInRestrictions;
private:
	typedef SpaceRestrictionHolder::CBaseRestrictionPtr CBaseRestrictionPtr;

private:
	struct CFreeInRestriction {
		CBaseRestrictionPtr		m_restriction;
		bool					m_enabled;

		IC	CFreeInRestriction	(CBaseRestrictionPtr restriction, bool enabled)
		{
			m_restriction		= restriction;
			m_enabled			= enabled;
		}
	};

private:
	typedef xr_vector<CBaseRestrictionPtr>	RESTRICTIONS;
	typedef xr_vector<CFreeInRestriction>	FREE_IN_RESTRICTIONS;

protected:
	bool							m_initialized;
	bool							m_applied;
	ref_str							m_out_restrictions;
	ref_str							m_in_restrictions;
	xr_vector<u32>					m_border;
	CSpaceRestrictionManager		*m_space_restriction_manager;
	CBaseRestrictionPtr				m_out_space_restriction;
	CBaseRestrictionPtr				m_in_space_restriction;
	FREE_IN_RESTRICTIONS			m_free_in_restrictions;

private:
	IC		bool					intersects					(CBaseRestrictionPtr bridge);
	IC		bool					intersects					(CBaseRestrictionPtr bridge0, CBaseRestrictionPtr bridge1);
			CBaseRestrictionPtr		merge						(CBaseRestrictionPtr bridge, const RESTRICTIONS &temp_restrictions) const;
			void					merge_in_out_restrictions	();
			void					merge_free_in_retrictions	();
	IC		bool					accessible_neighbours		(u32 level_vertex_id);
			u32						accessible_nearest			(CBaseRestrictionPtr restriction, const Fvector &position);

protected:
	IC		bool					initialized					() const;
			bool					affect						(CBaseRestrictionPtr bridge, const Fvector &start_position, float radius) const;
			bool					affect						(CBaseRestrictionPtr bridge, u32 start_vertex_id, float radius) const;
			bool					affect						(CBaseRestrictionPtr bridge, const Fvector &start_position, const Fvector &dest_position) const;
			bool					affect						(CBaseRestrictionPtr bridge, u32 start_vertex_id, u32 dest_vertex_id) const;

public:
									CSpaceRestriction			(CSpaceRestrictionManager *space_restriction_manager, ref_str out_restrictions, ref_str in_restrictions);
			void					initialize					();
			void					remove_border				();
	template <typename T1, typename T2>
	IC		void					add_border					(T1 p1, T2 p2);
	IC		const xr_vector<u32>	&border						();
			u32						accessible_nearest			(const Fvector &position, Fvector &result);
	template <typename T>
	IC		bool					accessible					(T position_or_vertex_id, float radius = EPS_L);
	IC		ref_str					out_restrictions			() const;
	IC		ref_str					in_restrictions				() const;
	IC		bool					applied						() const;
};

#include "space_restriction_inline.h"