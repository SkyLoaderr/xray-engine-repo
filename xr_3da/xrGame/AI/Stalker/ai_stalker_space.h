////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_space.h
//	Created 	: 28.03.2003
//  Modified 	: 28.03.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker types and structures
////////////////////////////////////////////////////////////////////////////

#pragma once

#define	MAX_HEAD_TURN_ANGLE				(1.f*PI_DIV_4)

DEFINE_VECTOR							(ref_sound,SOUND_VECTOR,SOUND_IT);

namespace StalkerSpace {
	enum EStalkerSounds {
		eStalkerSoundDie = u32(0),
		eStalkerSoundInjuring,
		eStalkerSoundHumming,
		eStalkerSoundAlarm,
		eStalkerSoundSurrender,
		eStalkerSoundBackup,
		eStalkerSoundAttack,
		eStalkerSoundInjuringByFriend,

		eStalkerSoundScript,
		eStalkerSoundDummy = u32(-1),
	};

	enum EStalkerSoundMasks {
		eStalkerSoundMaskAnySound			= u32(0),
		eStalkerSoundMaskDie				= u32(-1),
		eStalkerSoundMaskInjuring			= u32(-1),
		eStalkerSoundMaskHumming			= u32(1 << 31) | 1,
		eStalkerSoundMaskNoHumming			= u32(1 << 31),
		eStalkerSoundMaskAlarm				= u32(1 << 30) | 1,
		eStalkerSoundMaskSurrender			= u32(1 << 30) | 2,
		eStalkerSoundMaskBackup				= u32(1 << 30) | 4,
		eStalkerSoundMaskAttack				= u32(1 << 30) | 8,
		eStalkerSoundMaskInjuringByFriend	= u32(-1),
		eStalkerSoundMaskDummy				= u32(-1),
	};
};
