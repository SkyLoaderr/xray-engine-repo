#include "stdafx.h"
#include "map_location.h"

#include "level.h"
#include "ai_space.h"
#include "game_graph.h"

#include "xrServer.h"
#include "xrServer_Objects_ALife.h"
#include "../LightAnimLibrary.h"

//////////////////////////////////////////////////////////////////////////

SMapLocation::SMapLocation()
{
	info_portion_id			= NO_INFO_INDEX;
	level_name				= NULL;
	x						= y = 0;
	name					= NULL;
	text					= NULL;
	attached_to_object		= false;
	object_id				= 0xffff;
	marker_show				= false;
	icon_color				= 0xffffffff;
	type_flags.zero			();
	colorAnimation			= NULL;
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
	if (colorAnimation)
	{
		// Если не прошли один цикл анимации, то игрпть дальше
		if (animationTime <= (colorAnimation->iFrameCount / colorAnimation->fFPS))
		{
			int	currentFrame	= 0;
			u32 colorBGR		= colorAnimation->CalculateBGR(animationTime, currentFrame);
			icon_color			= color_rgba(color_get_B(colorBGR), color_get_G(colorBGR), color_get_R(colorBGR), 255 /*color_get_A(colorBGR)*/);
			// обновим время
			animationTime		+= Device.fTimeGlobal - prevTimeGlobal;
		}
	}

	prevTimeGlobal = Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

void SMapLocation::SetColorAnimation(const shared_str &animationName)
{
	colorAnimation	= LALib.FindItem(*animationName);
	R_ASSERT(colorAnimation);
	animationTime	= 0.0f;
	prevTimeGlobal	= Device.fTimeGlobal;
}