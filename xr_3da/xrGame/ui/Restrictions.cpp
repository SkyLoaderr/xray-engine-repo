#include "stdafx.h"

#include "Restrictions.h"

CRestrictions::CRestrictions(){
	InitGroups();
	m_rank = 0;
}

CRestrictions::~CRestrictions(){

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

void CRestrictions::AddGroup(LPCSTR group, LPCSTR lst){
	Groups_it it = m_goups.find(group);
	if (m_goups.end() == it)
	{
		group_items itms;
		m_goups.insert(mk_pair(group,itms));
		AddGroup(group, lst);
	}
	else
	{
		string256	singleItem;
		u32 count	= _GetItemCount(lst);
		for (u32 j = 0; j < count; ++j)
		{
			_GetItem(lst, j, singleItem);
			if (IsGroupExist(singleItem))
				CopyGroup(singleItem, group);
			else
				(*it).second.push_back(singleItem);
		}
	}
}

void CRestrictions::InitGroups(){
	if (!pSettings->section_exist("mp_item_groups"))
		return;

	u32 c =  pSettings->line_count("mp_item_groups");
	LPCSTR line, name;
	for (u32 i = 0; i<c; i++){
		pSettings->r_line("mp_item_groups",i,&name,&line);
		AddGroup(name,line);
	}

    string256 rank;
	for (int i = 0; true; i++)
	{
		sprintf(rank,"rank_%i",i);
		if (!pSettings->section_exist(rank))
			break;
		if (!pSettings->line_exist(rank,"amount_restriction"))
			continue;
        AddRestriction4rank(i, pSettings->r_string(rank,"amount_restriction"));
	}
}

void CRestrictions::AddRestriction4rank(int rank, LPCSTR lst){
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

RESTR CRestrictions::GetRestr(LPCSTR item){
	RESTR ret;
	char* pos = strstr(item,":");
	R_ASSERT(pos);
	pos[0]=0;

	ret.name = item;
	ret.n.cur_val = 0;
	if (0 == xr_strcmp(++pos,"no_limit"))
		ret.n.max_val = 65536;
	else
    	ret.n.max_val = atoi(pos);
	return ret;
}

LPCSTR CRestrictions::GetItemGroup(LPCSTR item){
	Groups_it it;
	group_items_it IT;
	
	for (it = m_goups.begin(); it != m_goups.end(); it++)
		for (IT = (*it).second.begin(); IT != (*it).second.end(); IT++)
			if ((*IT) == item)
				return (*it).first.c_str();		

	return NULL;
}

bool CRestrictions::GetPermission(LPCSTR item){
	if (!item || 0 == item[0])
		return false;
	if ((int)m_restrictions.size()<m_rank + 1)
		return true;
	bool ret = false;
	if (!IsGroupExist(item))
		ret = GetPermission(GetItemGroup(item));

	rank_rest& rest = m_restrictions[m_rank];
	rank_rest_it it = rest.find(item);
	if (rest.end() != it)
	{
		RESTR_N& r = (*it).second;
        if (r.cur_val < r.max_val)
		{
			r.cur_val++;
			ret |= true;
		}
		else
			ret |= false;
	}
	else
		ret |= true;

	return ret;
}

void CRestrictions::Return(LPCSTR item){
	if (!item || 0 == item[0])
		return;
	if ((int)m_restrictions.size()<m_rank + 1)
		return;

	if (!IsGroupExist(item))
		Return(GetItemGroup(item));

	rank_rest& rest = m_restrictions[m_rank];
	rank_rest_it it = rest.find(item);
	if (rest.end() != it)
	{
		RESTR_N& r = (*it).second;
		if (r.cur_val >= 0)
			r.cur_val++;
	}
}

