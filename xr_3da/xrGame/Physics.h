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
	static const	UINT NofGeoms=7;
	static const	UINT NofBodies=5;
	static const	UINT NofJoints=4;
	dBodyID Bodies[NofBodies];
	dGeomID Geoms[NofGeoms];
	dJointID Joints[NofJoints];


	void CreateDynamicData();
public:
	//CPHJeep(){}
	void Create(dSpaceID space, dWorldID world);
	void Destroy();
	void Steer(const char& velocity, const char& steering);

	PHDynamicData DynamicData;
};

//////////////////////////////////////////////////////////////////////////////////
class CPHMan{
};


class CPHGun{
public:
	//dGeomID canon;

	dGeomID Ray;



	void Create(dSpaceID space);
	void Destroy();
	void Shoot(const dReal* rayO,const dReal* rayD);

};

/////////////////////////////////////////////////////////////////////////////
static dWorldID phWorld;
static dJointGroupID ContactGroup;

class CPHWorld {
	dSpaceID Space;
	CPHGun Gun;
	CPHJeep Jeep;
	CPHMesh Mesh;
	

public:
//	CPhysicsWorld(){};
	~CPHWorld(){};

	void Create();

	void Destroy();

	void Step(dReal step=0.025f);

	void Render();

//private:
//static void FUNCCALL NearCallback(void* /*data*/, dGeomID o1, dGeomID o2);
};

#endif PHYSICS_H