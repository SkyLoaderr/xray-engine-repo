//////////////////////////////////////////////////////////////////////////
// character_info_defs.h		игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

#define NO_GOODWILL -1

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