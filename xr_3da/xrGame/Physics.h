#include <ode/ode.h>
#ifndef PHYSICS_H
#define PHYSICS_H

///////////////////////////////////////////////////////////////////////////////
class CPHMesh {
	dGeomID Geom;
public:
	void Create(dSpaceID space, dWorldID world);
	void Destroy();
};
///////////////////////////////////////////////////////////////////////////////////
class CPHJeep {
	dGeomID GeomsGroup;
	static const	UINT NofGeoms=5;
	static const	UINT NofBodies=6;
	static const	UINT NofJoints=4;
	dBodyID Bodies[NofBodies];
	dGeomID Geoms[NofGeoms];
	dJointID Joints[NofJoints];


	void CreateDynamicData();
public:
	CPHJeep(){}
	void Create(dSpaceID space, dWorldID world);
	void Destroy();
	void Steer(const char& velocity, const char& steering);

	PHDynamicData DynamicData;
};

//////////////////////////////////////////////////////////////////////////////////
class CPHMan{
};

class CPHGun{
};
/////////////////////////////////////////////////////////////////////////////
static dWorldID World;
static dJointGroupID ContactGroup;

class CPHWorld {
	dSpaceID Space;

	CPHJeep Jeep;
	CPHMesh Mesh;

public:
//	CPhysicsWorld(){};
	~CPHWorld(){};

	void Create();

	void Destroy();

	void Step(dReal step=0.025f);

private:
		static void FUNCCALL NearCallback(void* /*data*/, dGeomID o1, dGeomID o2){
	const ULONG N = 100;
	dContact contacts[N];
	// get the contacts up to a maximum of N contacts
   ULONG n = dCollide(o1, o2, N, &contacts[0].geom, sizeof(dContact));	

/*
	if(
		dGeomGetClass(o1)==dRayClass||dGeomGetClass(o2)==dRayClass
		){
		if(o1==canon||o2==canon) return;
		if(isShootting){
		dVector3 norm={rayD[0],rayD[1],rayD[2]};
		dNormalize3(norm);
	//	ulong n = dCollide(o1, o2, N, &bulletContact.geom, sizeof(dContact));
			for(int i = 0; i < n; ++i)
				{
			
				if(dDOT(contacts[i].geom.pos,norm)<dDOT(bulletContact.geom.pos,norm))

				{

			//	bulletContact.geom.pos[0]=contacts[i].geom.pos[0];
			//	bulletContact.geom.pos[1]=contacts[i].geom.pos[1];
			//	bulletContact.geom.pos[2]=contacts[i].geom.pos[2];
				memcpy(&bulletContact,&contacts[i],sizeof(dContact));
				bulletContact.geom.normal[0]=norm[0];
				bulletContact.geom.normal[1]=norm[1];
				bulletContact.geom.normal[2]=norm[2];
				
				bulletContact.geom.depth=0.5;
				
				bulletContact.surface.mode = dContactBounce;
				bulletContact.surface.mu = 0;
				bulletContact.surface.bounce = 0.3;
				bulletContact.surface.bounce_vel = 0.05;
				

				bulletJoint = dJointCreateContact(world, contactGroup, &bulletContact);
				//dJointAttach(bulletJoint, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(o2));
				}
			//isShootting=false;
			}
	
			}
		return;
	}
		
*/	

	if(n>N)
		n=N;
	ULONG i;

if(dGeomGetClass(o1)==dGeomTransformClass){
	for(i = 0; i < n; ++i)
	{

        contacts[i].surface.mode = dContactBounce;
		contacts[i].surface.mu = 500;
		contacts[i].surface.bounce = 0.3f;
		contacts[i].surface.bounce_vel = 0.05f;
		dJointID c = dJointCreateContact(World, ContactGroup, &contacts[i]);
		dJointAttach(c, dGeomGetBody(o1), dGeomGetBody(contacts[i].geom.g2));
		}

	}
else if(dGeomGetClass(o2)==dGeomTransformClass){
		for(i = 0; i < n; ++i)
		{

        contacts[i].surface.mode = dContactBounce;
		contacts[i].surface.mu = 500.f;
		contacts[i].surface.bounce = 0.3f;
		contacts[i].surface.bounce_vel = 0.05f;
		dJointID c = dJointCreateContact(World, ContactGroup, &contacts[i]);
		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(o2));
		}
	}

else
	for(i = 0; i < n; ++i)
	{

        contacts[i].surface.mode = dContactBounce;
		contacts[i].surface.mu = 250.f;
		contacts[i].surface.bounce = 0.3f;
		contacts[i].surface.bounce_vel =0.05f;
		dJointID c = dJointCreateContact(World, ContactGroup, &contacts[i]);
		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
		}
};
};

#endif PHYSICS_H