#include "stdafx.h"
#include "ai_sounds.h"

float ffGetStartVolume(ESoundTypes eType)
{
	if (eType & SOUND_TYPE_WEAPON) {
		if (eType & SOUND_TYPE_WEAPON_RECHARGING)
			return(.15f);
		else if (eType & SOUND_TYPE_WEAPON_SHOOTING)
			return(1.f);
		else if (eType & SOUND_TYPE_WEAPON_TAKING)
			return(.1f);
		else if (eType & SOUND_TYPE_WEAPON_HIDING)
			return(.1f);
		else if (eType & SOUND_TYPE_WEAPON_EMPTY_CLICKING)
			return(.1f);
		else if (eType & SOUND_TYPE_WEAPON_BULLET_RICOCHET)
			return(.5f);
	}
	else
		if (eType & SOUND_TYPE_MONSTER) {
			if (eType & SOUND_TYPE_MONSTER_DYING)
				return(.2f);
			else if (eType & SOUND_TYPE_MONSTER_INJURING)
				return(.15f);
			else if (eType & SOUND_TYPE_MONSTER_WALKING)
				return(.1f);
			else if (eType & SOUND_TYPE_MONSTER_JUMPING)
				return(.2f);
			else if (eType & SOUND_TYPE_MONSTER_FALLING)
				return(.25f);
			else if (eType & SOUND_TYPE_MONSTER_TALKING)
				return(.2f);
		}
		else
			if (eType & SOUND_TYPE_WORLD) {
				if (eType & SOUND_TYPE_WORLD_DOOR_OPENING)
					return(.3f);
				else if (eType & SOUND_TYPE_WORLD_DOOR_CLOSING)
					return(.5f);
				else if (eType & SOUND_TYPE_WORLD_WINDOW_BREAKING)
					return(.8f);
				else if (eType & SOUND_TYPE_WORLD_OBJECT_FALLING)
					return(.7f);
			}
			else
				Msg("Invalid sound type : %x",eType);
	return(1.f);
}
