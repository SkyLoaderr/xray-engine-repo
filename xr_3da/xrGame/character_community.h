//////////////////////////////////////////////////////////////////////////
// character_community.h:	структура представления группировки
//							
//////////////////////////////////////////////////////////////////////////

#pragma once


#include "ini_id_loader.h"
#include "character_info_defs.h"


struct COMMUNITY_DATA
{
	COMMUNITY_DATA (CHARACTER_COMMUNITY_INDEX, CHARACTER_COMMUNITY_ID, LPCSTR);

	CHARACTER_COMMUNITY_ID		id;
	CHARACTER_COMMUNITY_INDEX	index;
	u8 team;
};


class CHARACTER_COMMUNITY;

class CHARACTER_COMMUNITY: 
	public CIni_IdToIndex<1, COMMUNITY_DATA, CHARACTER_COMMUNITY_ID, CHARACTER_COMMUNITY_INDEX, CHARACTER_COMMUNITY>
{
private:
	typedef CIni_IdToIndex<1, COMMUNITY_DATA, CHARACTER_COMMUNITY_ID, CHARACTER_COMMUNITY_INDEX, CHARACTER_COMMUNITY> inherited;
	friend inherited;

public:
	CHARACTER_COMMUNITY			();
	~CHARACTER_COMMUNITY		();

	void						set				(CHARACTER_COMMUNITY_ID);
	void						set				(CHARACTER_COMMUNITY_INDEX);

	CHARACTER_COMMUNITY_ID		id				() const;
	CHARACTER_COMMUNITY_INDEX	index			() const;
	u8							team			() const;

private:
	CHARACTER_COMMUNITY_INDEX	m_current_index;

	static	void				InitIdToIndex	();


public:
	typedef						xr_vector<CHARACTER_GOODWILL>			GOODWILL_VECTOR;
	typedef						xr_vector<GOODWILL_VECTOR>				GOODWILL_TABLE;

	//отношение между группировками
	static GOODWILL_TABLE&		relation_table		();
	static CHARACTER_GOODWILL	relation			(CHARACTER_COMMUNITY_INDEX from, CHARACTER_COMMUNITY_INDEX to);
	CHARACTER_GOODWILL			relation			(CHARACTER_COMMUNITY_INDEX to);
	
	static void					DeleteIdToIndexData	();

private:
	static GOODWILL_TABLE*		m_pCommunityRelationTable;
};