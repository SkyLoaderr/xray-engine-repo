#include "stdafx.h"
#include "map_location.h"

#include "level.h"
#include "ai_space.h"
#include "game_graph.h"

#include "xrServer.h"
#include "xrServer_Objects_ALife.h"

//////////////////////////////////////////////////////////////////////////

SMapLocation::SMapLocation()
{
	info_portion_id				= NO_INFO_INDEX;
	level_name					= NULL;
	x							= y = 0;
	name						= NULL;
	text						= NULL;
	attached_to_object			= false;
	object_id					= 0xffff;
	marker_show					= false;
	icon_color					= 0xffffffff;
	type_flags.zero				();
	animation.Cyclic			(false);
}

//////////////////////////////////////////////////////////////////////////

shared_str SMapLocation::LevelName()
{
	if(!attached_to_object)
		return level_name;
	else
	{
		if(ai().get_alife() && ai().get_game_graph())		
		{
			CSE_Abstract* E = Level().Server->game->get_entity_from_eid(object_id); VERIFY(E);
			CSE_ALifeObject* AO = smart_cast<CSE_ALifeObject*>(E);
			if(AO)	
				return  ai().game_graph().header().level(ai().game_graph().vertex(AO->m_tGraphID)->level_id()).name();
			else 
				return Level().name();
		}
		else
			return Level().name();
	}
}

//////////////////////////////////////////////////////////////////////////

void SMapLocation::UpdateAnimation()
{
	animation.Update();
	icon_color = animation.GetColor();
}

//////////////////////////////////////////////////////////////////////////

void SMapLocation::SetColorAnimation(const shared_str &animationName)
{
	animation.SetColorAnimation(animationName);
	animation.Reset();
}