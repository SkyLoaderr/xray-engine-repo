#include "stdafx.h"
#pragma hdrstop

#include "actor.h"
#include "Car.h"
#include "..\CameraBase.h"
void CActor::attach_Vehicle(CCar* vehicle)
{
	if(!vehicle) return;

	if(m_vehicle) return;

	m_vehicle=vehicle;


	CKinematics* V		= PKinematics(Visual());
	R_ASSERT			(V);
	
	if(!m_vehicle->attach_Actor(this)){
		m_vehicle=NULL;
		return;
	}
	// temp play animation
	V->PlayCycle(m_anims.m_steering_legs_idle);
	V->PlayCycle(m_anims.m_steering_torso_idle);
	u16 spine_bone		= V->LL_BoneID("bip01_spine1");
	u16 shoulder_bone	= V->LL_BoneID("bip01_spine2");
	u16 head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetInstance(u16(spine_bone)).set_callback		(NULL,NULL);
	V->LL_GetInstance(u16(shoulder_bone)).set_callback	(NULL,NULL);
	V->LL_GetInstance(u16(head_bone)).set_callback		(CarHeadCallback,this);

	Movement.DestroyCharacter();
	//PIItem iitem=m_inventory.ActiveItem();
	//if(iitem)iitem->m_showHUD=false;
	setVisible(true);
	//clear actor movement states 
	mstate_wishful=0;
}

void CActor::detach_Vehicle()
{
	if(!m_vehicle) return;
	m_vehicle->detach_Actor();//
	Movement.CreateCharacter();
	Movement.SetPosition(m_vehicle->ExitPosition());
	r_model_yaw=-m_vehicle->Camera()->yaw;
	r_torso.yaw=r_model_yaw;
	r_model_yaw_dest=r_model_yaw;
	m_vehicle=NULL;
	CKinematics* V		= PKinematics(Visual());
	R_ASSERT			(V);
	u16 spine_bone		= V->LL_BoneID("bip01_spine1");
	u16 shoulder_bone	= V->LL_BoneID("bip01_spine2");
	u16 head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetInstance(u16(spine_bone)).set_callback		(SpinCallback,this);
	V->LL_GetInstance(u16(shoulder_bone)).set_callback	(ShoulderCallback,this);
	V->LL_GetInstance(u16(head_bone)).set_callback		(HeadCallback,this);
	PKinematics(Visual())->PlayCycle(m_anims.m_normal.legs_idle);
	PKinematics(Visual())->PlayCycle(m_anims.m_normal.m_torso_idle);
	//mstate_wishful &=~mcAnyMove;
}

bool CActor::use_Vehicle(CGameObject* object)
{
	
	CCar* vehicle=dynamic_cast<CCar*>(object);
	Fvector center;
	Center(center);
	if(m_vehicle){
		if(!vehicle&& m_vehicle->Use(Device.vCameraPosition, Device.vCameraDirection,center)) detach_Vehicle();
		elsed
		{
			if(m_vehicle==vehicle)
				if(m_vehicle->Use(Device.vCameraPosition, Device.vCameraDirection,center))detach_Vehicle();
		}
		return true;
	}else{
		if(vehicle)
		{
			if( vehicle->Use(Device.vCameraPosition, Device.vCameraDirection,center))
			{
				if (pCamBobbing){Level().Cameras.RemoveEffector(cefBobbing); pCamBobbing=0;}

				attach_Vehicle(vehicle);
			}
			return true;
		}
		return false;
	}
}
CGameObject* CActor::pick_Object(u16& element)
{

	setEnabled(false);
	Collide::ray_query	l_rq;
	l_rq.O=NULL;
	g_pGameLevel->ObjectSpace.RayPick(Device.vCameraPosition, Device.vCameraDirection, 15.f, l_rq);
	setEnabled(true);
	element=u16(l_rq.element);
	return dynamic_cast<CGameObject*>(l_rq.O);

}
