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
		eStalkerSoundMaskDie					= u32(-1),
		eStalkerSoundMaskDieInAnomaly			= u32(-1),
		eStalkerSoundMaskInjuring				= u32(-1),
		eStalkerSoundMaskHumming				= u32(1 << 31) | 1,
		eStalkerSoundMaskNoHumming				= u32(1 << 31),
		eStalkerSoundMaskAlarm					= u32(1 << 30) | 1,
		eStalkerSoundMaskAttack					= u32(1 << 30) | 2,
		eStalkerSoundMaskBackup					= u32(1 << 30) | 4,
		eStalkerSoundMaskDetour					= u32(1 << 30) | 8,
		eStalkerSoundMaskSearch					= u32(1 << 30) | 16,
		eStalkerSoundMaskInjuringByFriend		= u32(-1),
		eStalkerSoundMaskPanicHuman				= u32(-1),
		eStalkerSoundMaskPanicMonster			= u32(-1),
		eStalkerSoundMaskTolls					= u32(-1),
		eStalkerSoundMaskGrenadeAlarm			= u32(-1),
		eStalkerSoundMaskFriendlyGrenadeAlarm	= u32(-1),
		eStalkerSoundMaskDummy					= u32(-1),
	};

	enum EBodyAction {
		eBodyActionNone		= u32(0),
		eBodyActionHello,
		eBodyActionDummy	= u32(-1),
	};
};
