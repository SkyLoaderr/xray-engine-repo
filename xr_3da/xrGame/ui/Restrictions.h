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
	friend class CUIDragDropItemMP;
	friend class CUIBag;
	friend class CUIBagWnd;
public:
	CRestrictions();
	~CRestrictions();	
protected:
	void InitGroups();
	void SetRank			(int rank)		{m_rank = rank;}
	int  GetRank			()				{return m_rank;}
	bool GetPermission		(LPCSTR item, bool& last);
	void Return				(LPCSTR item);
	bool HasAmountControl	(LPCSTR item);

private:	
	void	AddGroup			(LPCSTR group,	LPCSTR lst);
	bool	IsGroupExist		(LPCSTR group);
	void	CopyGroup			(LPCSTR from,	LPCSTR to);
	void	AddRestriction4rank	(int rank,		LPCSTR lst);
	LPCSTR	GetItemGroup		(LPCSTR item);
	RESTR	GetRestr			(LPCSTR item);
IC	bool	Try					(RESTR_N& r, bool& last);

	int									m_rank;
	bool								m_bInited;
	
	typedef xr_vector<xr_string>						group_items;
	typedef xr_vector<xr_string>::iterator				group_items_it;
	typedef xr_map<xr_string, group_items>				Groups;
	typedef xr_map<xr_string, group_items>::iterator	Groups_it;
	Groups								m_goups;

	typedef xr_map<xr_string, RESTR_N>				rank_rest;
	typedef xr_map<xr_string, RESTR_N>::iterator	rank_rest_it;
	typedef xr_vector<rank_rest>					RankRests;						
	RankRests							m_restrictions;
	rank_rest							m_base_rank_rest;
};
