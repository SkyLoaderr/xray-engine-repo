//////////////////////////////////////////////////////////////////////////
// character_community.cpp:		структура представления группировки
//							
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_community.h"


#define GAME_RELATIONS_SECT "game_relations"
#define GAME_COMMUNITIES	"communities"



//////////////////////////////////////////////////////////////////////////

CHARACTER_COMMUNITY::COMMUNITIES_NAMES* CHARACTER_COMMUNITY::communities_names = NULL;

CHARACTER_COMMUNITY::CHARACTER_COMMUNITY	()
{
	m_current_id = NO_COMMUNITY_ID;
	m_current_index = NO_COMMUNITY_INDEX;

	communities_names = NULL;
}
CHARACTER_COMMUNITY::~CHARACTER_COMMUNITY	()
{
}


void  CHARACTER_COMMUNITY::set	(CHARACTER_COMMUNITY_ID id)
{
	m_current_id	 = id;
	m_current_index	 = index_by_id(m_current_id);

}
void  CHARACTER_COMMUNITY::set	(CHARACTER_COMMUNITY_INDEX index)
{
	m_current_index = index;
	m_current_id = id_by_index(m_current_index);
}

CHARACTER_COMMUNITY_ID		 CHARACTER_COMMUNITY::id			() const
{
	return m_current_id;
}
CHARACTER_COMMUNITY_INDEX	 CHARACTER_COMMUNITY::index			() const
{
	return m_current_index;
}

//////////////////////////////////////////////////////////////////////////
const CHARACTER_COMMUNITY::COMMUNITIES_NAMES& CHARACTER_COMMUNITY::CommunitiesNames	()
{
	if(!communities_names)
	{
		communities_names = xr_new<COMMUNITIES_NAMES>();
		pSettings->r_string(GAME_RELATIONS_SECT, GAME_COMMUNITIES);

		/*		LPCSTR				cfgRecord	= pSettings->r_string(section, field);
		u32					count		= _GetItemCount(cfgRecord);
		string32			singleThreshold;
		ZeroMemory			(singleThreshold, sizeof(singleThreshold));
		int					upBoundThreshold	= 0;
		CharInfoStringID	id;

		for (u32 k = 0; k < count; k += 2)
		{
		_GetItem(cfgRecord, k, singleThreshold);
		id.second = singleThreshold;

		if (count == k + 1)
		{
		// Indicate greatest value
		id.first = hugeValue;
		}
		else
		{
		_GetItem(cfgRecord, k + 1, singleThreshold);
		sscanf(singleThreshold, "%i", &upBoundThreshold);
		id.first = upBoundThreshold;
		}
		container->insert(id);
		}*/
	}

	return *communities_names;
}
void CHARACTER_COMMUNITY::DeleteCommunitiesNames()
{
	delete (communities_names);
}

CHARACTER_COMMUNITY_INDEX	 CHARACTER_COMMUNITY::index_by_id		(CHARACTER_COMMUNITY_ID id)	const
{
	if(id == NO_COMMUNITY_ID)
		return NO_COMMUNITY_INDEX;
	else
	{
/*		COMMUNITIES_NAMES& names = CommunitiesNames();
		for(std::size_t i=0; i<names.size(); i++)
		{
		}*/
		return 0;//NO_COMMUNITY_INDEX;
	}
}
CHARACTER_COMMUNITY_ID		 CHARACTER_COMMUNITY::id_by_index		(CHARACTER_COMMUNITY_INDEX index) const
{
	if(index == NO_COMMUNITY_INDEX)
		return NO_COMMUNITY_ID;
	else
		return CommunitiesNames()[index];
}