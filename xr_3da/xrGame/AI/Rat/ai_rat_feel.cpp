////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_feel.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Visibility and look for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"

BOOL CAI_Rat::feel_vision_isRelevant(CObject* O)
{
	if (CLSID_ENTITY!=O->CLS_ID)			
		return FALSE;
	else  {
		CEntityAlive* E = smart_cast<CEntityAlive*> (O);
		if (!E) return FALSE;
		if ((E->g_Team() == g_Team()) && (E->g_Alive())) return FALSE;
		return TRUE;
	}
}

void CAI_Rat::feel_sound_new(CObject* who, int eType, const Fvector &Position, float power)
{
	if (!g_Alive())
		return;

	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;

	if (power >= m_fSoundThreshold) {
		if ((this != who) && ((m_tLastSound.dwTime <= m_dwLastUpdateTime) || (m_tLastSound.fPower <= power))) {
			m_tLastSound.eSoundType		= ESoundTypes(eType);
			m_tLastSound.dwTime			= Level().timeServer();
			m_tLastSound.fPower			= power;
			m_tLastSound.tSavedPosition = Position;
			m_tLastSound.tpEntity		= smart_cast<CEntityAlive*>(who);
			if ((eType & SOUND_TYPE_MONSTER_DYING) == SOUND_TYPE_MONSTER_DYING)
				m_fMorale += m_fMoraleDeathQuant;
			else
				if (((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING) && !enemy())
					m_fMorale += m_fMoraleFearQuant;///fDistance;
				else
					if ((eType & SOUND_TYPE_MONSTER_ATTACKING) == SOUND_TYPE_MONSTER_ATTACKING)
						m_fMorale += m_fMoraleSuccessAttackQuant;///fDistance;
		}
	}
}
