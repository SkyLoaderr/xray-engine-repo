#include "stdafx.h"

#include "Restrictions.h"
#ifdef DEBUG
#include "../../xr_ioconsole.h"
#include "../../xr_ioc_cmd.h"
#endif //#ifdef DEBUG

shared_str	g_ranks[_RANK_COUNT];

int get_rank(const shared_str &section)
{
	int res = -1;
	if( g_ranks[0].size()==0 )
	{ //load
		string32			buff;
		for (int i = 0; i<_RANK_COUNT; i++)
		{
			sprintf(buff, "rank_%i", i);
			g_ranks[i] = pSettings->r_string(buff, "available_items");
		}
	}
	for (int i = 0; i<_RANK_COUNT; i++)
	{
		if (strstr(g_ranks[i].c_str(), section.c_str()))
		{
			res = i;
			break;
		}
	}

	R_ASSERT3	(res!=-1,"cannot find rank for", section.c_str());
	return		res;
}

CRestrictions::CRestrictions()
{	
	m_rank			= 0;
	m_bInited		= false;
}

CRestrictions::~CRestrictions()
{}

void CRestrictions::InitGroups()
{
	if (m_bInited)			return;
	m_bInited				= true;

	if (!pSettings->section_exist("mp_item_groups"))
		return;

	// create groups
	u32 c					=  pSettings->line_count("mp_item_groups");
	LPCSTR					line, name;
	for (u32 i = 0; i<c; i++)
	{
		pSettings->r_line	("mp_item_groups", i, &name, &line);
		AddGroup			(name, line);
	}

	// try to find restrictions in every rank
    string32				rank;
	for (int i = 0; i<_RANK_COUNT; i++)
	{
		sprintf				(rank,"rank_%i",i);
		
		// check restriction for existance
		if (!pSettings->line_exist(rank,"amount_restriction"))
			continue;

        AddRestriction4rank		(i, pSettings->r_string(rank, "amount_restriction"));
		FillUpRank				(rank, i);	// get info about available items for every rank
	}

	if (pSettings->section_exist("rank_base"))
		if (pSettings->line_exist("rank_base","amount_restriction"))
		{
			LPCSTR lst					= pSettings->r_string("rank_base","amount_restriction");
			string256					singleItem;
			u32 count					= _GetItemCount(lst);
			
			for (u32 j = 0; j < count; ++j)
			{
				_GetItem				(lst, j, singleItem);
				RESTR r					= GetRestr(singleItem);
				m_base_rank_rest.insert	(mk_pair(r.name, r.n));
			}
		}

#ifdef DEBUG
	CMD4(CCC_Integer,"rank_for_buymenu",&m_rank,0,4);
#endif	
}

void CRestrictions::FillUpRank(const shared_str& rank, int i_rank)
{
	if ((int)m_items.size()<i_rank + 1)
		m_items.resize(i_rank + 1);

	R_ASSERT			(pSettings->section_exist(rank));

	xr_string			itemsList;
	string256			single_item;

	itemsList			= pSettings->r_string(rank, "available_items");
	int itemsCount		= _GetItemCount(itemsList.c_str());

	if (i_rank > 0)
		m_items[i_rank] = m_items[i_rank - 1];

	for (int i = 0; i < itemsCount; i++)
	{
		_GetItem(itemsList.c_str(), i, single_item);
		m_items[i_rank].insert(mk_pair(single_item, 0/*dummy*/));		
	}
}

bool CRestrictions::IsAvailable(const shared_str& itm)
{
	return m_items[m_rank].find(itm) != m_items[m_rank].end();
}

bool CRestrictions::IsInRank(const shared_str& itm, int rank) const
{
    return m_items[rank].find(itm) != m_items[rank].end();
}

int	 CRestrictions::GetItemRank(const shared_str& itm) const
{
	for (int i = 0; i<_RANK_COUNT; i++){
		if (IsInRank(itm, i))
			return i;
	}

	return 0;
}

