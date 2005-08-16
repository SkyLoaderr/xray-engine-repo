#pragma once

typedef struct {
	int			max_val;
	int			cur_val;
} RESTR_N;

typedef struct {
	xr_string	name;
	RESTR_N		n;
} RESTR;

class CRestrictions {
public:
	CRestrictions();
	~CRestrictions();

	bool GetPermission(LPCSTR item);
	void Return(LPCSTR item);

	int  GetRank			()			{return m_rank;}
	void SetRank			(int rank)	{m_rank = rank;}
private:
	void InitGroups();
	void AddGroup			(LPCSTR group,	LPCSTR lst);
	bool IsGroupExist		(LPCSTR group);
	void CopyGroup			(LPCSTR from,	LPCSTR to);
	void AddRestriction4rank(int rank,		LPCSTR lst);
	LPCSTR GetItemGroup		(LPCSTR item);
	RESTR GetRestr			(LPCSTR item);

	int									m_rank;
	
	typedef xr_vector<xr_string>						group_items;
	typedef xr_vector<xr_string>::iterator				group_items_it;
	typedef xr_map<xr_string, group_items>				Groups;
	typedef xr_map<xr_string, group_items>::iterator	Groups_it;
	Groups								m_goups;

	typedef xr_map<xr_string, RESTR_N>				rank_rest;
	typedef xr_map<xr_string, RESTR_N>::iterator	rank_rest_it;
	typedef xr_vector<rank_rest>					RankRests;						
	RankRests							m_restrictions;

	

	//void FillUpRestriction(rank_rest& rest, LPCSTR lst);
};
