////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"

namespace AI_Biting {
	LPCSTR caStateNames			[] = {
		"stand_",
		"sit_",
		"lie_",
		0
	};

	LPCSTR caGlobalNames		[] = {
		"idle_",
		"walk_fwd_",
		"walk_ls_",
		"run_",
		"attack_",
		"eat_",
		"damage_",
		0
	};
};