void CRestrictions::AddGroup(LPCSTR group, LPCSTR lst)
{// private
	Groups_it it = m_goups.find(group);
	if (m_goups.end() == it)	// if not found
	{
		group_items				itms;
		m_goups.insert			(mk_pair(group,itms));	// add group with empty list
		
		AddGroup				(group, lst);			// recursive call (group items will be added to group item list)
	}
	else
	{
		string256				singleItem;
		u32 count				= _GetItemCount(lst);
		for (u32 j = 0; j < count; ++j)				// add all items to group
		{
			_GetItem(lst, j, singleItem);
			if (IsGroupExist(singleItem))			// if item is group...
				CopyGroup(singleItem, group);		// ... insert all items of this group
			else
				(*it).second.push_back(singleItem);	//
		}
	}
}

bool CRestrictions::IsGroupExist(const shared_str& group) const
{
	return (m_goups.end() != m_goups.find(group));
}

void CRestrictions::CopyGroup(const shared_str& from, const shared_str& to)
{
	if (from == to)	return;

	Groups_it it_from	= m_goups.find(from);
	Groups_it it_to		= m_goups.find(to);

	if (m_goups.end() == it_from || m_goups.end() == it_to)
		return;

	for (group_items_it it = (*it_from).second.begin(); it != (*it_from).second.end(); it++)
		(*it_to).second.push_back(*it);
}

void CRestrictions::AddRestriction4rank(int rank, const shared_str& lst)
{		// private
	if ((int)m_restrictions.size()<rank + 1)
		m_restrictions.resize(rank + 1);

	rank_rest& rest			= m_restrictions[rank];

	string256				singleItem;
	u32 count				= _GetItemCount(lst.c_str());
	for (u32 j = 0; j < count; ++j)
	{
		_GetItem			(lst.c_str(), j, singleItem);
		RESTR r				= GetRestr(singleItem);
		rest.insert			(mk_pair(r.name, r.n));
	}
}


RESTR CRestrictions::GetRestr(const shared_str& item)
{ // private function
	RESTR				ret;
	string512			_name;
	int _cnt			= 0;
	ptrdiff_t n				= strchr(item.c_str(),':') - item.c_str();
	if( n>0 )
	{
		strncpy			(_name, item.c_str(), n );
		_name[n]		= 0;
		_cnt			= sscanf(item.c_str()+n+1,"%d", &ret.n);
	}
	R_ASSERT3			(n>0 && _cnt==1, "invalid record format <name_sect:rank>", item.c_str());
	ret.name			= _name;

	return ret;
}

shared_str CRestrictions::GetItemGroup(const shared_str& item) const
{	// private function
//.	Groups_it							it;
	Groups::const_iterator				it;
//.	group_items_it						IT;
	group_items::const_iterator			IT;
	
	for (it = m_goups.begin(); it != m_goups.end(); it++)
		for (IT = (*it).second.begin(); IT != (*it).second.end(); IT++)
			if ((*IT) == item)
				return (*it).first;		

	return		NULL;
}

int CRestrictions::GetItemCount(const shared_str& item) const
{
	VERIFY						(m_bInited);
	if (HasAmountControl(item))
	{
		if ( !item.size() )
			return 1;

		if ((int)m_restrictions.size()<m_rank + 1)
			return 1;

		if (!IsGroupExist(item))	// if it is not group
			return GetItemCount(GetItemGroup(item));		

		const rank_rest& rest		= m_restrictions[m_rank];
		rank_rest::const_iterator	it; 

		/*rank_rest_it*/ it = rest.find(item);

		if (rest.end() != it)
			return (*it).second;
		else
		{
			it = m_base_rank_rest.find(item);
			if (m_base_rank_rest.end() != it)
				return (*it).second;
			else
				return 1;
		}
	}
	else
        return 1;
}

bool CRestrictions::HasAmountControl(const shared_str& item) const
{
	if (!item.size())
		return false;

	if ( (int)m_restrictions.size() < m_rank + 1)
		return false;

	if ( !IsGroupExist(item) )
		if (HasAmountControl(GetItemGroup(item)))
            return true;

	const rank_rest& rest		= m_restrictions[m_rank];
	rank_rest::const_iterator	it;
	/*rank_rest_it*/ it			= rest.find(item);

	if (rest.end() != it)
		return true;
	else
	{
		it = m_base_rank_rest.find(item);
		if (m_base_rank_rest.end() != it)
			return true;
		else return false;
	}
}