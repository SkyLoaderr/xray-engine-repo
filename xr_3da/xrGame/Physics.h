#ifndef PHYSICS_H
#define PHYSICS_H

#include "dCylinder/dCylinder.h"
#include "PhysicsShell.h"
#include "PHObject.h"
#include "PHInterpolation.h"
///////////////////////////////////////////////////////////////////////////////
const dReal fixed_step=0.02f;
const int dis_frames=11;

const dReal world_spring=24000000.f;//2400000.f;//550000.f;///1000000.f;;
const dReal world_damping=400000.f;
#define ERP(k_p,k_d)	((fixed_step*(k_p)) / (((fixed_step)*(k_p)) + (k_d)))
#define CFM(k_p,k_d)	(1.f / (((fixed_step)*(k_p)) + (k_d)))


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

	void applyImpulseTrace		(int part,const Fvector& pos, const Fvector& dir, float val){
	val/=fixed_step;
	if(part<0||part>NofBodies-1) return;
	Fvector body_pos;
	body_pos.set(pos);
	if(part==0){ body_pos.sub(DynamicData.BoneTransform.c);
	dBodyAddForceAtRelPos       (Bodies[part], dir.x*val,dir.y*val,dir.z*val,pos.z, pos.y, -pos.x);
	}
	else
		dBodyAddForceAtRelPos       (Bodies[part], dir.x*val,dir.y*val,dir.z*val,pos.x, pos.z, -pos.y);
	//else		body_pos.sub(DynamicData[part-1].BoneTransform.c);
	}
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



////////////////////////////////////////////////////////////////////////////
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
	//CPHGun Gun;
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
class CPHElement;
class CPHShell;
class CPHElement:  public CPhysicsElement {

	vector <dGeomID>		m_geoms;
	vector <dGeomID>		m_trans;
	vector <Fsphere>		m_spheras_data;
	vector <Fobb>			m_boxes_data;
	bool					bActive;
	bool					bActivating;
	float					m_start_time;
	float m_volume;
	Fvector m_mass_center;
	
	dMass m_mass;
	dSpaceID m_space;
	dBodyID m_body;
	dGeomID m_group;
	Fmatrix m_m0,m_m2;
///////////////////////////////

	CPHElement* m_parent_element;
	CPHShell*   m_shell;
	CPHInterpolation m_body_interpolation;


/////disable///////////////////////
//dVector3 mean_w;
//dVector3 mean_v;
dVector3 m_safe_position;
dVector3 m_safe_velocity;
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
public:

/////////////////////////////////////////////////////////////////////////////

////////////////////////////
private:
	void			create_Sphere				(Fsphere&	V);
	void			create_Box					(Fobb&		V);
	void			calculate_it_data			(const Fvector& mc,float mass);
	void			calculate_it_data_use_density(const Fvector& mc,float density);
	void			Disable						();
public:


	void					CallBack				(CBoneInstance* B);
	void					PhDataUpdate			(dReal step);
	virtual void			set_ParentElement		(CPhysicsElement* p){m_parent_element=(CPHElement*)p;}

	virtual void			applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	;
	Fmatrix m_inverse_local_transform;
	///
	virtual	void			add_Sphere				(const Fsphere&	V);

	virtual	void			add_Box					(const Fobb&		V);
	void			SetShell		(CPHShell* p){m_shell=p;}
	void			InterpolateGlobalTransform(Fmatrix* m);
	void			build(dSpaceID space);
	void			destroy();
	Fvector			get_mc_data();
	Fvector			get_mc_geoms();
	void			Start();
	void			RunSimulation();
	dBodyID			get_body(){return m_body;};
	float			get_volume(){get_mc_data();return m_volume;};
	void			SetTransform(const Fmatrix& m0);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false);
	virtual void			Activate				();
	virtual void			Deactivate				();
	virtual void			setMass					(float M);
	virtual void			applyForce				(const Fvector& dir, float val){
																					if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
																					dBodyAddForce(m_body,dir.x*val,dir.y*val,dir.z*val);
																					};
	virtual void			applyImpulse			(const Fvector& dir, float val){
																					if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
																					dBodyAddForce(m_body,dir.x*val/fixed_step,dir.y*val/fixed_step,dir.z*val/fixed_step);
																					};
	virtual void			Update					();
	CPHElement(dSpaceID a_space){ 
		m_space=a_space;
		m_body=NULL;
		bActive=false;
		bActivating=false;
		dis_count_f=0;
		dis_count_f1=0;
		m_parent_element=NULL;
		m_shell=NULL;
		m_group=NULL;
	};
		//CPHElement(){ m_space=ph_world->GetSpace();};
		virtual ~CPHElement	();
};
//////////////////////////////////////////////////////////////////////
class CPHJoint: public CPhysicsJoint{


dJointID m_joint;

struct SPHAxis {
float high;
float low;
float erp;
float cfm;
Fvector direction;
IC void set_limits(float h, float l) {high=h; low=l;}
IC void set_direction(const Fvector& v){direction.set(v);}
IC void set_direction(const float x,const float y,const float z){direction.set(x,y,z);}
IC void set_param(const float e,const float c){erp=e;cfm=c;}	
SPHAxis(){
	high=M_PI/15.f;
	low=-M_PI/15.f;;
	//erp=ERP(world_spring/5.f,world_damping*5.f);
	//cfm=CFM(world_spring/5.f,world_damping*5.f);
	erp=0.8f;
	cfm=0.000001f;
	direction.set(0,0,1);
	}
};
vector<SPHAxis> axes;
Fvector anchor;

void CreateBall();
void CreateHinge();
void CreateHinge2();
void CreateShoulder1();
void CreateShoulder2();
void CreateCarWeel();
void CreateUniversalHinge();

