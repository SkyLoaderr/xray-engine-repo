////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_misc.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Miscellanious functions for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
// ¬ходные воздействи€ 
// «рение, слух, веро€тность победы, выгодность противника
void CAI_Biting::vfUpdateParameters()
{
	EnemyMemory.update			();
	EnemyMan.update				();	
	SoundMemory.UpdateHearing	();	

		
	hear_dangerous_sound = hear_interesting_sound = false;
	SoundElem se;
	
	if (SoundMemory.IsRememberSound()) {
		SoundMemory.GetSound(se,hear_dangerous_sound);
		hear_interesting_sound = !hear_dangerous_sound;
	}

	// Setup is own additional flags
	m_bDamaged = ((GetHealth() < _sd->m_fDamagedThreshold) ? true : false);
}


