//////////////////////////////////////////////////////////////////////////
// character_info_defs.h		игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

typedef int					PROFILE_INDEX;
typedef shared_str			PROFILE_ID;
#define NO_PROFILE			PROFILE_INDEX(-1)
#define DEFAULT_PROFILE		PROFILE_INDEX(0)


typedef int					SPECIFIC_CHARACTER_INDEX;
typedef shared_str			SPECIFIC_CHARACTER_ID;
#define NO_SPECIFIC_CHARACTER	SPECIFIC_CHARACTER_INDEX(-1)

//репутация персонажа - величина от -100 (очень плохой, беспредельщик) 
//до 100 (очень хороший, благородный)
typedef int				CHARACTER_REPUTATION;
#define NO_REPUTATION	CHARACTER_REPUTATION(-1)

//ранг персонажа - величина от 0 (совсем неопытный) 
//до 100 (очень опытный)
typedef int			CHARACTER_RANK;
#define NO_RANK		CHARACTER_RANK(-1)

//личное отношение (благосклонность) одного персонажа к другому - 
//величина от 0 (крайне враждебное) до 100 (очень дрюжелюбное)
typedef int			CHARACTER_GOODWILL;
#define NO_GOODWILL	CHARACTER_GOODWILL(-1)

//общее отношение  одного персонажа к другому, вычисленное по формуле
//с учетом всех факторов - величина от 
//0 (крайне враждебное) до 100 (очень дрюжелюбное)
typedef int			CHARACTER_ATTITUDE;
#define NO_ATTITUDE	CHARACTER_ATTITUDE(-1)


//структура, описывающая отношение одного персонажа к другому
struct SRelation
{
	SRelation();
	~SRelation();
	CHARACTER_GOODWILL		Goodwill		() const;
	void					SetGoodwill		(CHARACTER_GOODWILL new_goodwill);
private:
	//благосклонность
	CHARACTER_GOODWILL m_iGoodwill;
};

DEFINE_MAP(u16, SRelation, RELATION_MAP, RELATION_MAP_IT);