//////////////////////////////////////////////////////////////////////////
// character_community.cpp:		структура представления группировки
//							
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_community.h"


#define GAME_RELATIONS_SECT "game_relations"
#define GAME_COMMUNITIES	"communities"

CHARACTER_COMMUNITY::GOODWILL_TABLE*  CHARACTER_COMMUNITY::m_pCommunityRelationTable = NULL;

//////////////////////////////////////////////////////////////////////////
COMMUNITY_DATA::COMMUNITY_DATA (CHARACTER_COMMUNITY_INDEX idx, CHARACTER_COMMUNITY_ID idn, LPCSTR team_str)
{
	index = idx;
	id = idn;
	team = (u8)atoi(team_str);
}

//////////////////////////////////////////////////////////////////////////
CHARACTER_COMMUNITY::CHARACTER_COMMUNITY	()
{
	m_current_index = NO_COMMUNITY_INDEX;
}
CHARACTER_COMMUNITY::~CHARACTER_COMMUNITY	()
{
}


void  CHARACTER_COMMUNITY::set	(CHARACTER_COMMUNITY_ID id)
{
	m_current_index	 = IdToIndex(id);

}
void  CHARACTER_COMMUNITY::set	(CHARACTER_COMMUNITY_INDEX index)
{
	m_current_index = index;
}

CHARACTER_COMMUNITY_ID		 CHARACTER_COMMUNITY::id			() const
{
	return IndexToId(m_current_index);
}
CHARACTER_COMMUNITY_INDEX	 CHARACTER_COMMUNITY::index			() const
{
	return m_current_index;
}

u8							 CHARACTER_COMMUNITY::team			() const
{
	return ItemDataVector()[m_current_index].team;
}


void CHARACTER_COMMUNITY::InitIdToIndex	()
{
	section_name = GAME_RELATIONS_SECT;
	line_name = GAME_COMMUNITIES;
}


CHARACTER_COMMUNITY::GOODWILL_TABLE& CHARACTER_COMMUNITY::relation_table	()
{

	if(m_pCommunityRelationTable)
		return *m_pCommunityRelationTable;

	m_pCommunityRelationTable = xr_new<GOODWILL_TABLE>();

	LPCSTR table_sect = "communities_relations";
	std::size_t table_size = GetMaxIndex()+1;

	m_pCommunityRelationTable->resize(table_size);

	string64 buffer;
	CInifile::Sect&	relations = pSettings->r_section(table_sect);

	R_ASSERT3(relations.size() == table_size, "wrong size for table in section", table_sect);

	for (CInifile::SectIt i = relations.begin(); relations.end() != i; ++i)
	{
		CHARACTER_COMMUNITY_INDEX cur_index = IdToIndex((*i).first);
		
		if(NO_COMMUNITY_INDEX == cur_index)
			Debug.fatal("wrong community %s in section [%s],  (*i).first, table_sect");
		
		(*m_pCommunityRelationTable)[cur_index].resize(table_size);
		for(std::size_t j=0; j<table_size; j++)
		{
			(*m_pCommunityRelationTable)[cur_index][j] = (CHARACTER_GOODWILL)atoi(_GetItem(*(*i).second,(int)j,buffer));
		}
	}

	return *m_pCommunityRelationTable;
}

CHARACTER_GOODWILL CHARACTER_COMMUNITY::relation		(CHARACTER_COMMUNITY_INDEX to)
{
	return relation(m_current_index, to);
}

CHARACTER_GOODWILL  CHARACTER_COMMUNITY::relation		(CHARACTER_COMMUNITY_INDEX from, CHARACTER_COMMUNITY_INDEX to)
{
	VERIFY(from	<(int)relation_table().size());
	VERIFY(to	<(int)relation_table().size());
	return relation_table()[from][to];
}

void CHARACTER_COMMUNITY::DeleteIdToIndexData	()
{
	xr_delete(m_pCommunityRelationTable);
	inherited::DeleteIdToIndexData();
}