#include "stdafx.h"
#pragma hdrstop

#include "actor.h"
#include "WeaponMounted.h"
#include "../CameraBase.h"
#include "ActorEffector.h"

bool CActor::use_MountedWeapon(CGameObject* object)
{
	CWeaponMounted* wpn	=dynamic_cast<CWeaponMounted*>(object);
	if(m_holder){
		if(!wpn||(m_holder==wpn)){
			m_holder->detach_Actor();
			m_holder=NULL;
		}
		return true;
	}else{
		if(wpn){
			Fvector center;	Center(center);
			if(wpn->Use(Device.vCameraPosition, Device.vCameraDirection,center)){
				if(wpn->attach_Actor(this)){
					m_holder=wpn;
					if (pCamBobbing){
						EffectorManager().RemoveEffector(eCEBobbing);
						pCamBobbing = NULL;
					}
					return true;
				}
			}
		}
	}
	return false;
}