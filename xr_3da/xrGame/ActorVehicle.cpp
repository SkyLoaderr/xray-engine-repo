#include "stdafx.h"
#pragma hdrstop

#include "actor.h"

void CActor::attach_Vehicle(CCar* vehicle)
{
	if(m_vehicle) return;
	m_vehicle=dynamic_cast<CCar*>(vehicle);
	if(!m_vehicle) return;
	if(!m_vehicle->attach_Actor(this)){
		m_vehicle=NULL;
		return;
	}
	// temp play animation
	PKinematics	(pVisual)->PlayCycle(m_anims.m_steering_legs_idle);
	PKinematics	(pVisual)->PlayCycle(m_anims.m_steering_torso_idle);

	ph_Movement.DestroyCharacter();
	PIItem iitem=m_inventory.ActiveItem();
	if(iitem)iitem->m_showHUD=false;
	setVisible(true);
}

void CActor::detach_Vehicle()
{
	if(!m_vehicle) return;
	//m_vehicle->detach_Actor();//calling by detach_Actor()
	ph_Movement.CreateCharacter();
	ph_Movement.SetPosition(vPosition);
	m_vehicle=NULL;
}

void CActor::use_Vehicle()
{
	if(m_vehicle){
		detach_Vehicle();
	}else{
		if (pCamBobbing){Level().Cameras.RemoveEffector(cefBobbing); pCamBobbing=0;}
		attach_Vehicle(dynamic_cast<CCar*>(pick_Object()));
	}
}

