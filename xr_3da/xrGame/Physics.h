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
	dVector3 startPosition;

	void CreateDynamicData();
public:
	//CPHJeep(){}

	void Create(dSpaceID space, dWorldID world);
	void Create1(dSpaceID space, dWorldID world);
	void Destroy();
	void Steer1(const char& velocity, const char& steering);
	void Steer(const char& steering);
	void Drive(const char& velocity,dReal force=500.f);
	void Drive();
	void NeutralDrive();
	void JointTune(dReal step);
	void Revert();
	void SetStartPosition(Fvector pos){dBodySetPosition(Bodies[0],pos.x,pos.y,pos.z);}
	void SetPosition(Fvector pos){
	const dReal* currentPos=dBodyGetPosition(Bodies[0]);	
	Fvector v={pos.x-currentPos[0],pos.y+1.f-currentPos[1],pos.z-currentPos[2]};
	dBodySetPosition(Bodies[0],pos.x,pos.y+1.f,pos.z);
	for(unsigned int i=1;i<NofBodies; i++ ){
		const dReal* currentPos=dBodyGetPosition(Bodies[i]);
		dVector3 newPos={currentPos[0]+v.x,currentPos[1]+v.y,currentPos[2]+v.z};
		dBodySetPosition(Bodies[i],newPos[0],newPos[1],newPos[2]);
		}
	}
	Fvector GetVelocity(){
		Fvector ret;
		const dReal* vel=dBodyGetLinearVel(Bodies[0]);
		ret.x=vel[0];
		ret.y=vel[1];
		ret.z=vel[2];
		return ret;
	}
	PHDynamicData DynamicData;
	dVector3 jeepBox;
	dVector3 cabinBox;
	dReal VelocityRate;
	dReal DriveForce;
	
	dReal DriveVelocity;
	char  DriveDirection;
	bool Breaks;
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

	CPHMesh Mesh;
	

public:
	CPHGun Gun;
	CPHJeep Jeep;
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