#include "stdafx.h"
#include "ai_stalker.h"

//void CAI_Stalker::g_fireParams(Fvector &fire_pos, Fvector &fire_dir)
//{
//	if (Weapons->ActiveWeapon()) {
//		fire_pos.set(Weapons->ActiveWeapon()->Position());
//		/**
//		if ((tSavedEnemy) && (tSavedEnemy->g_Health() > 0)) {
//			Fvector fTemp;
//			fTemp.sub(tSavedEnemy->Position(),fire_pos);
//			fTemp.normalize_safe();
//			fire_dir.set(fTemp);
//		}
//		else
//			fire_dir.set(eye_matrix.k);
//		/**/
//			//fire_dir.set(eye_matrix.k);
//			//fire_dir.set(tWatchDirection);
//		/**
//		if (r_torso_current.yaw > PI)
//			fire_dir.setHP(-r_torso_current.yaw + m_fAddWeaponAngle,-r_torso_current.pitch);
//		else
//			fire_dir.setHP(-r_torso_current.yaw,-r_torso_current.pitch);
//		/**/
//		fire_dir.setHP(-r_torso_current.yaw,-r_torso_current.pitch);
//	}
//}
//
