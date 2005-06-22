///////////////////////////////////
// class CUIOptionsManager
///////////////////////////////////

#pragma once

class CUIOptionsItem;

class CUIOptionsManager{
	friend class CUIOptionsItem;
public:
	void SetCurrentValues(const char* group);
	void SaveValues(const char* group);
protected:
	void RegisterItem(CUIOptionsItem* item, const char* group);

	typedef	xr_string									group_name;
	typedef xr_vector<CUIOptionsItem*>					items_list;
    typedef xr_map<group_name, items_list>				groups;
	typedef xr_map<group_name, items_list>::iterator	groups_it;

	groups	m_groups;
};