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

xr_token anomaly_type_token[]={
	{ "undefined",				st_Undefined						},
	{ "Item picking up",		SOUND_TYPE_ITEM_PICKING_UP			},
	{ "Item dropping",			SOUND_TYPE_ITEM_DROPPING			},
	{ "Item taking",			SOUND_TYPE_ITEM_TAKING				},
	{ "Item hiding",			SOUND_TYPE_ITEM_HIDING				},
	{ "Item using",				SOUND_TYPE_ITEM_USING				},
	{ "Weapon shooting",		SOUND_TYPE_WEAPON_SHOOTING			},
	{ "Weapon empty clicking",	SOUND_TYPE_WEAPON_EMPTY_CLICKING	},
	{ "Weapon bullet hit",		SOUND_TYPE_WEAPON_BULLET_HIT		},
	{ "Weapon recharging",		SOUND_TYPE_WEAPON_RECHARGING		},
	{ "NPC dying",				SOUND_TYPE_MONSTER_DYING			},
	{ "NPC injuring",			SOUND_TYPE_MONSTER_INJURING			},
	{ "NPC step",				SOUND_TYPE_MONSTER_STEP				},
	{ "NPC talking",			SOUND_TYPE_MONSTER_TALKING			},
	{ "NPC attacking",			SOUND_TYPE_MONSTER_ATTACKING		},
	{ "NPC eating",				SOUND_TYPE_MONSTER_EATING			},
	{ "Anomaly idle",			SOUND_TYPE_ANOMALY_IDLE				},
	{ "Object breaking",		SOUND_TYPE_WORLD_OBJECT_BREAKING	},
	{ "Object colliding",		SOUND_TYPE_WORLD_OBJECT_COLLIDING	},
	{ "Object exploding",		SOUND_TYPE_WORLD_OBJECT_EXPLODING	},
	{ "World ambient",			SOUND_TYPE_WORLD_AMBIENT			},
	{ 0,						0}
};
