//////////////////////////////////////////////////////////////////////////
// character_info_defs.h		игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

#define NO_GOODWILL -1

typedef int					PROFILE_INDEX;
typedef ref_str				PROFILE_ID;
#define NO_PROFILE			PROFILE_INDEX(-1)
#define DEFAULT_PROFILE		PROFILE_INDEX(0)


typedef int			SPECIFIC_CHARACTER_INDEX;
typedef ref_str		SPECIFIC_CHARACTER_ID;
#define NO_SPECIFIC_CHARACTER	SPECIFIC_CHARACTER_INDEX(-1)

//репутация персонажа - величина от 0 (очень плохой, беспредельщик) 
//до 100 (очень хороший, благородный)
typedef int				CHARACTER_REPUTATION;
#define NO_REPUTATION	CHARACTER_REPUTATION(-1)

//ранг персонажа - величина от 0 (совсем неопытный) 
//до 100 (очень опытный)
typedef int			CHARACTER_RANK;
#define NO_RANK		CHARACTER_RANK(-1)

typedef ref_str			CHARACTER_COMMUNITY;
#define NO_COMMUNITY	CHARACTER_COMMUNITY(NULL)

//структура, описывающая отношение одного персонажа к другому
struct SRelation
{
	SRelation();
	~SRelation();

	ALife::ERelationType	RelationType	() const;
	void					SetRelationType	(ALife::ERelationType relation);

	int						Goodwill		() const;
	void					SetGoodwill		(int new_goodwill);
private:
	//отношения (враг, нейтрал, друг)
	ALife::ERelationType m_eRelationType;
	//благосклонность
	int m_iGoodwill;
};

DEFINE_MAP(u16, SRelation, RELATION_MAP, RELATION_MAP_IT);