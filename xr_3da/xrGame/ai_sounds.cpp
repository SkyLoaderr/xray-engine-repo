#include "stdafx.h"
#include "ai_sounds.h"

float ffGetStartVolume(ESoundTypes eType)
{
	if (eType & SOUND_TYPE_WEAPON) {
		if (eType & SOUND_TYPE_WEAPON_RECHARGING)
			return(.5f);
		else if (eType & SOUND_TYPE_WEAPON_SHOOTING)
			return(1.f);
		else if (eType & SOUND_TYPE_WEAPON_TAKING)
			return(.3f);
		else if (eType & SOUND_TYPE_WEAPON_HIDING)
			return(.3f);
		else if (eType & SOUND_TYPE_WEAPON_EMPTY_CLICKING)
			return(.3f);
		else if (eType & SOUND_TYPE_WEAPON_BULLET_RICOCHET)
			return(.8f);
	}
	else
		if (eType & SOUND_TYPE_MONSTER) {
			if (eType & SOUND_TYPE_MONSTER_DYING)
				return(.5f);
			else if (eType & SOUND_TYPE_MONSTER_INJURING)
				return(.8f);
			else if (eType & SOUND_TYPE_MONSTER_WALKING_NORMAL)
				return(.3f);
			else if (eType & SOUND_TYPE_MONSTER_WALKING_CROUCH)
				return(.15f);
			else if (eType & SOUND_TYPE_MONSTER_WALKING_LIE)
				return(.25f);
			else if (eType & SOUND_TYPE_MONSTER_RUNNING_NORMAL)
				return(.6f);
			else if (eType & SOUND_TYPE_MONSTER_RUNNING_CROUCH)
				return(.4f);
			else if (eType & SOUND_TYPE_MONSTER_RUNNING_LIE)
				return(.5f);
			else if (eType & SOUND_TYPE_MONSTER_JUMPING_NORMAL)
				return(.6f);
			else if (eType & SOUND_TYPE_MONSTER_JUMPING_CROUCH)
				return(.5f);
			else if (eType & SOUND_TYPE_MONSTER_FALLING)
				return(.7f);
			else if (eType & SOUND_TYPE_MONSTER_TALKING)
				return(.8f);
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
