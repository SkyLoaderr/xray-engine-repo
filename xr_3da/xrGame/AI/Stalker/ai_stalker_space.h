////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_space.h
//	Created 	: 28.03.2003
//  Modified 	: 28.03.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker types and structures
////////////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_DYNAMIC_OBJECTS 			32
#define MAX_DYNAMIC_SOUNDS  			32
#define MAX_HURT_COUNT					32
#define MAX_SEARCH_COUNT				32

#define MAX_TIME_RANGE_SEARCH			5000.f
#define	MAX_HEAD_TURN_ANGLE				(2.f*PI_DIV_6)

#define	STALKER_SND_STEP_COUNT			2

#define SOUND_UPDATE_DELAY				3000

#define R2D(x)							(angle_normalize(x)*180.f/PI)

DEFINE_VECTOR							(ref_sound,SOUND_VECTOR,SOUND_IT);

#define SILENCE

#ifndef SILENCE
	#define OUT_TEXT(S)						if (g_Alive()) Msg(S);
#else
	#define OUT_TEXT(S)						;
//	#define OUT_TEXT(S)						if (g_Alive()) Msg(S);
#endif

namespace StalkerSpace {
	enum EActionState {
		eActionStateRun = 0,
		eActionStateStand,
		eActionStateDontWatch,
		eActionStateWatch,
		eActionStateWatchGo,
		eActionStateWatchLook,
	};

	enum ESearchState {
		eSearchStateMarkVisibleNodes = 0,
		eSearchStateFindSuspiciousNodes,
	};
};
