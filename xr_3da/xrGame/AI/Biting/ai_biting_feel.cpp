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
	
	// ignore sounds from team
	CEntityAlive* E = dynamic_cast<CEntityAlive*> (who);
	if (E && (E->g_Team() == g_Team())) return;
	

	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));

	if (power >= _sd->m_fSoundThreshold) {
		if (this != who) {
			HearSound(who,eType,Position,power,m_dwCurrentUpdate);
		}
 	}
}

void CAI_Biting::DoDamage(CEntity *pEntity, float fDamage, float yaw, float pitch)
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
		float impulse = ::Random.randF(_sd->m_fImpulseMin,_sd->m_fImpulseMax);
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
	feel_sound_new(who,SOUND_TYPE_WEAPON_SHOOTING,who->Position(),1.f);
	if (g_Alive()) SetSoundOnce(SND_TYPE_TAKE_DAMAGE, m_dwCurrentTime);

	if (element < 0) return;

	// Определить направление хита (спереди || сзади)
	float yaw,pitch;
	vLocalDir.getHP(yaw,pitch);
	
	yaw = angle_normalize(yaw);
	bool is_front = ( ((PI_DIV_2 <= yaw) && (yaw <= 3*PI_DIV_2))? false: true );

	MotionMan.FX_Play(u16(element), is_front, amount / 4);	
}

