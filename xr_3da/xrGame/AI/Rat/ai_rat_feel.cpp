////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_feel.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Visibility and look for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"

void CAI_Rat::SetDirectionLook()
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,m_body.target);
		}
	}
	else
		m_body.target.pitch = 0;
	m_head.target = m_body.target;
}

void CAI_Rat::vfAimAtEnemy()
{
	Fvector	pos1, pos2;
	//m_Enemy.m_enemy->Center(pos1);
	pos1 = m_Enemy.m_enemy->Position();
	Center(pos2);
	tWatchDirection.sub(pos1,pos2);
	mk_rotation(tWatchDirection,m_body.target);
	m_head.target.yaw = m_body.target.yaw;
}

BOOL CAI_Rat::feel_vision_isRelevant(CObject* O)
{
	if (CLSID_ENTITY!=O->CLS_ID)			
		return FALSE;
	else  {
		CEntityAlive* E = dynamic_cast<CEntityAlive*> (O);
		if (!E) return FALSE;
		if ((E->g_Team() == g_Team()) && (E->g_Alive())) return FALSE;
		return TRUE;
	}
}

void CAI_Rat::feel_sound_new(CObject* who, int eType, const Fvector &Position, float power)
{
	if (!g_Alive())
		return;

	//Msg("%.3f %.3f %.3f %8X [%7.2f,%7.2f,%7.2f] %s",power,ffGetStartVolume(ESoundTypes(eType)),power*ffGetStartVolume(ESoundTypes(eType)),eType,Position.x,Position.y,Position.z,who ? who->cName() : "world");
	//power *= ffGetStartVolume(ESoundTypes(eType));
	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));

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
				if (((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING) && (!m_Enemy.m_enemy))
					m_fMorale += m_fMoraleFearQuant;///fDistance;
				else
					if ((eType & SOUND_TYPE_MONSTER_ATTACKING) == SOUND_TYPE_MONSTER_ATTACKING)
						m_fMorale += m_fMoraleSuccessAttackQuant;///fDistance;
		}
	}
}