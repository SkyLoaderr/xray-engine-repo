////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_manager.cpp
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction.h"
#include "restriction_space.h"
#include "space_restriction_manager.h"
#include "space_restriction_base.h"
#include "space_restriction_bridge.h"
#include "object_broker.h"

const u32 time_to_delete = 30000;

CSpaceRestrictionManager::CSpaceRestrictionManager			()
{
	m_clients						= xr_new<CLIENT_RESTRICTIONS>();
}

CSpaceRestrictionManager::~CSpaceRestrictionManager			()
{
	xr_delete						(m_clients);
	delete_data						(m_space_restrictions);
}

IC	CSpaceRestrictionManager::CRestrictionPtr CSpaceRestrictionManager::restriction	(ALife::_OBJECT_ID id)
{
	CLIENT_RESTRICTIONS::iterator	I = m_clients->find(id);
	VERIFY							(m_clients->end() != I);
	return							((*I).second);
}

IC	void CSpaceRestrictionManager::restrict						(ALife::_OBJECT_ID id, ref_str out_restrictors, ref_str in_restrictors)
{
	CLIENT_RESTRICTIONS::iterator	I = m_clients->find(id);
	VERIFY2							((m_clients->end() == I) || !(*I).second || !(*I).second->applied(),"Restriction cannot be changed since its border is still applied!");
	(*m_clients)[id]				= restriction(out_restrictors,in_restrictors);
	remove_unused					();
}

IC	void CSpaceRestrictionManager::remove_unused				()
{
	SPACE_RESTRICTIONS::iterator	I = m_space_restrictions.begin(), J;
	SPACE_RESTRICTIONS::iterator	E = m_space_restrictions.end();
	for ( ; I != E; ) {
		if (!(*I).second->m_ref_count && (Device.dwTimeGlobal >= (*I).second->m_last_time_dec + time_to_delete)) {
			J						= I;
//			Msg						("INTRUSIVE : removing CSpaceRestriction [%s][%s]",*(*J).second->out_restrictions(),*(*J).second->in_restrictions());
			++I;
			xr_delete				((*J).second);
			m_space_restrictions.erase	(J);
		}
		else
			++I;
	}
}

bool CSpaceRestrictionManager::accessible						(ALife::_OBJECT_ID id, const Fvector &position, float radius)
{
	CRestrictionPtr				client_restriction = restriction(id);
	if (client_restriction)
		return					(client_restriction->accessible(position,radius));
	return						(true);
}

bool CSpaceRestrictionManager::accessible					(ALife::_OBJECT_ID id, u32 level_vertex_id, float radius)
{
	CRestrictionPtr				client_restriction = restriction(id);
	if (client_restriction)
		return					(client_restriction->accessible(level_vertex_id,radius));
	return						(true);
}

CSpaceRestrictionManager::CRestrictionPtr	CSpaceRestrictionManager::restriction	(ref_str out_restrictors, ref_str in_restrictors)
{
	if (!xr_strlen(out_restrictors) && !xr_strlen(in_restrictors))
		return					(0);

	out_restrictors				= normalize_string(out_restrictors);
	in_restrictors				= normalize_string(in_restrictors);

	strconcat					(m_temp,*out_restrictors,"\x01",*in_restrictors);
	ref_str						space_restrictions = m_temp;
	
	SPACE_RESTRICTIONS::const_iterator	I = m_space_restrictions.find(space_restrictions);
	if (I != m_space_restrictions.end())
		return					((*I).second);

//	Msg							("INTRUSIVE : adding CSpaceRestriction [%s][%s]",*out_restrictors,*in_restrictors);
	CSpaceRestriction			*client_restriction = xr_new<CSpaceRestriction>(this,out_restrictors,in_restrictors);
	m_space_restrictions.insert	(std::make_pair(space_restrictions,client_restriction));
	return						(client_restriction);
}

u32	CSpaceRestrictionManager::accessible_nearest			(ALife::_OBJECT_ID id, const Fvector &position, Fvector &result)
{
	CRestrictionPtr				client_restriction = restriction(id);
	VERIFY						(client_restriction);
	return						(client_restriction->accessible_nearest(position,result));
}

