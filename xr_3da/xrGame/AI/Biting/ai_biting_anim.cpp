////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"

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
	"walk_rs_",
	"walk_back_",
	"run_",
	"run_attack_",
	"attack_",
	"eat_",
	"damage_",
	0
};