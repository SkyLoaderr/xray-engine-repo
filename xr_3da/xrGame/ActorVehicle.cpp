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
		attach_Vehicle(dynamic_cast<CCar*>(pick_Object()));
	}
}

