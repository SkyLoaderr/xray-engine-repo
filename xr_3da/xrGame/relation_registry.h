//////////////////////////////////////////////////////////////////////////
// relation_registry.h: реестр для хранения данных об отношении персонажа к 
//						другим персонажам
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "character_info_defs.h"

class CRelationRegistryWrapper;

//////////////////////////////////////////////////////////////////////////

struct RELATION_REGISTRY 
{
public:
	RELATION_REGISTRY  ();
	virtual ~RELATION_REGISTRY ();

	void				 Init					(u16 id, CHARACTER_COMMUNITY_INDEX);


public:	
	ALife::ERelationType GetRelationType		(u16 person_id, CHARACTER_COMMUNITY_INDEX comm_index) const ;
	void				 SetRelationType		(u16 person_id, CHARACTER_COMMUNITY_INDEX comm_index, ALife::ERelationType new_relation);

	//общее отношение  одного персонажа к другому, вычисленное по формуле
	//с учетом всех факторов - величина от 
	//-100< (крайне враждебное) до >100 (очень дрюжелюбное)
	CHARACTER_GOODWILL	 GetAttitude			(u16 person_id, CHARACTER_COMMUNITY_INDEX)	const ;

	//личное отношение (благосклонность) одного персонажа к другому - 
	//величина от -100< (крайне враждебное) до >100 (очень дрюжелюбное)
	CHARACTER_GOODWILL	 GetGoodwill			(u16 person_id) const ;
	void				 SetGoodwill			(u16 person_id, CHARACTER_GOODWILL goodwill);
	void				 ChangeGoodwill 		(u16 person_id, CHARACTER_GOODWILL delta_goodwill);

	//отношения группировки к персонажу (именно так, а не наоборот)
	//т.е. персонаж сам помнит, как к нему какая группировка отностися
	CHARACTER_GOODWILL	 GetCommunityGoodwill		(CHARACTER_COMMUNITY_INDEX) const ;
	void				 SetCommunityGoodwill		(CHARACTER_COMMUNITY_INDEX, CHARACTER_GOODWILL goodwill);
	void				 ChangeCommunityGoodwill	(CHARACTER_COMMUNITY_INDEX, CHARACTER_GOODWILL delta_goodwill);
	
	void				 ClearRelations			();

private:
	CRelationRegistryWrapper					*relation_registry;
	CHARACTER_COMMUNITY_INDEX					m_iOurCommunity;
};