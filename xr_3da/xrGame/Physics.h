#ifndef PHYSICS_H
#define PHYSICS_H

#include "dCylinder/dCylinder.h"
#include "PhysicsShell.h"
#include "PHObject.h"
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
	bool weels_limited;
	void CreateDynamicData();
public:
	CPHJeep(){weels_limited=true;}

	void Create(dSpaceID space, dWorldID world);
	void Create1(dSpaceID space, dWorldID world);
	void Destroy();
	void Steer1(const char& velocity, const char& steering);
	void Steer(const char& steering);
	void LimitWeels();
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
	void SetRotation(dReal* R){
		const dReal* currentPos0=dBodyGetPosition(Bodies[0]);	
		const dReal* currentR=dBodyGetRotation(Bodies[0]);
		dMatrix3 relRot;
		dMULTIPLYOP1_333(relRot,=,R,currentR);
		dBodySetRotation(Bodies[0],R);
		
		
		for(unsigned int i=1;i<NofBodies; i++ ){
			const dReal* currentPos=dBodyGetPosition(Bodies[i]);
			dVector3 relPos={currentPos[0]-currentPos0[0],currentPos[1]-currentPos0[1],currentPos[2]-currentPos0[2]};
			dVector3 relPosRotated;
			dMULTIPLYOP0_331(relPosRotated,=,relRot,relPos);
			dVector3 posChange={relPosRotated[0]-relPos[0],relPosRotated[1]-relPos[1],relPosRotated[2]-relPos[2]};
			dVector3 newPos={currentPos[0]+posChange[0],currentPos[1]+posChange[1],currentPos[2]+posChange[2]};
			dBodySetPosition(Bodies[i],newPos[0],newPos[1],newPos[2]);

			const dReal* currentR=dBodyGetRotation(Bodies[i]);
			dMatrix3 newRotation;
			dMULTIPLYOP0_333(newRotation,=,currentR,relRot);
			dBodySetRotation(Bodies[i],newRotation);
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
const dReal fixed_step=0.02f;
/////////////////////////////////////////////////////////////////////////////
class CPHWorld {
	__int64 m_steps_num;
	double m_start_time;
	UINT   m_delay;
	UINT   m_previous_delay;
	UINT   m_reduce_delay;
	UINT   m_update_delay_count;
	static const UINT update_delay=1;
	dSpaceID Space;
	
	CPHMesh Mesh;
	list<CPHObject*> m_objects;
public:
	double m_frame_sum;
	dReal frame_time;
	float m_update_time;
	CPHGun Gun;
	//CPHJeep Jeep;
	unsigned int disable_count;
	//vector<CPHElement*> elements;
	CPHWorld(){disable_count=0;frame_time=0.f;m_steps_num=0;m_start_time=Device.fTimeGlobal;m_frame_sum=0.f;
	m_delay=0; m_previous_delay=0;m_reduce_delay=0;m_update_delay_count=0;}
	~CPHWorld(){};
	double Time(){return m_start_time+m_steps_num*fixed_step;}
	dSpaceID GetSpace(){return Space;};
	//	dWorldID GetWorld(){return phWorld;};
	void Create();
	list <CPHObject*> ::iterator AddObject(CPHObject* object){
		m_objects.push_back(object);
		//list <CPHObject*> ::iterator i= m_objects.end();
		return --(m_objects.end());
	};
	void RemoveObject(list<CPHObject*> :: iterator i){
		m_objects.erase((i));
	};
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
	void			calculate_it_data_use_density(const Fvector& mc,float density);
public:
	Fmatrix m_inverse_local_transform;
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
	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false);
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
class CPHShell: public CPhysicsShell,public CPHObject {
	vector<CPHElement*> elements;
	Fmatrix m_m2;
	Fmatrix m_m0;
	dBodyID m_body;
	Fmatrix m_inverse_local_transform;
//dVector3 mean_w;
//dVector3 mean_v;
dVector3 previous_p;
dMatrix3 previous_r;
dVector3 previous_p1;
dMatrix3 previous_r1;
//dVector3 previous_f;
//dVector3 previous_t;
dReal previous_dev;
dReal previous_v;
UINT dis_count_f;
UINT dis_count_f1;
list<CPHObject*>::iterator m_ident;
public:
	CPHShell				()							{bActive=false;
														dis_count_f=0;
														dis_count_f1=0;
											
																		};
	~CPHShell				()							{if(bActive) Deactivate();}

	virtual	void			add_Element				(CPhysicsElement* E)		  {
		elements.push_back((CPHElement*)E);
	};
	virtual	void			add_Joint				(CPhysicsJoint* E, int E1, int E2)					{};
	virtual void			applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	{};

	virtual void			Update					()	;											

	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false);
	virtual void			Deactivate				()		;

	virtual void			setMass					(float M)									;

	virtual void			applyForce				(const Fvector& dir, float val)				{};
	virtual void			applyImpulse			(const Fvector& dir, float val)				{};
	virtual	void PhDataUpdate(dReal step);
	virtual	void PhTune(dReal step);
	virtual void InitContact(dContact* c){};
	virtual void StepFrameUpdate(dReal step){};

};


///////////////////////////////////////////////////////////////////////////////////////////////////
#endif PHYSICS_H