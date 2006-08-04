#include "stdafx.h"

#include "Restrictions.h"
#ifdef DEBUG
#include "../../xr_ioconsole.h"
#include "../../xr_ioc_cmd.h"
#endif //#ifdef DEBUG

int get_rank(const shared_str &section){
	// ATTENTION: low performance
	string256 rank;
	for (int i = 0; true; i++)
	{
		sprintf(rank,"rank_%i",i);
		if (!pSettings->section_exist(rank))
			return -1;

		if (strstr(pSettings->r_string(rank, "available_items"), *section))
			return i;
	}
}

CRestrictions::CRestrictions(){	
	m_rank = 0;
	m_bInited = false;
}

CRestrictions::~CRestrictions(){

}

void CRestrictions::InitGroups(){
	if (m_bInited)
		return;
	m_bInited = true;

	if (!pSettings->section_exist("mp_item_groups"))
		return;

	// create groups
	u32 c =  pSettings->line_count("mp_item_groups");
	LPCSTR line, name;
	for (u32 i = 0; i<c; i++){
		pSettings->r_line("mp_item_groups",i,&name,&line);
		AddGroup(name,line);
	}

	// try to find restrictions in every rank
    string256 rank;
	for (int i = 0; true; i++)
	{
		sprintf(rank,"rank_%i",i);
		// check rank for existance
		if (!pSettings->section_exist(rank))
			break;
		// check restriction for existance
		if (!pSettings->line_exist(rank,"amount_restriction"))
			continue;
        AddRestriction4rank(i, pSettings->r_string(rank,"amount_restriction"));
		FillUpRank(rank, i);	// get info about available items for every rank
	}

	if (pSettings->section_exist("rank_base"))
		if (pSettings->line_exist("rank_base","amount_restriction"))
		{
			LPCSTR lst = pSettings->r_string("rank_base","amount_restriction");
			string256	singleItem;
			u32 count	= _GetItemCount(lst);
			for (u32 j = 0; j < count; ++j)
			{
				_GetItem(lst, j, singleItem);
				RESTR r = GetRestr(singleItem);
				m_base_rank_rest.insert(mk_pair(r.name, r.n));
			}
		}

#ifdef DEBUG
	CMD4(CCC_Integer,"rank_for_buymenu",&m_rank,0,4);
#endif	
}

void CRestrictions::FillUpRank(LPCSTR rank, int i_rank){
	if ((int)m_items.size()<i_rank + 1)
		m_items.resize(i_rank + 1);

	R_ASSERT(pSettings->section_exist(rank));

	xr_string itemsList;
	string256 single_item;

	itemsList = pSettings->r_string(rank, "available_items");
	int itemsCount	= _GetItemCount(itemsList.c_str());

	if (i_rank > 0)
		m_items[i_rank] = m_items[i_rank - 1];

	for (int i = 0; i < itemsCount; i++)
	{
		_GetItem(itemsList.c_str(), i, single_item);
		m_items[i_rank].insert(mk_pair(single_item, 0/*dummy*/));		
	}
}

bool CRestrictions::IsAvailable(LPCSTR itm){
	return m_items[m_rank].find(itm) != m_items[m_rank].end();
}

bool CRestrictions::IsInRank(LPCSTR itm, int rank){
    return m_items[rank].find(itm) != m_items[rank].end();
}

