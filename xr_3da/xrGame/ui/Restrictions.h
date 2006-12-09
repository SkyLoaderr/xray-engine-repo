#pragma once

u32 get_rank(const shared_str &section);

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
	const u32  				GetRank					() const				{return m_rank;}
	bool 					IsAvailable				(const shared_str& itm);
    u32  					GetItemCount			(const shared_str& itm)								const;
	void 					SetRank					(u32 rank)				{m_rank = rank;}

protected:
	void					Dump					() const;

private:	
	bool					HasAmountControl		(const shared_str& item)							const;
	void					AddGroup				(LPCSTR group, LPCSTR lst);
	bool					IsGroupExist			(const shared_str& group)							const;
	void					AddRestriction4rank		(u32 rank, const shared_str& lst);
	shared_str				GetItemGroup			(const shared_str& item)							const;
	RESTR					GetRestr				(const shared_str& item);

	u32						m_rank;
	bool					m_bInited;
	
	DEF_VECTOR(group_items, shared_str);
	DEF_MAP(Groups, shared_str, group_items);
	Groups											m_goups;


	typedef		std::pair<shared_str, u32>			restr_item;
	DEF_VECTOR(rank_rest_vec, restr_item);
	rank_rest_vec									m_restrictions[_RANK_COUNT+1];

	const restr_item*		find_restr_item			(const u32& rank, const shared_str& what ) const;
};
extern CRestrictions g_mp_restrictions;
