#include "stdafx.h"
#pragma hdrstop

#include "actor.h"
#include "../CameraBase.h"

#include "ActorEffector.h"
#include "holder_custom.h"

void CActor::attach_Vehicle(CHolderCustom* vehicle)
{
	if(!vehicle) return;

	if(m_holder) return;

	m_holder=vehicle;

	CSkeletonAnimated* V		= PSkeletonAnimated(Visual()); R_ASSERT(V);
	
	if(!m_holder->attach_Actor(this)){
		m_holder=NULL;
		return;
	}
	// temp play animation
	V->PlayCycle(m_anims.m_steering_legs_idle);
	V->PlayCycle(m_anims.m_steering_torso_idle);
	u16 spine_bone		= V->LL_BoneID("bip01_spine1");
	u16 shoulder_bone	= V->LL_BoneID("bip01_spine2");
	u16 head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetBoneInstance(u16(spine_bone)).set_callback		(NULL,NULL);
	V->LL_GetBoneInstance(u16(shoulder_bone)).set_callback	(NULL,NULL);
	V->LL_GetBoneInstance(u16(head_bone)).set_callback		(VehicleHeadCallback,this);

	m_PhysicMovementControl->DestroyCharacter();
	//PIItem iitem=inventory().ActiveItem();
	setVisible(true);
	//clear actor movement states 
	mstate_wishful=0;
}

void CActor::detach_Vehicle()
{
	if(!m_holder) return;
	m_holder->detach_Actor();//
	m_PhysicMovementControl->CreateCharacter();
	m_PhysicMovementControl->SetPosition(m_holder->ExitPosition());
	r_model_yaw=-m_holder->Camera()->yaw;
	r_torso.yaw=r_model_yaw;
	r_model_yaw_dest=r_model_yaw;
	m_holder=NULL;
	CKinematics* V		= PKinematics(Visual());
	R_ASSERT			(V);
	u16 spine_bone		= V->LL_BoneID("bip01_spine1");
	u16 shoulder_bone	= V->LL_BoneID("bip01_spine2");
	u16 head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetBoneInstance(u16(spine_bone)).set_callback		(SpinCallback,this);
	V->LL_GetBoneInstance(u16(shoulder_bone)).set_callback	(ShoulderCallback,this);
	V->LL_GetBoneInstance(u16(head_bone)).set_callback		(HeadCallback,this);
	PSkeletonAnimated(Visual())->PlayCycle(m_anims.m_normal.legs_idle);
	PSkeletonAnimated(Visual())->PlayCycle(m_anims.m_normal.m_torso_idle);
	//mstate_wishful &=~mcAnyMove;
}

bool CActor::use_Vehicle(CPhysicsShellHolder* object)
{
	
	CHolderCustom* vehicle=smart_cast<CHolderCustom*>(object);
	Fvector center;
	Center(center);
	if(m_holder){
		if(!vehicle&& m_holder->Use(Device.vCameraPosition, Device.vCameraDirection,center)) detach_Vehicle();
		else{ 
			if(m_holder==vehicle)
				if(m_holder->Use(Device.vCameraPosition, Device.vCameraDirection,center))detach_Vehicle();
		}
		return true;
	}else{
		if(vehicle)
		{
			if( vehicle->Use(Device.vCameraPosition, Device.vCameraDirection,center))
			{
//				if (pCamBobbing){Level().Cameras.RemoveEffector(cefBobbing); pCamBobbing=0;}
				if (pCamBobbing)
				{
					EffectorManager().RemoveEffector(eCEBobbing);
					pCamBobbing = NULL;
				}

				attach_Vehicle(vehicle);
			}
			return true;
		}
		return false;
	}
}