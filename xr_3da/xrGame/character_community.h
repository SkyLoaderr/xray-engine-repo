//////////////////////////////////////////////////////////////////////////
// character_community.h:	структура представления группировки
//							
//////////////////////////////////////////////////////////////////////////

#pragma once


typedef shared_str				CHARACTER_COMMUNITY_ID;
#define NO_COMMUNITY_ID			CHARACTER_COMMUNITY_ID(NULL)

typedef int						CHARACTER_COMMUNITY_INDEX;
#define NO_COMMUNITY_INDEX		CHARACTER_COMMUNITY_INDEX(-1)

struct CHARACTER_COMMUNITY
{
	CHARACTER_COMMUNITY		();
	~CHARACTER_COMMUNITY	();

	void						set				(CHARACTER_COMMUNITY_ID);
	void						set				(CHARACTER_COMMUNITY_INDEX);

	CHARACTER_COMMUNITY_ID		id				() const;
	CHARACTER_COMMUNITY_INDEX	index			() const;

private:
	CHARACTER_COMMUNITY_ID		m_current_id;
	CHARACTER_COMMUNITY_INDEX	m_current_index;

public:
	CHARACTER_COMMUNITY_INDEX	index_by_id		(CHARACTER_COMMUNITY_ID)	const;
	CHARACTER_COMMUNITY_ID		id_by_index		(CHARACTER_COMMUNITY_INDEX) const;

	typedef std::vector<CHARACTER_COMMUNITY_ID> COMMUNITIES_NAMES;
	static  const COMMUNITIES_NAMES&	CommunitiesNames		();
	static void							DeleteCommunitiesNames	();
private:
	static COMMUNITIES_NAMES* communities_names;
};