////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_space.h
//	Created 	: 28.03.2003
//  Modified 	: 28.03.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker types and structures
////////////////////////////////////////////////////////////////////////////

#pragma once

#define	MAX_HEAD_TURN_ANGLE				(1.f*PI_DIV_4)

namespace StalkerSpace {
	enum EStalkerSounds {
		eStalkerSoundDie = u32(0),
		eStalkerSoundDieInAnomaly,
		eStalkerSoundInjuring,
		eStalkerSoundHumming,
		eStalkerSoundAlarm,
		eStalkerSoundAttack,
		eStalkerSoundBackup,
		eStalkerSoundDetour,
		eStalkerSoundSearch,
		eStalkerSoundInjuringByFriend,
		eStalkerSoundPanicHuman,
		eStalkerSoundPanicMonster,
		eStalkerSoundTolls,
		eStalkerSoundGrenadeAlarm,
		eStalkerSoundFriendlyGrenadeAlarm,

		eStalkerSoundScript,
		eStalkerSoundDummy = u32(-1),
	};

	enum EStalkerSoundMasks {
		eStalkerSoundMaskAnySound				= u32(0),
		eStalkerSoundMaskDie					= u32(1 << 31),
		eStalkerSoundMaskDieInAnomaly			= u32(1 << 31),
		eStalkerSoundMaskInjuring				= u32(1 << 31),
		eStalkerSoundMaskInjuringByFriend		= u32(1 << 31),
		eStalkerSoundMaskNonTriggered			= u32(1 << 31) | (1 << 30),
		eStalkerSoundMaskNoHumming				= (1 << 29),
		eStalkerSoundMaskFree					= eStalkerSoundMaskNoHumming | eStalkerSoundMaskNonTriggered,
		eStalkerSoundMaskHumming				= 1 | eStalkerSoundMaskFree,
		eStalkerSoundMaskNoDanger				= (1 << 28),
		eStalkerSoundMaskDanger					= eStalkerSoundMaskNoDanger | eStalkerSoundMaskNonTriggered,
		eStalkerSoundMaskAlarm					= (1 << 0) | eStalkerSoundMaskDanger,
		eStalkerSoundMaskAttack					= (1 << 1) | eStalkerSoundMaskDanger,
		eStalkerSoundMaskBackup					= (1 << 2) | eStalkerSoundMaskDanger,
		eStalkerSoundMaskDetour					= (1 << 3) | eStalkerSoundMaskDanger,
		eStalkerSoundMaskSearch					= (1 << 4) | eStalkerSoundMaskDanger,
		eStalkerSoundMaskPanicHuman				= eStalkerSoundMaskDanger,
		eStalkerSoundMaskPanicMonster			= eStalkerSoundMaskDanger,
		eStalkerSoundMaskTolls					= eStalkerSoundMaskDanger,
		eStalkerSoundMaskGrenadeAlarm			= eStalkerSoundMaskDanger,
		eStalkerSoundMaskFriendlyGrenadeAlarm	= eStalkerSoundMaskDanger,
		eStalkerSoundMaskDummy					= u32(-1),
	};

	enum EBodyAction {
		eBodyActionNone		= u32(0),
		eBodyActionHello,
		eBodyActionDummy	= u32(-1),
	};
};
