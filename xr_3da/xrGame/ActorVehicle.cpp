#include "stdafx.h"
#pragma hdrstop

#include "actor.h"
#include "../CameraBase.h"

#include "ActorEffector.h"
#include "holder_custom.h"
#ifdef DEBUG
#include "PHDebug.h"
#endif
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "../skeletonanimated.h"
#include "PHShellSplitter.h"

#include "actor_anim_defs.h"
#include "game_object_space.h"


void CActor::attach_Vehicle(CHolderCustom* vehicle)
{
	if(!vehicle) return;

	if(m_holder) return;
	PickupModeOff		();
	m_holder=vehicle;

	CSkeletonAnimated* V		= smart_cast<CSkeletonAnimated*>(Visual()); R_ASSERT(V);
	
	if(!m_holder->attach_Actor(this)){
		m_holder=NULL;
		return;
	}
	// temp play animation
	CCar*	car			= smart_cast<CCar*>(m_holder);
	u16 anim_type       = car->DriverAnimationType();
	SActorVehicleAnims::SOneTypeCollection& anims=m_vehicle_anims->m_vehicles_type_collections[anim_type];
	V->PlayCycle(anims.idles[0],FALSE);

	ResetCallbacks		();
	u16 head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetBoneInstance(u16(head_bone)).set_callback		(bctPhysics, VehicleHeadCallback,this);

	m_PhysicMovementControl->DestroyCharacter();
	//PIItem iitem=inventory().ActiveItem();
	//setVisible(true);
	//clear actor movement states 
	mstate_wishful=0;
	m_holderID=car->ID();

//	HideCurrentWeapon(GEG_PLAYER_ENTER_CAR);
	SetWeaponHideState(whs_CAR, TRUE);

	CStepManager::on_animation_start(MotionID(), 0);
}

void CActor::detach_Vehicle()
{
	if(!m_holder) return;
	CCar* car=smart_cast<CCar*>(m_holder);
	if(!car)return;
	CPHShellSplitterHolder*sh= car->PPhysicsShell()->SplitterHolder();
	if(sh)sh->Deactivate();
	if(!m_PhysicMovementControl->ActivateBoxDynamic(0))
	{
		if(sh)sh->Activate();
		return;
	}
	if(sh)sh->Activate();
	m_holder->detach_Actor();//

	m_PhysicMovementControl->SetPosition(m_holder->ExitPosition());
	r_model_yaw=-m_holder->Camera()->yaw;
	r_torso.yaw=r_model_yaw;
	r_model_yaw_dest=r_model_yaw;
	m_holder=NULL;
	SetCallbacks		();
	CSkeletonAnimated* V= smart_cast<CSkeletonAnimated*>(Visual()); R_ASSERT(V);
	V->PlayCycle		(m_anims->m_normal.legs_idle);
	V->PlayCycle		(m_anims->m_normal.m_torso_idle);
	//mstate_wishful &=~mcAnyMove;
	m_holderID=u16(-1);

//	RestoreHidedWeapon(GEG_PLAYER_EXIT_CAR);
	SetWeaponHideState(whs_CAR, FALSE);
}

bool CActor::use_Vehicle(CHolderCustom* object)
{
	
//	CHolderCustom* vehicle=smart_cast<CHolderCustom*>(object);
	CHolderCustom* vehicle=object;
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
				if (pCamBobbing)
				{
					Cameras().RemoveCamEffector(eCEBobbing);
					pCamBobbing = NULL;
				}

				attach_Vehicle(vehicle);
			}
			return true;
		}
		return false;
	}
}

void CActor::on_reguested_spawn(CObject *object)
{
	CCar * car= smart_cast<CCar*>(object);
	attach_Vehicle(car);
}