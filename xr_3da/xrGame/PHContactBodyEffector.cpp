#include "stdafx.h"
#include "PHContactBodyEffector.h"
#include "ExtendedGeom.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PhysicsCommon.h"
void CPHContactBodyEffector::Merge(dContact& contact, float	flotation)
{
	m_recip_flotation=_max(1.f/flotation,m_recip_flotation);
	m_contact.geom.normal[0]+=contact.geom.normal[0];
	m_contact.geom.normal[1]+=contact.geom.normal[1];
	m_contact.geom.normal[2]+=contact.geom.normal[2];
}

void CPHContactBodyEffector::Apply()
{
	const dReal*	linear_velocity				=dBodyGetLinearVel(m_body);
	dReal			linear_velocity_smag		=dDOT(linear_velocity,linear_velocity);
	dReal			linear_velocity_mag			=_sqrt(linear_velocity_smag);
	dReal			effect						=default_k_l*m_recip_flotation*m_contact.geom.depth;
	dMass mass;
	dBodyGetMass(m_body,&mass);
	dReal l_air=linear_velocity_mag*effect;//force/velocity !!!
	if(l_air>mass.mass/fixed_step) l_air=mass.mass/fixed_step;//validate

	if(!fis_zero(l_air))
		dBodyAddForce(
		m_body,
		-linear_velocity[0]*l_air,
		-linear_velocity[1]*l_air,
		-linear_velocity[2]*l_air
		);

	dBodySetData(m_body,NULL);
}