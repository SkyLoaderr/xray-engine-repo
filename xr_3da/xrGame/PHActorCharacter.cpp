#include "stdafx.h"
#include "phactorcharacter.h"
#include "Extendedgeom.h"
#include "PhysicsCommon.h"


//const float JUMP_HIGHT=0.5;
const float JUMP_UP_VELOCITY=6.0f;//5.6f;
const float JUMP_INCREASE_VELOCITY_RATE=1.2f;

CPHActorCharacter::CPHActorCharacter(void)
{
	m_cap=NULL;
	m_cap_transform=NULL;
}

CPHActorCharacter::~CPHActorCharacter(void)
{
}

void CPHActorCharacter::Create(dVector3 sizes)
{
	if(b_exist) return;
	inherited::Create(sizes);


}

void CPHActorCharacter::Destroy()
{
	if(!b_exist) return;

	inherited::Destroy();
}

void CPHActorCharacter::SetPhysicsRefObject(CPhysicsShellHolder* ref_object)
{
	inherited::SetPhysicsRefObject(ref_object);

	if(b_exist)
	{

		dGeomUserDataSetPhysicsRefObject(m_cap,ref_object);

	}
}





void CPHActorCharacter::SetAcceleration(Fvector accel)
{
	inherited::SetAcceleration(accel);
}

void CPHActorCharacter::Jump(const Fvector& accel)
{
	if(!b_exist) return;
	if(!b_lose_control && (m_ground_contact_normal[1]>0.5f||m_elevator_state.ClimbingState()))
	{
		b_jump=true;
		const dReal* vel=dBodyGetLinearVel(m_body);
		dReal amag =m_acceleration.magnitude();
		if(amag<1.f)amag=1.f;
		m_jump_accel.set(vel[0]*JUMP_INCREASE_VELOCITY_RATE+m_acceleration.x/amag*0.2f,jump_up_velocity,vel[2]*JUMP_INCREASE_VELOCITY_RATE +m_acceleration.z/amag*0.2f);
		Enable();
	}
}
void CPHActorCharacter::SetObjectContactCallback(ObjectContactCallbackFun* callback)
{
	inherited::SetObjectContactCallback(callback);
///	if(!b_exist) return;
//	dGeomUserDataSetObjectContactCallback(m_cap,callback);
}

void CPHActorCharacter::Disable()
{
	inherited::Disable();
}