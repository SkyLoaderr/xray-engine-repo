#ifndef PHYSICS_H
#define PHYSICS_H

#include <ode/ode.h>
#include "dCylinder/dCylinder.h"
#include "PhysicsShell.h"

class CPHObject {
public:
virtual void PhDataUpdate(dReal step)=0;
virtual void PhTune(dReal step)=0;
};
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
	static const	UINT NofGeoms=8;
	static const	UINT NofBodies=5;
	static const	UINT NofJoints=4;
	dReal MassShift;
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
	Fvector v={pos.x-currentPos[0],pos.y-currentPos[1],pos.z-currentPos[2]};
	dBodySetPosition(Bodies[0],pos.x,pos.y,pos.z);
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
////////////////////////////////////////////////////////////////////////////
class CPHWorld {
	dSpaceID Space;
	
	CPHMesh Mesh;
	vector<CPHObject*> m_objects;
public:
	CPHGun Gun;
	//CPHJeep Jeep;
	
	//vector<CPHElement*> elements;
//	CPhysicsWorld(){};
	~CPHWorld(){};

	dSpaceID GetSpace(){return Space;};
//	dWorldID GetWorld(){return phWorld;};
	void Create();
	void AddObject(CPHObject*object){m_objects.push_back(object);};
	//CPHElement* AddElement(){
	//CPHElement* phelement=new CPHElement(Space);
	//elements.push_back(phelement);
	//return phelement;
	//}
	void Destroy();

	void Step(dReal step=0.025f);

	void Render();

//private:
//static void FUNCCALL NearCallback(void* /*data*/, dGeomID o1, dGeomID o2);
};
/////////////////////////////////////////////////////////////////////////////

extern dWorldID phWorld;
/////////////////////////////////
static dJointGroupID ContactGroup;
/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////Implemetation//for//CPhysicsElement//////////////////
////////////////////////////////////////////////////////////////////////////////
//using namespace std;
class CPHElement: public CPhysicsElement {
vector <dGeomID> m_geoms;
vector <dGeomID> m_trans;
vector <Fsphere> m_spheras_data;
vector <Fobb>    m_boxes_data;
float m_volume;
Fvector m_mass_center;
dMass m_mass;
dSpaceID m_space;
dBodyID m_body;
dGeomID m_group;
Fmatrix m_m0,m_m2;
void			create_Sphere				(Fsphere&	V);
void			create_Box					(Fobb&		V);
void			calculate_it_data			(const Fvector& mc,float mass);
public:
///
	virtual	void			add_Sphere				(const Fsphere&	V);
														
	virtual	void			add_Box					(const Fobb&		V);

	void			build(dSpaceID space);
	void			destroy();
	Fvector			get_mc_data();
	Fvector			get_mc_geoms();
	void			Start();
	dBodyID			get_body(){return m_body;};
	float			get_volume(){get_mc_data();return m_volume;};
	void			SetTransform(const Fmatrix& m0);
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2);
	virtual void			Deactivate				();
	virtual void			setMass					(float M);
	virtual void			applyForce				(const Fvector& dir, float val){;};
	virtual void			applyImpulse			(const Fvector& dir, float val){;};
	virtual void			Update					();
	CPHElement(dSpaceID a_space){ 
								m_space=a_space;
								m_body=NULL;};
	//CPHElement(){ m_space=ph_world->GetSpace();};
	virtual ~CPHElement	();
};
///////////////////////////////////////////////////////////////////////
class CPHShell: public CPhysicsShell {
vector<CPHElement*> elements;
Fmatrix m_m2;
Fmatrix m_m0;
dBodyID m_body;
public:
	virtual	void			add_Element				(CPhysicsElement* E)		  {elements.push_back((CPHElement*)E);};
	virtual	void			add_Joint				(CPhysicsJoint* E, int E1, int E2)					{};
	virtual void			applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	{};

	virtual void			Update					()	;											

	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2);
	virtual void			Deactivate				()		;

	virtual void			setMass					(float M)									;

	virtual void			applyForce				(const Fvector& dir, float val)				{};
	virtual void			applyImpulse			(const Fvector& dir, float val)				{};

};


///////////////////////////////////////////////////////////////////////////////////////////////////
#endif PHYSICS_H