#include "stdafx.h"
#include "ai_sounds.h"

float ffGetStartVolume(ESoundTypes eType)
{
	if ((eType & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON) {
		if ((eType & SOUND_TYPE_WEAPON_RECHARGING) == SOUND_TYPE_WEAPON_RECHARGING)
			return(.15f);
		else if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
			return(1.0f);
		else if ((eType & SOUND_TYPE_ITEM_TAKING) == SOUND_TYPE_ITEM_TAKING)
			return(.1f);																		
		else if ((eType & SOUND_TYPE_ITEM_HIDING) == SOUND_TYPE_ITEM_HIDING)
			return(.1f);
		else if ((eType & SOUND_TYPE_WEAPON_EMPTY_CLICKING) == SOUND_TYPE_WEAPON_EMPTY_CLICKING)
			return(.1f);
		else if ((eType & SOUND_TYPE_WEAPON_BULLET_HIT) == SOUND_TYPE_WEAPON_BULLET_HIT)
			return(.1f);
	}
	else
		if ((eType & SOUND_TYPE_MONSTER) == SOUND_TYPE_MONSTER) {
			if ((eType & SOUND_TYPE_MONSTER_DYING) == SOUND_TYPE_MONSTER_DYING)
				return(.3f);
			else if ((eType & SOUND_TYPE_MONSTER_INJURING) == SOUND_TYPE_MONSTER_INJURING)
				return(.5f);
			else if ((eType & SOUND_TYPE_MONSTER_STEP) == SOUND_TYPE_MONSTER_STEP)
				return(.2f);
			else if ((eType & SOUND_TYPE_MONSTER_TALKING) == SOUND_TYPE_MONSTER_TALKING)
				return(.3f);
			else if ((eType & SOUND_TYPE_MONSTER_ATTACKING) == SOUND_TYPE_MONSTER_ATTACKING)
				return(.5f);
		}
		else
			if ((eType & SOUND_TYPE_WORLD) == SOUND_TYPE_WORLD) {
				if ((eType & SOUND_TYPE_WORLD_OBJECT_BREAKING) == SOUND_TYPE_WORLD_OBJECT_BREAKING)
					return(.8f);
				else if ((eType & SOUND_TYPE_WORLD_OBJECT_COLLIDING) == SOUND_TYPE_WORLD_OBJECT_COLLIDING)
					return(.7f);
			}
			else
				Msg("Invalid ref_sound type : %x",eType);
	return(1.f);
}
