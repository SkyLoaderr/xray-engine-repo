#include "stdafx.h"
#pragma hdrstop

#include "actor.h"
#include "Car.h"
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
	CKinematics* V		= PKinematics(Visual());
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
	//PIItem iitem=m_inventory.ActiveItem();
	//if(iitem)iitem->m_showHUD=false;
	setVisible(true);
}

void CActor::detach_Vehicle()
{
	if(!m_vehicle) return;
	m_vehicle->detach_Actor();//
	ph_Movement.CreateCharacter();
	ph_Movement.SetPosition(m_vehicle->ExitPosition());
	m_vehicle=NULL;
	CKinematics* V		= PKinematics(Visual());
	R_ASSERT			(V);
	int spine_bone		= V->LL_BoneID("bip01_spine1");
	int shoulder_bone	= V->LL_BoneID("bip01_spine2");
	int head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetInstance(spine_bone).set_callback		(SpinCallback,this);
	V->LL_GetInstance(shoulder_bone).set_callback	(ShoulderCallback,this);
	V->LL_GetInstance(head_bone).set_callback		(HeadCallback,this);
	PKinematics(Visual())->PlayCycle(m_anims.m_normal.legs_idle);
	PKinematics(Visual())->PlayCycle(m_anims.m_normal.m_torso_idle);
}

void CActor::use_Vehicle()
{
	int element=-1;
	CCar* vehicle=pick_VehicleObject(element);
	if(m_vehicle){
		if(!vehicle&& m_vehicle->Use(element,Device.vCameraPosition, Device.vCameraDirection)) detach_Vehicle();
		else
		{
		 if(m_vehicle==vehicle)
			 if(m_vehicle->Use(element,Device.vCameraPosition, Device.vCameraDirection))detach_Vehicle();
		}
	}else{
		if(vehicle && vehicle->Use(element,Device.vCameraPosition, Device.vCameraDirection))
		{
			if (pCamBobbing){Level().Cameras.RemoveEffector(cefBobbing); pCamBobbing=0;}

			attach_Vehicle(vehicle);
		}
	}
}

CCar* CActor::pick_VehicleObject(int& element)
{
	setEnabled(false);
	Collide::ray_query	l_rq;
	l_rq.O=NULL;
	g_pGameLevel->ObjectSpace.RayPick(Device.vCameraPosition, Device.vCameraDirection, 15.f, l_rq);
	
	if (l_rq.O)
	{
		ICollisionForm::RayPickResult result;
		if (l_rq.O->collidable.model->_RayPick	(result,Device.vCameraPosition, Device.vCameraDirection, 3.f, 0)) // CDB::OPT_ONLYFIRST CDB::OPT_ONLYNEAREST
		{
			int y=result.r_count();
			for (int k=0; k<y; k++){
				ICollisionForm::RayPickResult::Result* R = result.r_begin()+k;
				R->element;
				R->range;
				R=0;
			}
		}
	}
	
	setEnabled(true);
	element=l_rq.element;
	return dynamic_cast<CCar*>(l_rq.O);

}
