#pragma once

//typedef struct {
//	int			max_val;
//	int			cur_val;
//} RESTR_N;

typedef struct {
	xr_string	name;
	int			n;
} RESTR;

class CRestrictions {
	friend class CUIDragDropItemMP;
	friend class CUIBag;
	friend class CUIBagWnd;
	friend class CUIBuyWnd;
public:
	CRestrictions();
	~CRestrictions();	
protected:
	void InitGroups();
	void SetRank			(int rank)		{m_rank = rank;}
	int  GetRank			()				{return m_rank;}

    int  GetItemCount		(LPCSTR itm);
	bool IsAvailable		(LPCSTR itm);


private:	
	bool	HasAmountControl	(LPCSTR item);
	void	AddGroup			(LPCSTR group,	LPCSTR lst);
	bool	IsGroupExist		(LPCSTR group);
	void	CopyGroup			(LPCSTR from,	LPCSTR to);
	void	AddRestriction4rank	(int rank,		LPCSTR lst);
	LPCSTR	GetItemGroup		(LPCSTR item);
	RESTR	GetRestr			(LPCSTR item);
	void	FillUpRank			(LPCSTR rank, int i_rank);

	int									m_rank;
	bool								m_bInited;
	
	typedef xr_vector<xr_string>						group_items;
	typedef xr_vector<xr_string>::iterator				group_items_it;
	typedef xr_map<xr_string, group_items>				Groups;
	typedef xr_map<xr_string, group_items>::iterator	Groups_it;
	Groups								m_goups;

	typedef xr_map<xr_string, int>				rank_rest;
	typedef xr_map<xr_string, int>::iterator	rank_rest_it;
	typedef xr_vector<rank_rest>				RankRests;						
	RankRests							m_restrictions;
	rank_rest							m_base_rank_rest;

	typedef xr_map<shared_str, int>		str_map;
	xr_vector<str_map>					m_items;
};
