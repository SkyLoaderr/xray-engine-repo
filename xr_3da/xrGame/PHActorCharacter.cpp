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

CPHActorCharacter::CPHActorCharacter()
{
	SetRestrictionType(CPHCharacter::rtActor);

	std::fill(m_restrictors_index,m_restrictors_index+CPHCharacter::rtNone,end(m_restrictors));
	m_restrictors_index[CPHCharacter::rtStalker]		=begin(m_restrictors)+0;
	m_restrictors_index[CPHCharacter::rtMonsterMedium]	=begin(m_restrictors)+1;
	
	m_restrictors[0]=(xr_new<stalker_restrictor>());
	m_restrictors[1]=(xr_new<medium_monster_restrictor>());
}

CPHActorCharacter::~CPHActorCharacter(void)
{
}

void CPHActorCharacter::Create(dVector3 sizes)
{
	if(b_exist) return;
	inherited::Create(sizes);

	RESTRICTOR_I i=begin(m_restrictors),e=end(m_restrictors);
	for(;e!=i;++i)
	{
		(*i)->Create(this,sizes);
	}

	if(m_phys_ref_object)
	{
		SetPhysicsRefObject(m_phys_ref_object);
	}
}
void SPHCharacterRestrictor::Create(CPHCharacter* ch,dVector3 sizes)
{
	VERIFY(ch);
	if(m_character)return;
	m_character=ch;
	m_restrictor=dCreateCylinder(0,m_restrictor_radius,sizes[1]);
	dGeomSetPosition(m_restrictor,0.f,sizes[1]/2.f,0.f);
	m_restrictor_transform=dCreateGeomTransform(0);
	dGeomTransformSetCleanup(m_restrictor_transform,0);
	dGeomTransformSetInfo(m_restrictor_transform,1);
	dGeomTransformSetGeom(m_restrictor_transform,m_restrictor);
	dGeomCreateUserData(m_restrictor);

	
	dGeomSetBody(m_restrictor_transform,m_character->get_body());
	dSpaceAdd(m_character->dSpace(),m_restrictor_transform);
	dGeomUserDataSetPhObject(m_restrictor,(CPHObject*)m_character);
	switch(m_type) {
		case CPHCharacter::rtStalker:static_cast<CPHActorCharacter::stalker_restrictor*>(this)->Create(ch,sizes);
		break;
		case CPHCharacter::rtMonsterMedium:static_cast<CPHActorCharacter::medium_monster_restrictor*>(this)->Create(ch,sizes);
		break;
		default:NODEFAULT;
	}
	
}

RESTRICTOR_I CPHActorCharacter::Restrictor(CPHCharacter::ERestrictionType rtype)
{
	return m_restrictors_index[rtype];
}
void CPHActorCharacter::SetRestrictorRadius(CPHCharacter::ERestrictionType rtype,float r)
{
	(*Restrictor(rtype))->SetRadius(r);
}

void SPHCharacterRestrictor::SetRadius(float r)
{
	m_restrictor_radius=r;
	if(m_character)
	{
		float h;
		dGeomCylinderGetParams(m_restrictor,&r,&h);
		dGeomCylinderSetParams(m_restrictor,m_restrictor_radius,h);
	}
}
void CPHActorCharacter::Destroy()
{
	if(!b_exist) return;
	RESTRICTOR_I i=begin(m_restrictors),e=end(m_restrictors);
	for(;e!=i;++i)
	{
		(*i)->Destroy();
	}
	inherited::Destroy();
}

void SPHCharacterRestrictor::Destroy()
{
	if(m_restrictor) {
		dGeomDestroyUserData(m_restrictor);
		dGeomDestroy(m_restrictor);
		m_restrictor=NULL;
	}

	if(m_restrictor_transform){
		dGeomDestroyUserData(m_restrictor_transform);
		m_restrictor_transform=NULL;
	}
	m_character=NULL;
}
void CPHActorCharacter::SetPhysicsRefObject(CPhysicsShellHolder* ref_object)
{
	inherited::SetPhysicsRefObject(ref_object);
	RESTRICTOR_I i=begin(m_restrictors),e=end(m_restrictors);
	for(;e!=i;++i)
	{
		(*i)->SetPhysicsRefObject(ref_object);
	}
}
void SPHCharacterRestrictor::SetPhysicsRefObject(CPhysicsShellHolder* ref_object)
{
	if(m_character)
		dGeomUserDataSetPhysicsRefObject(m_restrictor,ref_object);
}
void CPHActorCharacter::SetMaterial							(u16 material)
{
	inherited::SetMaterial(material);
	if(!b_exist) return;
	RESTRICTOR_I i=begin(m_restrictors),e=end(m_restrictors);
	for(;e!=i;++i)
	{
		(*i)->SetMaterial(material);
	}
}
void SPHCharacterRestrictor::SetMaterial(u16 material)
{
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

}

void CPHActorCharacter::Disable()
{
	inherited::Disable();
}


struct SFindPredicate
{
	SFindPredicate(const dContact* ac,bool *b)
	{
		c=ac;
		b1=b;
	}
	bool			*b1	;
	const dContact	*c	;
	bool operator ()	(SPHCharacterRestrictor* o)
	{
		*b1=c->geom.g1==o->m_restrictor_transform;
		return *b1||c->geom.g2==o->m_restrictor_transform;
	}
};
void CPHActorCharacter::InitContact(dContact* c,bool &do_collide,SGameMtl * material_1,SGameMtl *material_2 )
{

	
	bool b1;
	SFindPredicate fp(c,&b1);
	RESTRICTOR_I r=std::find_if(begin(m_restrictors),end(m_restrictors),fp);
	bool b_restrictor=(r!=end(m_restrictors));
	if(b_restrictor)
	{
		b_side_contact=true;
		MulSprDmp(c->surface.soft_cfm,c->surface.soft_erp,def_spring_rate,def_dumping_rate);
		c->surface.mu		=0.00f;
	}
	
	inherited::InitContact(c,do_collide,material_1,material_2);
	if(b_restrictor&&
		do_collide&&
		!(b1 ? static_cast<CPHCharacter*>(retrieveGeomUserData(c->geom.g2)->ph_object)->ActorMovable():static_cast<CPHCharacter*>(retrieveGeomUserData(c->geom.g1)->ph_object)->ActorMovable())
		)
	{
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