#include "stdafx.h"

#include "Restrictions.h"
#ifdef DEBUG
#include "../../xr_ioconsole.h"
#include "../../xr_ioc_cmd.h"
#endif //#ifdef DEBUG

CRestrictions::CRestrictions(){	
	m_rank = 0;
	m_bInited = false;
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
	if (m_bInited)
		return;
	m_bInited = true;

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

bool CRestrictions::GetPermission(LPCSTR item, bool& last){
	last = true;

	if (!item || 0 == item[0])
		return true;
	if ((int)m_restrictions.size()<m_rank + 1)
		return true;

	bool ret = true;
	if (!IsGroupExist(item))
		ret = GetPermission(GetItemGroup(item), last);		

	rank_rest& rest = m_restrictions[m_rank];
	rank_rest_it it = rest.find(item);
	if (rest.end() != it)
		ret &= Try((*it).second, last);
	else
	{
		it = m_base_rank_rest.find(item);
		if (m_base_rank_rest.end() != it)
			ret &= Try((*it).second, last);
		else
            ret &= true;
	}

	Msg("mp_buymenu: GetPermission(%s,%d) == %d",item,last,ret);

	return ret;
}

bool CRestrictions::Try(RESTR_N& r, bool& last){
	if (r.cur_val < r.max_val)
	{
		r.cur_val++;
		Msg("mp_buymenu: ++, cur_val=%d, max_val=%d", r.cur_val,r.max_val);
		last &= r.cur_val == r.max_val;
		return true;
	}
	else
		return false;
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
			r.cur_val--;

		Msg("mp_buymenu: %s --, cur_val=%d, max_val=%d",item, r.cur_val,r.max_val);
	}
	else
	{
//		rank_rest& rest = m_base_rank_rest;
		it = m_base_rank_rest.find(item);
		if (m_base_rank_rest.end() != it)
		{
			RESTR_N& r = (*it).second;
			if (r.cur_val >= 0)
				r.cur_val--;

			Msg("mp_buymenu: %s --, cur_val=%d, max_val=%d",item, r.cur_val,r.max_val);
		}

	}
	Msg("Return(%s)",item);
	return;
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