int	 CRestrictions::GetItemRank(LPCSTR itm){
	for (int i = 0; i<4; i++){
		if (IsInRank(itm, i))
			return i;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
////// PRIVATE METHODS
/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
void CRestrictions::AddGroup(LPCSTR group, LPCSTR lst){		// private
	Groups_it it = m_goups.find(group);
	if (m_goups.end() == it)	// if not found
	{
		group_items itms;
		m_goups.insert(mk_pair(group,itms));	// add group with empty list
		
		AddGroup(group, lst);	// recursive call (group items will be added to group item list)
	}
	else
	{
		string256	singleItem;
		u32 count	= _GetItemCount(lst);
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

bool CRestrictions::IsGroupExist(LPCSTR group){
	return (m_goups.end() != m_goups.find(group));
}

void CRestrictions::CopyGroup(LPCSTR from, LPCSTR to){
	if (0 == xr_strcmp(from,to))
		return;

	Groups_it it_from = m_goups.find(from);
	Groups_it it_to = m_goups.find(to);

	if (m_goups.end() == it_from || m_goups.end() == it_to)
		return;

	for (group_items_it it = (*it_from).second.begin(); it != (*it_from).second.end(); it++)
		(*it_to).second.push_back(*it);
}

void CRestrictions::AddRestriction4rank(int rank, LPCSTR lst){		// private
	if ((int)m_restrictions.size()<rank + 1)
		m_restrictions.resize(rank + 1);

	rank_rest& rest = m_restrictions[rank];

	string256	singleItem;
	u32 count	= _GetItemCount(lst);
	for (u32 j = 0; j < count; ++j)
	{
		_GetItem(lst, j, singleItem);
		RESTR r = GetRestr(singleItem);
		rest.insert(mk_pair(r.name, r.n));
	}
}


RESTR CRestrictions::GetRestr(LPCSTR item){ // private function
	RESTR ret;
	char* pos = strstr(item,":");
	R_ASSERT(pos);
	pos[0]=0;

	ret.name = item;
	//if (0 == xr_strcmp(++pos,"no_limit"))
	//	ret.n.max_val = 65536;
	//else
    	ret.n = atoi(++pos);
	return ret;
}

LPCSTR CRestrictions::GetItemGroup(LPCSTR item){	// private function
	Groups_it it;
	group_items_it IT;
	
	for (it = m_goups.begin(); it != m_goups.end(); it++)
		for (IT = (*it).second.begin(); IT != (*it).second.end(); IT++)
			if ((*IT) == item)
				return (*it).first.c_str();		

	return NULL;
}

int CRestrictions::GetItemCount(LPCSTR item){
	VERIFY(m_bInited);
	if (HasAmountControl(item))
	{
		if (!item || 0 == item[0])
			return 1;
		if ((int)m_restrictions.size()<m_rank + 1)
			return 1;

//		bool ret = true;
		if (!IsGroupExist(item))	// if it is not group
			return GetItemCount(GetItemGroup(item));		

		rank_rest& rest = m_restrictions[m_rank];
		rank_rest_it it = rest.find(item);
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

bool CRestrictions::HasAmountControl(LPCSTR item){
	if (!item || 0 == item[0])
		return false;
	if ((int)m_restrictions.size()<m_rank + 1)
		return false;

	if (!IsGroupExist(item))
		if (HasAmountControl(GetItemGroup(item)))
            return true;

	rank_rest& rest = m_restrictions[m_rank];
	rank_rest_it it = rest.find(item);
	if (rest.end() != it)
		return true;
	else
	{
//		rest = m_base_rank_rest;
		it = m_base_rank_rest.find(item);
		if (m_base_rank_rest.end() != it)
			return true;
		else return false;
	}
}



//bool CRestrictions::GetPermission(LPCSTR item){
//	if (!item || 0 == item[0])
//		return true;
//	if ((int)m_restrictions.size()<m_rank + 1)
//		return true;
//
//	bool ret = true;
//	if (!IsGroupExist(item))
//		ret = GetPermission(GetItemGroup(item));		
//
//	rank_rest& rest = m_restrictions[m_rank];
//	rank_rest_it it = rest.find(item);
//	if (rest.end() != it)
//		ret &= Try((*it).second);
//	else
//	{
//		it = m_base_rank_rest.find(item);
//		if (m_base_rank_rest.end() != it)
//			ret &= Try((*it).second);
//		else
//            ret &= true;
//	}
//
//	return ret;
//}

//bool CRestrictions::Try(RESTR_N& r){
//	return r.cur_val < r.max_val;
//}