IC	bool CSpaceRestrictionManager::restriction_presented	(ref_str restrictions, ref_str restriction) const
{
	for (u32 i=0, n=_GetItemCount(*restrictions); i<n; ++i)
		if (!xr_strcmp(restriction,_GetItem(*restrictions,i,m_temp)))
			return				(true);
	return						(false);
}

template <bool add>
IC	void CSpaceRestrictionManager::update_restrictions		(ref_str &restrictions, ref_str update)
{
//	if (add) {
//		strcpy						(m_temp2,*restrictions);
//		for (u32 i=0, n=_GetItemCount(*update), count = xr_strlen(m_temp2); i<n; ++i)
//			if (!restriction_presented(m_temp2,_GetItem(*update,i,m_temp1))) {
//				if (count)
//					strcat			(m_temp2,",");
//				strcat				(m_temp2,m_temp1);
//				++count;
//			}
//		restrictions				= ref_str(m_temp2);
//	}
//	else {
//		strcpy						(m_temp2,"");
//		for (u32 i=0, n=_GetItemCount(*restrictions), count = 0; i<n; ++i)
//			if (!restriction_presented(update,_GetItem(*restrictions,i,m_temp1))) {
//				if (count)
//					strcat			(m_temp2,",");
//				strcat				(m_temp2,m_temp1);
//				++count;
//			}
//		restrictions				= ref_str(m_temp2);
//	}

	// commented code explains what we do here (actually, join or intersect token strings)
	strcpy						(m_temp2,add ? *restrictions : "");
	for (u32 i=0, n=_GetItemCount(add ? *update : *restrictions), count = xr_strlen(m_temp2); i<n; ++i)
		if (!restriction_presented(add ? m_temp2 : update,_GetItem(add ? *update : *restrictions,i,m_temp1))) {
			if (count)
				strcat			(m_temp2,",");
			strcat				(m_temp2,m_temp1);
			++count;
		}
	restrictions				= ref_str(m_temp2);
}

template <bool add>
IC	void CSpaceRestrictionManager::update_restrictions		(ALife::_OBJECT_ID id, ref_str out_restrictions, ref_str in_restrictions)
{
	CRestrictionPtr				client_restriction = restriction(id);
	VERIFY						(client_restriction);
	ref_str						new_out_restrictions = client_restriction->out_restrictions();
	ref_str						new_in_restrictions = client_restriction->in_restrictions();
	update_restrictions<add>	(new_out_restrictions,out_restrictions);
	update_restrictions<add>	(new_in_restrictions,in_restrictions);
	if ((new_out_restrictions != client_restriction->out_restrictions()) || (new_in_restrictions != client_restriction->in_restrictions()))
		restrict				(id,new_out_restrictions,new_in_restrictions);
}

void CSpaceRestrictionManager::add_restrictions				(ALife::_OBJECT_ID id, ref_str out_restrictions, ref_str in_restrictions)
{
	update_restrictions<true>	(id,out_restrictions,in_restrictions);
}

void CSpaceRestrictionManager::remove_restrictions			(ALife::_OBJECT_ID id, ref_str out_restrictions, ref_str in_restrictions)
{
	update_restrictions<false>	(id,out_restrictions,in_restrictions);
}

void CSpaceRestrictionManager::remove_border				(ALife::_OBJECT_ID id)
{
	CRestrictionPtr				client_restriction = restriction(id);
	if (client_restriction)
		client_restriction->remove_border	();
}

ref_str	CSpaceRestrictionManager::in_restrictions			(ALife::_OBJECT_ID id)
{
	CRestrictionPtr				client_restriction = restriction(id);
	if (client_restriction)
		return					(client_restriction->in_restrictions());
	return						("");
}

ref_str	CSpaceRestrictionManager::out_restrictions			(ALife::_OBJECT_ID id)
{
	CRestrictionPtr				client_restriction = restriction(id);
	if (client_restriction)
		return					(client_restriction->out_restrictions());
	return						("");
}
