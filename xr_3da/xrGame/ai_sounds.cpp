#include "stdafx.h"
#include "ai_sounds.h"

float ffGetStartVolume(ESoundTypes eType)
{
	if ((eType & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON) {
		if ((eType & SOUND_TYPE_WEAPON_RECHARGING) == SOUND_TYPE_WEAPON_RECHARGING)
			return(.15f);
		else if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
			return(1.0f);
		else if ((eType & SOUND_TYPE_WEAPON_TAKING) == SOUND_TYPE_WEAPON_TAKING)
			return(.1f);																		
		else if ((eType & SOUND_TYPE_WEAPON_HIDING) == SOUND_TYPE_WEAPON_HIDING)
			return(.1f);
		else if ((eType & SOUND_TYPE_WEAPON_CHANGING) == SOUND_TYPE_WEAPON_CHANGING)
			return(.1f);
		else if ((eType & SOUND_TYPE_WEAPON_EMPTY_CLICKING) == SOUND_TYPE_WEAPON_EMPTY_CLICKING)
			return(.1f);
		else if ((eType & SOUND_TYPE_WEAPON_BULLET_RICOCHET) == SOUND_TYPE_WEAPON_BULLET_RICOCHET)
			return(.1f);
	}
	else
		if ((eType & SOUND_TYPE_MONSTER) == SOUND_TYPE_MONSTER) {
			if ((eType & SOUND_TYPE_MONSTER_DYING) == SOUND_TYPE_MONSTER_DYING)
				return(.3f);
			else if ((eType & SOUND_TYPE_MONSTER_INJURING) == SOUND_TYPE_MONSTER_INJURING)
				return(.15f);
			else if ((eType & SOUND_TYPE_MONSTER_WALKING) == SOUND_TYPE_MONSTER_WALKING)
				return(.05f);
			else if ((eType & SOUND_TYPE_MONSTER_JUMPING) == SOUND_TYPE_MONSTER_JUMPING)
				return(.06f);
			else if ((eType & SOUND_TYPE_MONSTER_FALLING) == SOUND_TYPE_MONSTER_FALLING)
				return(.06f);
			else if ((eType & SOUND_TYPE_MONSTER_TALKING) == SOUND_TYPE_MONSTER_TALKING)
				return(.10f);
			else if ((eType & SOUND_TYPE_MONSTER_ATTACKING_ANIMAL) == SOUND_TYPE_MONSTER_ATTACKING_ANIMAL)
				return(.3f);
		}
		else
			if ((eType & SOUND_TYPE_WORLD) == SOUND_TYPE_WORLD) {
				if ((eType & SOUND_TYPE_WORLD_DOOR_OPENING) == SOUND_TYPE_WORLD_DOOR_OPENING)
					return(.3f);
				else if ((eType & SOUND_TYPE_WORLD_DOOR_CLOSING) == SOUND_TYPE_WORLD_DOOR_CLOSING)
					return(.5f);
				else if ((eType & SOUND_TYPE_WORLD_WINDOW_BREAKING) == SOUND_TYPE_WORLD_WINDOW_BREAKING)
					return(.8f);
				else if ((eType & SOUND_TYPE_WORLD_OBJECT_FALLING) == SOUND_TYPE_WORLD_OBJECT_FALLING)
					return(.7f);
			}
			else
				Msg("Invalid sound type : %x",eType);
	return(1.f);
}
