////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "ai_rat_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\3dsound.h"
#include "..\\..\\..\\xr_trims.h"

void CAI_Rat::Die()
{
	WRITE_TO_LOG("Dying...");

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	//bActive = false;
	bEnabled = false;
}

void CAI_Rat::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		switch(eCurrentState) {
			case aiRatDie : {
				Die();
				break;
			}

		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
}
