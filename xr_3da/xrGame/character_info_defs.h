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


typedef int						SPECIFIC_CHARACTER_INDEX;
typedef shared_str				SPECIFIC_CHARACTER_ID;
#define NO_SPECIFIC_CHARACTER	SPECIFIC_CHARACTER_INDEX(-1)

//репутация персонажа - величина от -100 (очень плохой, беспредельщик) 
//до 100 (очень хороший, благородный)
typedef int					CHARACTER_REPUTATION;
#define NO_REPUTATION		-type_max(CHARACTER_REPUTATION)
#define NEUTRAL_REPUTATION	CHARACTER_REPUTATION(0)

//ранг персонажа - величина от 0 (совсем неопытный) 
//до >100 (очень опытный)
typedef int			CHARACTER_RANK;
#define NO_RANK		CHARACTER_RANK(-1)

//личное отношение (благосклонность) одного персонажа к другому - 
//величина от -100< (крайне враждебное) до >100 (очень дрюжелюбное)
typedef int					CHARACTER_GOODWILL;
#define NO_GOODWILL			-type_max(CHARACTER_GOODWILL)
#define NEUTRAL_GOODWILL	CHARACTER_GOODWILL(0)

typedef shared_str				CHARACTER_CLASS;
#define NO_CHARACTER_CLASS		NULL


typedef shared_str				CHARACTER_COMMUNITY_ID;
#define NO_COMMUNITY_ID			CHARACTER_COMMUNITY_ID(NULL)

typedef int						CHARACTER_COMMUNITY_INDEX;
#define NO_COMMUNITY_INDEX		CHARACTER_COMMUNITY_INDEX(-1)