	virtual void SetAnchor					(const Fvector& position){SetAnchor(position.x,position.y,position.z);}	
	virtual void SetAnchorVsFirstElement	(const Fvector& position){SetAnchorVsFirstElement(position.x,position.y,position.z);}
	virtual void SetAnchorVsSecondElement	(const Fvector& position){SetAnchorVsSecondElement(position.x,position.y,position.z);}

	virtual void SetAxis					(const Fvector& orientation,const int axis_num){SetAxis(orientation.x,orientation.y,orientation.z,axis_num);}	
	virtual void SetAxisVsFirstElement		(const Fvector& orientation,const int axis_num){SetAxisVsFirstElement(orientation.x,orientation.y,orientation.z,axis_num);}
	virtual void SetAxisVsSecondElement		(const Fvector& orientation,const int axis_num){SetAxisVsSecondElement(orientation.x,orientation.y,orientation.z,axis_num);}

	virtual void SetLimits					(const float low,const float high,const int axis_num)	;
	virtual void SetLimitsVsFirstElement	(const float low,const float high,const int axis_num)	;
	virtual void SetLimitsVsSecondElement	(const float low,const float high,const int axis_num)	;

	virtual void SetAnchor					(const float x,const float y,const float z)	;
	virtual void SetAnchorVsFirstElement	(const float x,const float y,const float z)	;
	virtual void SetAnchorVsSecondElement	(const float x,const float y,const float z)	;

	virtual void SetAxis					(const float x,const float y,const float z,const int axis_num);
	virtual void SetAxisVsFirstElement		(const float x,const float y,const float z,const int axis_num);
	virtual void SetAxisVsSecondElement		(const float x,const float y,const float z,const int axis_num);
public:
	CPHJoint(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second);
	virtual ~CPHJoint(){
						axes.clear();

						};
	void Activate();
	void Deactivate();
};




///////////////////////////////////////////////////////////////////////
class CPHShell: public CPhysicsShell,public CPHObject {
	vector<CPHElement*> elements;
	vector<CPHJoint*>	joints;
	dSpaceID			m_space;
	Fmatrix m_m2;
	Fmatrix m_m0;
	bool bActivating;

list<CPHObject*>::iterator m_ident;
				
public:

	CPHShell				()							{bActive=false;
														 bActivating=false;
														 m_space=NULL;

											
																		};
	~CPHShell				()							{if(bActive) Deactivate();

															vector<CPHElement*>::iterator i;
														for(i=elements.begin();i!=elements.end();i++)
																							xr_delete(*i);
														elements.clear();

														vector<CPHJoint*>::iterator j;
														for(j=joints.begin();j!=joints.end();j++)
																							xr_delete(*j);
														joints.clear();
														}


	static void __stdcall	BonesCallback				(CBoneInstance* B);
	virtual	BoneCallbackFun* GetBonesCallback		()	{return BonesCallback ;}
	virtual	void			add_Element				(CPhysicsElement* E)		  {
		CPHElement* ph_element=(CPHElement*)E;
		ph_element->SetShell(this);
		elements.push_back(ph_element);

	};

	virtual	void			add_Joint				(CPhysicsJoint* J)					{
																						joints.push_back((CPHJoint*)J);
																						};
	virtual void			applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	;

	virtual void			Update					()	;											

	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false);
	virtual void			Activate				();
	virtual void			Deactivate				()		;

	virtual void			setMass					(float M)									;

	virtual void			applyForce				(const Fvector& dir, float val)				{
																								(*elements.begin())->applyForce				( dir, val);
																								};
	virtual void			applyImpulse			(const Fvector& dir, float val)				{
																								(*elements.begin())->applyImpulse			( dir, val);
																								};
	virtual	void PhDataUpdate(dReal step);
	virtual	void PhTune(dReal step);
	virtual void InitContact(dContact* c){};
	virtual void StepFrameUpdate(dReal step){};

	dSpaceID GetSpace()
	{
		return m_space;
	}
	void CreateSpace()
	{
		if(!m_space) m_space=dSimpleSpaceCreate(ph_world->GetSpace());
	}

	void SetTransform(Fmatrix m);
private:
	

};


///////////////////////////////////////////////////////////////////////////////////////////////////
#endif PHYSICS_H