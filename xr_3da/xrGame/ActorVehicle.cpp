#include "stdafx.h"
#pragma hdrstop

#include "actor.h"

void CActor::attach_Vehicle(CCar* vehicle)
{
	if(!vehicle) return;

	if(m_vehicle) return;

	m_vehicle=vehicle;


	
	if(!m_vehicle->attach_Actor(this)){
		m_vehicle=NULL;
		return;
	}
	// temp play animation
	CKinematics* V		= PKinematics(pVisual);
	R_ASSERT			(V);
	V->PlayCycle(m_anims.m_steering_legs_idle);
	V->PlayCycle(m_anims.m_steering_torso_idle);
	int spine_bone		= V->LL_BoneID("bip01_spine1");
	int shoulder_bone	= V->LL_BoneID("bip01_spine2");
	int head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetInstance(spine_bone).set_callback		(NULL,NULL);
	V->LL_GetInstance(shoulder_bone).set_callback	(NULL,NULL);
	V->LL_GetInstance(head_bone).set_callback		(NULL,NULL);
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
	CKinematics* V		= PKinematics(pVisual);
	R_ASSERT			(V);
	int spine_bone		= V->LL_BoneID("bip01_spine1");
	int shoulder_bone	= V->LL_BoneID("bip01_spine2");
	int head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetInstance(spine_bone).set_callback		(SpinCallback,this);
	V->LL_GetInstance(shoulder_bone).set_callback	(ShoulderCallback,this);
	V->LL_GetInstance(head_bone).set_callback		(HeadCallback,this);
}

void CActor::use_Vehicle()
{
	if(m_vehicle){
		detach_Vehicle();
	}else{
		if (pCamBobbing){Level().Cameras.RemoveEffector(cefBobbing); pCamBobbing=0;}
		attach_Vehicle(pick_VehicleObject());
	}
}

CCar* CActor::pick_VehicleObject()
{
	setEnabled(false);
	Collide::ray_query	l_rq;
	l_rq.O=NULL;
	pCreator->ObjectSpace.RayPick(Device.vCameraPosition, Device.vCameraDirection, 15.f, l_rq);
	setEnabled(true);
	CCar* ret=dynamic_cast<CCar*>(l_rq.O);

	if(!ret)	return NULL;
	if(!ret->is_Door(l_rq.element)) return NULL;

	return ret;
}
