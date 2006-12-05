#pragma once

#define _RANK_COUNT 5

typedef struct 
{
	shared_str		name;
	int				n;
} RESTR;

class CRestrictions 
{
public:
							CRestrictions			();
							~CRestrictions			();	

	void 					InitGroups				();
	const int  				GetRank					() const				{return m_rank;}
	bool 					IsAvailable				(const shared_str& itm);
	int	 					GetItemRank				(const shared_str& itm)								const;
    int  					GetItemCount			(const shared_str& itm)								const;
	void 					SetRank					(int rank)				{m_rank = rank;}
protected:

	bool 					IsInRank				(const shared_str& itm, int rank)					const;


private:	
	bool					HasAmountControl		(const shared_str& item)							const;
	void					AddGroup				(LPCSTR group, LPCSTR lst);
	bool					IsGroupExist			(const shared_str& group)							const;
	void					CopyGroup				(const shared_str& from, const shared_str& to);
	void					AddRestriction4rank		(int rank, const shared_str& lst);
	shared_str				GetItemGroup			(const shared_str& item)							const;
	RESTR					GetRestr				(const shared_str& item);
	void					FillUpRank				(const shared_str& rank, int i_rank);

	int						m_rank;
	bool					m_bInited;
	
	DEF_VECTOR(group_items, shared_str);
//.	typedef xr_vector<xr_string>						group_items;
//.	typedef xr_vector<xr_string>::iterator				group_items_it;
	DEF_MAP(Groups, shared_str, group_items);
//.	typedef xr_map<xr_string, group_items>				Groups;
//.	typedef xr_map<xr_string, group_items>::iterator	Groups_it;

	Groups												m_goups;

	DEF_MAP(rank_rest, shared_str, int);
//.	typedef xr_map<xr_string, int>				rank_rest;
//.	typedef xr_map<xr_string, int>::iterator	rank_rest_it;
	DEF_VECTOR(RankRests, rank_rest);
//.	typedef xr_vector<rank_rest>				RankRests;						
	RankRests											m_restrictions;
	rank_rest											m_base_rank_rest;

	DEF_MAP(str_map, shared_str, int);
//.	typedef xr_map<shared_str, int>		str_map;
	DEF_VECTOR(str_maps,str_map);
//.	xr_vector<str_map>					m_items;
	str_maps											m_items;
};
