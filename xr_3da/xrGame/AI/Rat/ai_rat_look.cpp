////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 06.11.2002
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
			mk_rotation(tWatchDirection,r_torso_target);
		}
	}
	else
		r_torso_target.pitch = 0;
}

void CAI_Rat::vfAimAtEnemy()
{
	Fvector	pos1, pos2;
	m_Enemy.Enemy->svCenter(pos1);
	svCenter(pos2);
	tWatchDirection.sub(pos1,pos2);
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
}

static BOOL __fastcall RatQualifier(CObject* O, void* P)
{
	if (O->CLS_ID!=CLSID_ENTITY)			
		return FALSE;
	else  {
		CEntity* E = dynamic_cast<CEntity*> (O);
		if (!E) return FALSE;
		if (!E->IsVisibleForAI()) return FALSE; 
		if (E->g_Team() == *((int*)P)) return FALSE;
		return TRUE;
	}
}

objQualifier* CAI_Rat::GetQualifier	()
{
	return(&RatQualifier);
}

void CAI_Rat::feel_sound_new(CObject* who, int eType, Fvector& Position, float power)
{
	if (g_Health() <= 0)
		return;
	
	power *= ffGetStartVolume(ESoundTypes(eType));
	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));

	if (power >= 0.1) {
		if ((this != who) && ((m_tLastSound.dwTime <= m_dwLastUpdateTime) || (m_tLastSound.fPower <= power))) {
			CEntity *tpEntity = dynamic_cast<CEntity *>(who);
			if (tpEntity) {
				m_tLastSound.eSoundType		= ESoundTypes(eType);
				m_tLastSound.dwTime			= Level().timeServer();
				m_tLastSound.fPower			= power;
				m_tLastSound.tSavedPosition = Position;
				m_tLastSound.tpEntity		= tpEntity;
			}
		}
	}
}