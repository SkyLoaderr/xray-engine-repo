//////////////////////////////////////////////////////////////////////////
// character_rank.cpp:	структура представления рангов и отношений между 
//						ними		
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_rank.h"


#define GAME_RELATIONS_SECT		"game_relations"
#define RANK_LINE				"rating"
#define RANK_TABLE				"rank_relations"

//////////////////////////////////////////////////////////////////////////
RANK_DATA::RANK_DATA (int idx, shared_str idn, LPCSTR team_str)
{
	index = idx;
	id = idn;
	threshold = (u8)atoi(team_str);
}

//////////////////////////////////////////////////////////////////////////
CHARACTER_RANK::CHARACTER_RANK	()
{
	m_current_value = NO_RANK;
}
CHARACTER_RANK::~CHARACTER_RANK	()
{
}


int   CHARACTER_RANK::ValueToIndex    (CHARACTER_RANK_VALUE val)
{
	for(int i=0; i<(int)ItemDataVector().size(); i++)
	{
		if(val < ItemDataVector()[i].threshold)
			break;
	}
	return i;
}

void  CHARACTER_RANK::set	(CHARACTER_RANK_VALUE new_val)
{
	m_current_value = new_val;
	m_current_index = ValueToIndex(new_val);
}

shared_str					CHARACTER_RANK::id			() const
{
	return IndexToId(m_current_index);
}
int							CHARACTER_RANK::index			() const
{
	return m_current_index;
}

CHARACTER_RANK_VALUE		CHARACTER_RANK::value			() const
{
	return m_current_value;
}


void CHARACTER_RANK::InitIdToIndex	()
{
	section_name	= GAME_RELATIONS_SECT;
	line_name		= RANK_LINE;
	m_relation_table.set_table_sect(RANK_TABLE);
}


CHARACTER_GOODWILL CHARACTER_RANK::relation		(int to)
{
	return relation(m_current_index, to);
}

CHARACTER_GOODWILL  CHARACTER_RANK::relation		(int from, int to)
{
	InitIdToIndex ();

	VERIFY(from >= 0 && from <(int)m_relation_table.table().size());
	VERIFY(to >= 0 && to <(int)m_relation_table.table().size());

	return m_relation_table.table()[from][to];
}

void CHARACTER_RANK::DeleteIdToIndexData	()
{
	m_relation_table.clear();
	inherited::DeleteIdToIndexData();
}