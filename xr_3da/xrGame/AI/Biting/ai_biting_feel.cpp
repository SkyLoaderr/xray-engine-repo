////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_feel.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Visibility and look for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\..\\actor.h"


void CAI_Biting::feel_sound_new(CObject* who, int eType, const Fvector &Position, float power)
{
	if (!g_Alive())
		return;

#ifdef IGNORE_ACTOR
	if ((who == Level().CurrentEntity()) || (who && (who->H_Parent() == Level().CurrentEntity())))
		return;
#endif

	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));

	if (power >= m_fSoundThreshold) {
		if (this != who) {
			HearSound(who,eType,Position,power,m_dwCurrentUpdate);
		}
 	}
}

void CAI_Biting::DoDamage(CEntity *pEntity, float fDamage, float yaw, float pitch, float impulse)
{
	if (!g_Alive()) return;
	if (!pEntity) return;

	VisionElem ve;
	if (!GetEnemy(ve)) return;

	if ((ve.obj->CLS_ID == CLSID_ENTITY) && (ve.obj == pEntity)) {
		Fvector position_in_bone_space;
		position_in_bone_space.set(0.f,0.f,0.f);

		Fvector	dir;
		dir.set(XFORM().k);
		if (!fsimilar(yaw,0.f) || !fsimilar(pitch,0.f)) {
			float y,p;
			dir.getHP(y,p);
			y = angle_normalize(y + yaw);
			p += pitch;
			dir.setHP(y,p);
		}
		pEntity->Hit(fDamage,dir,this,0,position_in_bone_space,impulse);
	}
}


BOOL  CAI_Biting::feel_vision_isRelevant(CObject* O)
{
	if (O->CLS_ID!=CLSID_ENTITY)	
		return FALSE;
	else  {
		CEntityAlive* E = dynamic_cast<CEntityAlive*> (O);
		if (!E) return FALSE;
		if (E->g_Team() == g_Team() && E->g_Alive()) return FALSE;
		return TRUE;
	}
}

void CAI_Biting::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	
	// Save event
	Fvector D;
	XFORM().transform_dir(D,vLocalDir);
	
	feel_sound_new(who,SOUND_TYPE_WEAPON_SHOOTING,who->Position(),1.f);
}

