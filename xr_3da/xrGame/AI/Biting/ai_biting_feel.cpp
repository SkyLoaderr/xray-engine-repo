////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_feel.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Visibility and look for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../../actor.h"


void CAI_Biting::feel_sound_new(CObject* who, int eType, const Fvector &Position, float power)
{
	if (!g_Alive())
		return;

#ifdef IGNORE_ACTOR
	if ((Level().CurrentEntity() == who) || (who && (who->H_Parent() == Level().CurrentEntity())))
		return;
#endif
	
	// ignore sounds from team
	CEntityAlive* E = dynamic_cast<CEntityAlive*> (who);
	if (E && (E->g_Team() == g_Team())) return;
	

	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));
	//else power = expf(.1f*log(power));

	if (power >= _sd->m_fSoundThreshold) {
		if (this != who) {
			HearSound(who,eType,Position,power,m_current_update);
			if (who)
				LOG_EX2("%s Hear: %s type = %u", *"*/ cName(), who->cName(), eType /*"*);
			else 
				LOG_EX2("%s Hear: WORLD type = %u", *"*/ cName(), eType /*"*);
		}
 	}
}

void CAI_Biting::HitEntity(const CEntity *pEntity, float fDamage, Fvector &dir)
{
	if (!g_Alive()) return;
	if (!pEntity) return;

	VisionElem ve;
	if (!GetEnemy(ve)) return;

	if ((CLSID_ENTITY == ve.obj->CLS_ID) && (ve.obj == pEntity)) {
		Fvector position_in_bone_space;
		position_in_bone_space.set(0.f,0.f,0.f);

//		Fvector	dir;
//		dir.set(XFORM().k);
//		if (!fsimilar(yaw,0.f) || !fsimilar(pitch,0.f)) {
//			float y,p;
//			dir.getHP(y,p);
//			y = angle_normalize(y + yaw);
//			p += pitch;
//			dir.setHP(y,p);
//		}

		float		impulse = ::Random.randF(_sd->m_fImpulseMin,_sd->m_fImpulseMax);
		CEntity		*pEntityNC	= const_cast<CEntity*>(pEntity);
		VERIFY		(pEntityNC);
		pEntityNC->Hit(fDamage,dir,this,0,position_in_bone_space,impulse);
	}
}


BOOL  CAI_Biting::feel_vision_isRelevant(CObject* O)
{
	if (CLSID_ENTITY!=O->CLS_ID)	
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

	AddDangerousEnemy(who,50000);
}

bool CAI_Biting::RayPickEnemy(const CObject *target_obj, const Fvector &trace_from, const Fvector &dir, float dist, float radius, u32 num_picks)
{
	bool ret_val = false;
	
	this->setEnabled(false);
	Collide::rq_result	l_rq;
	
	if (Level().ObjectSpace.RayPick(trace_from, dir, dist, Collide::rqtDynamic, l_rq)) {
		if ((l_rq.O == target_obj) && (l_rq.range < dist)) ret_val = true;
	} else {
		// макс. угол отклонения
		float max_alpha = atanf(radius/dist);
		
		float src_h, src_p;	
		dir.getHP(src_h,src_p);

		Fvector new_dir;
		float new_h,new_p;

		for (u32 i=0;i<num_picks;i++) {

			new_h = src_h + ::Random.randF(-max_alpha, max_alpha);
			new_p = src_p + ::Random.randF(-max_alpha, max_alpha);

			new_dir = dir;
			new_dir.setHP(new_h,new_p);
			new_dir.normalize();

			if (Level().ObjectSpace.RayPick(trace_from, new_dir, dist, Collide::rqtDynamic, l_rq)) {
				if ((l_rq.O == target_obj) && (l_rq.range < dist)) {
					ret_val = true;
					break;
				}
			}
		}
	}

	this->setEnabled(true);	
	return ret_val;
}

