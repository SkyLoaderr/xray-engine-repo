////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_sounds.h
//	Created 	: 15.08.2002
//  Modified 	: 15.08.2002
//	Author		: Dmitriy Iassenev
//	Description : Sounds for AI personalities
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_SOUNDS__
#define __XRAY_AI_SOUNDS__

#define DECLARE_WEAPON_SOUNDS(A,B)\
	SOUND_TYPE_WEAPON_##A = B,\
	SOUND_TYPE_WEAPON_RECHARGING_##A		= SOUND_TYPE_WEAPON_RECHARGING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_SHOOTING_##A			= SOUND_TYPE_WEAPON_SHOOTING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_TAKING_##A			= SOUND_TYPE_WEAPON_TAKING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_HIDING_##A			= SOUND_TYPE_WEAPON_HIDING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_EMPTY_CLICKING_##A	= SOUND_TYPE_WEAPON_EMPTY_CLICKING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_BULLET_RICOCHET_##A	= SOUND_TYPE_WEAPON_BULLET_RICOCHET | SOUND_TYPE_WEAPON_##A,

#define DECLARE_MONSTER_SOUNDS(A,B)\
	SOUND_TYPE_MONSTER_##A					= B,\
	SOUND_TYPE_MONSTER_DYING_##A			= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_DYING,\
	SOUND_TYPE_MONSTER_INJURING_##A			= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_INJURING,\
	SOUND_TYPE_MONSTER_WALKING_##A			= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_WALKING,\
	SOUND_TYPE_MONSTER_JUMPING_##A			= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_JUMPING,\
	SOUND_TYPE_MONSTER_FALLING_##A			= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_FALLING,\
	SOUND_TYPE_MONSTER_TALKING_##A			= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_TALKING,\
	SOUND_TYPE_MONSTER_ATTACKING_##A		= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_ATTACKING,


enum ESoundTypes {
	SOUND_TYPE_NO_SOUND					= 0x00000000,
	SOUND_TYPE_WEAPON					= u32(0x80000000),
	SOUND_TYPE_MONSTER					= 0x40000000,
	SOUND_TYPE_WORLD					= 0x20000000,
	
	SOUND_TYPE_WEAPON_RECHARGING		= SOUND_TYPE_WEAPON | 0x10000000,
	SOUND_TYPE_WEAPON_SHOOTING			= SOUND_TYPE_WEAPON | 0x08000000,
	SOUND_TYPE_WEAPON_TAKING			= SOUND_TYPE_WEAPON | 0x04000000,
	SOUND_TYPE_WEAPON_HIDING			= SOUND_TYPE_WEAPON | 0x02000000,
	SOUND_TYPE_WEAPON_EMPTY_CLICKING	= SOUND_TYPE_WEAPON | 0x01000000,
	SOUND_TYPE_WEAPON_BULLET_RICOCHET	= SOUND_TYPE_WEAPON | 0x00800000,

	SOUND_TYPE_MONSTER_DYING			= SOUND_TYPE_MONSTER | 0x10000000,
	SOUND_TYPE_MONSTER_INJURING			= SOUND_TYPE_MONSTER | 0x08000000,
	SOUND_TYPE_MONSTER_WALKING			= SOUND_TYPE_MONSTER | 0x04000000,
	SOUND_TYPE_MONSTER_JUMPING			= SOUND_TYPE_MONSTER | 0x00100000,
	SOUND_TYPE_MONSTER_FALLING			= SOUND_TYPE_MONSTER | 0x00040000,
	SOUND_TYPE_MONSTER_TALKING			= SOUND_TYPE_MONSTER | 0x00020000,
	SOUND_TYPE_MONSTER_ATTACKING		= SOUND_TYPE_MONSTER | 0x00010000,
	
	SOUND_TYPE_WORLD_OBJECT_BREAKING	= SOUND_TYPE_WORLD | 0x10000000,
	SOUND_TYPE_WORLD_OBJECT_FALLING		= SOUND_TYPE_WORLD | 0x08000000,

	DECLARE_WEAPON_SOUNDS(PISTOL			,0x00000001)
	DECLARE_WEAPON_SOUNDS(GUN				,0x00000002)
	DECLARE_WEAPON_SOUNDS(SUBMACHINEGUN		,0x00000004)
	DECLARE_WEAPON_SOUNDS(MACHINEGUN		,0x00000008)
	DECLARE_WEAPON_SOUNDS(SNIPERRIFLE		,0x00000010)
	DECLARE_WEAPON_SOUNDS(GRENADELAUNCHER	,0x00000020)
	DECLARE_WEAPON_SOUNDS(ROCKETLAUNCHER	,0x00000040)
	
	DECLARE_MONSTER_SOUNDS(HUMAN			,0x00000001)
	DECLARE_MONSTER_SOUNDS(ANIMAL			,0x00000002)
//				grenade
//	SOUND_TYPE_WEAPON_GRENADE_CHANGE_WEAPON,
//	SOUND_TYPE_WEAPON_GRENADE_SHOT,
//	SOUND_TYPE_WORLD_GRENADE_FALLING,
};

#define CROUCH_SOUND_FACTOR					.3f
#define ACCELERATED_SOUND_FACTOR			.5f

xr_token anomaly_type_token[]={
	{ "undefined",				st_Undefined						},
	{ "Weapon Recharging",		SOUND_TYPE_WEAPON_RECHARGING		},
	{ "Weapon Shooting",		SOUND_TYPE_WEAPON_SHOOTING			},
	{ "Weapon Taking",			SOUND_TYPE_WEAPON_TAKING			},
	{ "Weapon Hiding",			SOUND_TYPE_WEAPON_HIDING			},
	{ "Weapon Empty Clicking",	SOUND_TYPE_WEAPON_EMPTY_CLICKING	},
	{ "Weapon Bullet Ricochet",	SOUND_TYPE_WEAPON_BULLET_RICOCHET	},
	{ "Human Dying",			SOUND_TYPE_MONSTER_DYING_HUMAN		},
	{ "Human Injuring",			SOUND_TYPE_MONSTER_INJURING_HUMAN	},
	{ "Human Walking",			SOUND_TYPE_MONSTER_WALKING_HUMAN	},
	{ "Human Jumping",			SOUND_TYPE_MONSTER_JUMPING_HUMAN	},
	{ "Human Falling",			SOUND_TYPE_MONSTER_FALLING_HUMAN	},
	{ "Human Talking",			SOUND_TYPE_MONSTER_TALKING_HUMAN	},
	{ "Human Attacking",		SOUND_TYPE_MONSTER_ATTACKING_HUMAN	},
	{ "Monster Dying",			SOUND_TYPE_MONSTER_DYING_ANIMAL		},
	{ "Monster Injuring",		SOUND_TYPE_MONSTER_INJURING_ANIMAL	},
	{ "Monster Walking",		SOUND_TYPE_MONSTER_WALKING_ANIMAL	},
	{ "Monster Jumping",		SOUND_TYPE_MONSTER_JUMPING_ANIMAL	},
	{ "Monster Falling",		SOUND_TYPE_MONSTER_FALLING_ANIMAL	},
	{ "Monster Talking",		SOUND_TYPE_MONSTER_TALKING_ANIMAL	},
	{ "Monster Attacking",		SOUND_TYPE_MONSTER_ATTACKING_ANIMAL	},
	{ "World Object Breaking",	SOUND_TYPE_WORLD_OBJECT_BREAKING	},
	{ "World Object Falling",	SOUND_TYPE_WORLD_OBJECT_FALLING		},
	{ 0,						0}
};

extern float ffGetStartVolume(ESoundTypes eType);

#endif
