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

void CAI_Biting::ParseParam(LPCSTR param, u32 i1, u32 i2) 
{
	CKinematics* tpVisualObject = PKinematics(pVisual);

	TTest t;
	for (int j=0, N = _GetItemCount(param); j<N; j++) {
		string128				s;
		LPSTR					S = (LPSTR)&s;
		_GetItem				(param,j,S);
		
		
		strcpy (t.name,S);
		t.pM = tpVisualObject->ID_Cycle(S);
		AniVec[i1][i2].push_back(t);
	}
	
}


void CAI_Biting::LoadAnimations(LPCSTR section)
{
	
	if (pSettings->line_exist(section,"Ani_StandIdle")) {
		ParseParam(pSettings->r_string(section,"Ani_StandIdle"),ANIM_STAND,ANIM_IDLE);
	}

	if (pSettings->line_exist(section,"Ani_StandWalk")) {
		ParseParam(pSettings->r_string(section,"Ani_StandIdle"),ANIM_STAND,ANIM_WALK_FORWARD);
	}

}

void CAI_Biting::ShowMe()
{

	TTest t;
	for (int i=ANIM_STAND; i<=ANIM_SIT;i++) {
		for (int j=ANIM_IDLE;j<=ANIM_DAMAGED; j++) {
			
			
			for (int d = 0; d< AniVec[i][j].size(); d++) {
				t = AniVec[i][j][d];

				if (t.pM) {
					Log("PARAMS :",t.name);
				} else {
					Log("PARAMS BAD!!!");
				}
			}

		}
	}
}




