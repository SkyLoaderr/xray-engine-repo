////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_feel.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Visibility and look for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"

void CAI_Zombie::SetDirectionLook()
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
		}
	}
	else
		r_torso_target.pitch = 0;
	r_target = r_torso_target;
}

void CAI_Zombie::vfAimAtEnemy()
{
	Fvector	pos1, pos2;
	m_Enemy.Enemy->Center(pos1);
	Center(pos2);
	tWatchDirection.sub(pos1,pos2);
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
}

BOOL CAI_Zombie::feel_vision_isRelevant(CObject* O)
{
	if (O->CLS_ID!=CLSID_ENTITY)			
		return FALSE;
	else  {
		CEntityAlive* E = dynamic_cast<CEntityAlive*> (O);
		if (!E) return FALSE;
		if (!E->IsVisibleForAI())		return FALSE; 
		if (E->g_Team() == g_Team())	return FALSE;
		return TRUE;
	}
}

void CAI_Zombie::feel_sound_new(CObject* who, int eType, Fvector& Position, float power)
{
	if (!g_Alive())
		return;
	
	//Msg("%.3f %.3f %.3f %8X [%7.2f,%7.2f,%7.2f] %s",power,ffGetStartVolume(ESoundTypes(eType)),power*ffGetStartVolume(ESoundTypes(eType)),eType,Position.x,Position.y,Position.z,who ? who->cName() : "world");
	power *= ffGetStartVolume(ESoundTypes(eType));

	if (power >= m_fSoundThreshold) {
		if ((this != who) && ((m_tLastSound.dwTime <= m_dwLastUpdateTime) || (m_tLastSound.fPower <= power))) {
			m_tLastSound.eSoundType		= ESoundTypes(eType);
			m_tLastSound.dwTime			= Level().timeServer();
			m_tLastSound.fPower			= power;
			m_tLastSound.tSavedPosition = Position;
			m_tLastSound.tpEntity		= dynamic_cast<CEntity *>(who);
			//float fDistance = (Position.distance_to(Position()) < 1.f ? 1.f : Position.distance_to(Position()));
//			if ((eType & SOUND_TYPE_MONSTER_DYING) == SOUND_TYPE_MONSTER_DYING)
//				m_fMorale += m_fMoraleDeathQuant;///fDistance;
//			else
//				if (((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING) && (!m_Enemy.Enemy))
//					m_fMorale += m_fMoraleFearQuant;///fDistance;
//				else
//					if ((eType & SOUND_TYPE_MONSTER_ATTACKING) == SOUND_TYPE_MONSTER_ATTACKING)
//						m_fMorale += m_fMoraleSuccessAttackQuant;///fDistance;
		}
	}
}