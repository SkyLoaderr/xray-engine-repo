#ifndef PHYSICS_H
#define PHYSICS_H


#include "dCylinder/dCylinder.h"
#include "PhysicsShell.h"
#include "PHObject.h"
#include "PHInterpolation.h"
#include "_cylinder.h"
#include "BlockAllocator.h"
#include "PhysicsCommon.h"
#include "PHWorld.h"
#include "PHContactBodyEffector.h"
//#define ODE_SLOW_SOLVER
///////////////////////////////////////////////////////////////////////////////

void	BodyCutForce			(dBodyID body,float l_limit,float w_limit)					;
void	dBodyAngAccelFromTorqu	(const dBodyID body, dReal* ang_accel, const dReal* torque)	;
float	E_NlS					(dBodyID body,dReal* norm,float norm_sign)					;
void	ApplyGravityAccel		(dBodyID body,const dReal* accel);
const	dReal					fix_ext_param=10000.f;
const	dReal					fix_mass_param=100000000.f;
void	FixBody					(dBodyID body)												;
void	dMassSub				(dMass *a,const dMass *b)									;
void	SaveContacts			(dGeomID o1, dGeomID o2,dJointGroupID jointGroup)			;


IC BOOL dV_valid			(const dReal * v)
{
	return _valid(v[0])&&_valid(v[1])&&_valid(v[2]);
}

IC BOOL dM_valid			(const dReal* m)
{
	return  _valid(m[0])&&_valid(m[1])&&_valid(m[2])&& 
		_valid(m[4])&&_valid(m[5])&&_valid(m[6])&&
		_valid(m[8])&&_valid(m[9])&&_valid(m[10]);
}

IC BOOL dV4_valid			(const dReal* v4)
{
	return _valid(v4[0])&&_valid(v4[1])&&
			_valid(v4[2])&&_valid(v4[3]);
}
IC BOOL dQ_valid			(const dReal* q)
{
	return dV4_valid(q);
}
IC BOOL dMass_valide(const dMass* m)
{
	return	_valid(m->mass)&&
			dV_valid(m->c)&&
			dM_valid(m->I);
}
IC BOOL dBodyStateValide(const dBodyID body)
{
	return dM_valid(dBodyGetRotation(body)) &&
			dV_valid(dBodyGetPosition(body))&&
			dV_valid(dBodyGetLinearVel(body))&&
			dV_valid(dBodyGetAngularVel(body));
}



//const dReal world_spring=24000000.f;//2400000.f;//550000.f;///1000000.f;;
//const dReal world_damping=400000.f;//erp/cfm1.1363636e-006f,0.54545456f


extern class CBlockAllocator<dJointFeedback,128> ContactFeedBacks;
extern CBlockAllocator<CPHContactBodyEffector,128> ContactEffectors;
//void NearCallback(void* /*data*/, dGeomID o1, dGeomID o2);
void NearCallback(CPHObject* obj1,CPHObject* obj2, dGeomID o1, dGeomID o2);
void CollideStatic(dGeomID o2,CPHObject* obj2);



class CPHElement;
class CPHShell;
extern dJointGroupID ContactGroup;

#ifdef DRAW_CONTACTS
DEFINE_VECTOR(dContact,CONTACT_VECTOR,CONTACT_I);
extern CONTACT_VECTOR Contacts;
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif PHYSICS_H