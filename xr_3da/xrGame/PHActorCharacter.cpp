#include "stdafx.h"
#include "phactorcharacter.h"
#include "Extendedgeom.h"
#include "PhysicsCommon.h"
#include "GameObject.h"
#include "PhysicsShellHolder.h"
#include "ai/stalker/ai_stalker.h"
#include "Actor.h"
//const float JUMP_HIGHT=0.5;
const float JUMP_UP_VELOCITY=6.0f;//5.6f;
const float JUMP_INCREASE_VELOCITY_RATE=1.2f;

CPHActorCharacter::CPHActorCharacter(void)
{
	m_restrictor=NULL;
}

CPHActorCharacter::~CPHActorCharacter(void)
{
}

void CPHActorCharacter::Create(dVector3 sizes)
{
	if(b_exist) return;
	m_restrictor=dCreateCylinder(0,sizes[1]/2.f,sizes[1]);
	dGeomSetPosition(m_restrictor,0.f,sizes[1]/2.f,0.f);
	m_restrictor_transform=dCreateGeomTransform(0);
	dGeomTransformSetCleanup(m_restrictor_transform,0);
	dGeomTransformSetInfo(m_restrictor_transform,1);
	dGeomTransformSetGeom(m_restrictor_transform,m_restrictor);
	dGeomCreateUserData(m_restrictor);
	dGeomUserDataSetObjectContactCallback(m_restrictor,RestrictorCallBack);
	inherited::Create(sizes);
	dGeomSetBody(m_restrictor_transform,m_body);
	dSpaceAdd(m_space,m_restrictor_transform);
	dGeomUserDataSetPhObject(m_restrictor,(CPHObject*)this);
}

void CPHActorCharacter::Destroy()
{
	if(!b_exist) return;
	if(m_restrictor) {
		dGeomDestroyUserData(m_restrictor);
		dGeomDestroy(m_restrictor);
		m_restrictor=NULL;
	}

	if(m_restrictor_transform){
		dGeomDestroyUserData(m_restrictor_transform);
		m_restrictor_transform=NULL;
	}
	inherited::Destroy();
}

void CPHActorCharacter::SetPhysicsRefObject(CPhysicsShellHolder* ref_object)
{
	inherited::SetPhysicsRefObject(ref_object);
	dGeomUserDataSetPhysicsRefObject(m_restrictor,ref_object);
}
void CPHActorCharacter::SetMaterial							(u16 material)
{
	inherited::SetMaterial(material);
	if(!b_exist) return;
	dGeomGetUserData(m_restrictor)->material=material;
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

void CPHActorCharacter::RestrictorCallBack (bool& do_colide,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
{
	do_colide=false;
	dBodyID						b1		=	dGeomGetBody(c.geom.g1);
	dBodyID						b2		=	dGeomGetBody(c.geom.g2);
	if(!(b1&&b2))	return;
	dxGeomUserData				*ud1	=	retrieveGeomUserData(c.geom.g1);
	dxGeomUserData				*ud2	=	retrieveGeomUserData(c.geom.g2);
	if(!(ud1&&ud2))return;

	CPhysicsShellHolder			*o1		=	NULL;if(ud1)o1=ud1->ph_ref_object;
	CPhysicsShellHolder			*o2		=	NULL;if(ud2)o2=ud2->ph_ref_object;
	
	if(!(o1&&o2))return;
	
	CGameObject					*go1		=	static_cast<CGameObject*>(o1);
	CGameObject					*go2		=	static_cast<CGameObject*>(o2);
	CAI_Stalker					*S			=	NULL;
	CActor						*A			=	go1->cast_actor();

	if(A)
	{
		 S=smart_cast<CAI_Stalker*>(go2);
	}
	else
	{
		 S=smart_cast<CAI_Stalker*>(go1);
		 A=go2->cast_actor();
	}
	VERIFY2(A,"wrong callback or reference object");
	
	if(S&&S->g_Alive())	
	{
		do_colide=true;
	}
}

void CPHActorCharacter::InitContact(dContact* c,bool &do_collide,SGameMtl * material_1,SGameMtl *material_2 )
{

	bool b_restrictor=false;
	if(c->geom.g1==m_restrictor_transform||c->geom.g2==m_restrictor_transform)
	{
		b_restrictor=true;
		b_side_contact=true;
		MulSprDmp(c->surface.soft_cfm,c->surface.soft_erp,def_spring_rate,def_dumping_rate);
		c->surface.mu		=0.00f;
	}
	inherited::InitContact(c,do_collide,material_1,material_2);
	if(b_restrictor&&do_collide)
	{
		bool b1=c->geom.g1==m_restrictor_transform;

		dJointID contact_joint	= dJointCreateContact(0, ContactGroup, c);
		Enable();
		CPHObject::Island().DActiveIsland()->ConnectJoint(contact_joint);
		if(b1)
			dJointAttach			(contact_joint, dGeomGetBody(c->geom.g1), 0);
		else
			dJointAttach			(contact_joint, 0, dGeomGetBody(c->geom.g2));
		do_collide=false;
		m_friction_factor*=0.1f;
		
	}
}