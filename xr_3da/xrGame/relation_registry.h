//////////////////////////////////////////////////////////////////////////
// relation_registry.h: реестр для хранения данных об отношении персонажа к 
//						другим персонажам
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "character_info_defs.h"

#include "alife_registry_container.h"
#include "alife_registry_wrapper.h"



//////////////////////////////////////////////////////////////////////////

struct RELATION_REGISTRY 
{
public:
	RELATION_REGISTRY  ();
	virtual ~RELATION_REGISTRY ();

	void				 Init					(u16 id);

	ALife::ERelationType GetRelationType		(u16 person_id) const ;
	void				 SetRelationType		(u16 person_id, ALife::ERelationType new_relation);

	CHARACTER_ATTITUDE	 GetAttitude			(u16 person_id) const ;

	CHARACTER_GOODWILL	 GetGoodwill			(u16 person_id) const ;
	void				 SetGoodwill			(u16 person_id, CHARACTER_GOODWILL goodwill);
	void				 ChangeGoodwill 		(u16 person_id, CHARACTER_GOODWILL delta_goodwill);
	
	void				 ClearRelations			();

private:
	CALifeRegistryWrapper<CRelationRegistry>	relation_registry;
};

