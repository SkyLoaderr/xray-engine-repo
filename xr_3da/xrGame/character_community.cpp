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
	m_current_index = NO_COMMUNITY_INDEX;
	communities_names = NULL;
}
CHARACTER_COMMUNITY::~CHARACTER_COMMUNITY	()
{
}


void  CHARACTER_COMMUNITY::set	(CHARACTER_COMMUNITY_ID id)
{
	m_current_index	 = index_by_id(id);

}
void  CHARACTER_COMMUNITY::set	(CHARACTER_COMMUNITY_INDEX index)
{
	m_current_index = index;
}

CHARACTER_COMMUNITY_ID		 CHARACTER_COMMUNITY::id			() const
{
	return id_by_index(m_current_index);
}
CHARACTER_COMMUNITY_INDEX	 CHARACTER_COMMUNITY::index			() const
{
	return m_current_index;
}

u8							  CHARACTER_COMMUNITY::team			() const
{
	return CommunitiesNames()[m_current_index].team;
}

//////////////////////////////////////////////////////////////////////////
const CHARACTER_COMMUNITY::COMMUNITIES_NAMES& CHARACTER_COMMUNITY::CommunitiesNames	()
{
	if(!communities_names)
	{
		communities_names = xr_new<COMMUNITIES_NAMES>();
		LPCSTR				cfgRecord = pSettings->r_string(GAME_RELATIONS_SECT, GAME_COMMUNITIES); VERIFY(cfgRecord);
		u32					count		= _GetItemCount(cfgRecord);
		
		string64 buf;
		for (u32 k = 0; k < count; k+=2)
		{
			COMMUNITY_DATA comm_data;
			LPCSTR comm_name  = _GetItem(cfgRecord, k, buf);
			char* comm_lwr = xr_strdup(comm_name);
			xr_strlwr(comm_lwr);
			comm_data.id = comm_name;
			xr_free(comm_lwr);

			u8 team_idx = (u8)atoi(_GetItem(cfgRecord, k+1, buf));
			
			comm_data.team = team_idx;
			communities_names->push_back(comm_data);
		}
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
		const COMMUNITIES_NAMES& names = CommunitiesNames();
		for(std::size_t i=0; i<names.size(); i++)
		{
			if(names[i].id == id)
				return (CHARACTER_COMMUNITY_INDEX)i;
		}
		return NO_COMMUNITY_INDEX;
	}
}

CHARACTER_COMMUNITY_ID		 CHARACTER_COMMUNITY::id_by_index		(CHARACTER_COMMUNITY_INDEX index) const
{
	if(index == NO_COMMUNITY_INDEX)
		return NO_COMMUNITY_ID;
	else
		return CommunitiesNames()[index].id;
}