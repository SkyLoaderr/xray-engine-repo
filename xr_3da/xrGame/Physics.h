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
	static const	u32 NofGeoms=8;
	static const	u32 NofBodies=5;
	static const	u32 NofJoints=4;
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
	u64		m_steps_num;
	double	m_start_time;
	u32		m_delay;
	u32		m_previous_delay;
	u32		m_reduce_delay;
	u32		m_update_delay_count;
	static const u32 update_delay=1;
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
	CPHWorld(){disable_count=0;frame_time=0.f;m_steps_num=0;m_frame_sum=0.f;
	m_delay=0; m_previous_delay=0;m_reduce_delay=0;m_update_delay_count=0;}
	~CPHWorld(){};
	//double Time(){return m_start_time+m_steps_num*fixed_step;}
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
	float					m_volume;
	Fvector					m_mass_center;
	u32						ul_material;
	dMass					m_mass;
	dSpaceID				m_space;
	dBodyID					m_body;
	dGeomID					m_group;
///////////////////////////////

	CPHElement				*m_parent_element;
	CPHShell				*m_shell;
	CPHInterpolation		m_body_interpolation;


/////disable///////////////////////
//dVector3 mean_w;
//dVector3 mean_v;
dVector3					m_safe_position;
dVector3					m_safe_velocity;
dVector3					previous_p;
dMatrix3					previous_r;
dVector3					previous_p1;
dMatrix3					previous_r1;
//dVector3 previous_f;
//dVector3 previous_t;
dReal						previous_dev;
dReal						previous_v;
UINT						dis_count_f;
UINT						dis_count_f1;
dReal						k_w;
dReal						k_l;//1.8f;
bool						attached;
bool						b_contacts_saved;
dJointGroupID				m_saved_contacts;			
public:

/////////////////////////////////////////////////////////////////////////////

////////////////////////////
private:
	void			create_Sphere				(Fsphere&	V);
	void			create_Box					(Fobb&		V);
	void			calculate_it_data			(const Fvector& mc,float mass);
	void			calculate_it_data_use_density(const Fvector& mc,float density);
	void			Disabling						();
public:
	void					Disable					();
	void					ReEnable				();
	void					DynamicAttach			(CPHElement * E);
	virtual void			SetAirResistance		(dReal linear=0.0002f, dReal angular=0.05f) {
													k_w= angular;
													k_l=linear;}
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
	virtual void			SetMaterial				(u32 m){ul_material=m;}
	virtual void			SetMaterial				(LPCSTR m){ul_material=GMLib.GetMaterialIdx(m);}
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
		ul_material=GMLib.GetMaterialIdx("materials\\box_default");
		k_w=0.05f;
		k_l=0.0002f;//1.8f;
		attached=false;
	};
		//CPHElement(){ m_space=ph_world->GetSpace();};
		virtual ~CPHElement	();
};
//////////////////////////////////////////////////////////////////////
class CPHJoint: public CPhysicsJoint{

CPHShell*   pShell;
dJointID m_joint;
dJointID m_joint1;
enum eVs {
vs_first,
vs_second,
vs_global
};
struct SPHAxis {
float high;
float low;
float zero;
Fmatrix zero_transform;
float erp;
float cfm;
eVs   vs;
float force;
float velocity;
Fvector direction;
IC void set_limits(float h, float l) {high=h; low=l;}
IC void set_direction(const Fvector& v){direction.set(v);}
IC void set_direction(const float x,const float y,const float z){direction.set(x,y,z);}
IC void set_param(const float e,const float c){erp=e;cfm=c;}	
SPHAxis(){
	high=M_PI/15.f;
	low=-M_PI/15.f;;
	zero=0.f;
	//erp=ERP(world_spring/5.f,world_damping*5.f);
	//cfm=CFM(world_spring/5.f,world_damping*5.f);
	erp=0.6f;
	cfm=0.000001f;
	direction.set(0,0,1);
	vs=vs_first;
	force=5.f;
	velocity=0.f;
	}
};
vector<SPHAxis> axes;
Fvector anchor;
eVs vs_anchor;

void CreateBall();
void CreateHinge();
void CreateHinge2();
void CreateShoulder1();
void CreateShoulder2();
void CreateCarWeel();
void CreateWelding();
void CreateUniversalHinge();
void CreateFullControl();

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

	virtual void SetForceAndVelocity		(const float force,const float velocity=0.f,const int axis_num=-1);
public:
	CPHJoint(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second);
	virtual ~CPHJoint(){
						if(bActive) Deactivate();
						axes.clear();

						};
	void SetShell					(CPHShell* p)			 {pShell=p;}
	void Activate();
	void Deactivate();
};




///////////////////////////////////////////////////////////////////////
class CPHShell: public CPhysicsShell,public CPHObject {
	vector<CPHElement*> elements;
	vector<CPHJoint*>	joints;
	dSpaceID			m_space;
	bool bActivating;

list<CPHObject*>::iterator m_ident;
				
public:

	CPHShell				()							{bActive=false;
														 bActivating=false;
														 m_space=NULL;
														 p_kinematics=NULL;

											
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

	virtual void	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const s16 element);
	static void __stdcall	BonesCallback				(CBoneInstance* B);
	virtual	BoneCallbackFun* GetBonesCallback		()	{return BonesCallback ;}
	virtual	void			add_Element				(CPhysicsElement* E)		  {
		CPHElement* ph_element=(CPHElement*)E;
		ph_element->SetShell(this);
		elements.push_back(ph_element);

	};
	virtual void remove_Element(CPhysicsElement* E){
	}

	virtual void			SetAirResistance		(dReal linear=0.0002f, dReal angular=0.05f){
														vector<CPHElement*>::iterator i;
														for(i=elements.begin();i!=elements.end();i++)
																						(*i)->SetAirResistance(linear,angular);
								}

	virtual	void			add_Joint				(CPhysicsJoint* J)					{
																						joints.push_back((CPHJoint*)J);
																						dynamic_cast<CPHJoint*>(J)->SetShell(this);
																						};
	virtual void			applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	;

	virtual void			Update					()	;											

	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false);
	virtual void			Activate				();
	virtual void			Deactivate				()		;

	virtual void			setMass					(float M)									;

	virtual void			applyForce				(const Fvector& dir, float val)				{
																								if(!bActive) return;
																								(*elements.begin())->applyForce				( dir, val);
																								};
	virtual void			applyImpulse			(const Fvector& dir, float val)				{
																								if(!bActive) return;
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
		//dSpaceSetCleanup (m_space, 0);
	}

	void SetTransform(Fmatrix m);
private:
	

};

///////////////////////////////////////////////////////////////////////////////////////////////////
IC void own_axis(const Fmatrix& m,Fvector& axis){
	if(m._11==1.f) {axis.set(1.f,0.f,0.f); return;}
	float k=m._13*m._21-m._11*m._23+m._23;

	if(k==0.f){
	if(m._13==0.f) {axis.set(0.f,0.f,1.f);return;}
	float k1=m._13/(1.f-m._11);
	axis.z=_sqrt(1.f/(1.f+k1*k1));
	axis.x=axis.z*k1;
	axis.y=0.f;
	return;
	}

	float k_zy=-(m._12*m._21-m._11*m._22+m._11+m._22-1.f)/k;
	float k_xy=(m._12+m._13*k_zy)/(1.f-m._11);
	axis.y=_sqrt(1.f/(k_zy*k_zy+k_xy*k_xy+1.f));
	axis.x=axis.y*k_xy;
	axis.z=axis.y*k_zy;
	return;
}



IC void own_axis_angle(const Fmatrix& m,Fvector& axis,float& angle){
	own_axis(m,axis);
	Fvector ort1,ort2;
	if(!(axis.z==0.f&&axis.y==0.f)){
		ort1.set(0.f,-axis.z,axis.y);
		ort2.crossproduct(axis,ort1);
		}
	else{
		ort1.set(0.f,1.f,0.f);
		ort2.crossproduct(axis,ort1);
		}
	ort1.normalize();
	ort2.normalize();

	Fvector ort1_t;
	m.transform_dir(ort1_t,ort1);

	float cosinus=ort1.dotproduct(ort1_t);
	float sinus=ort2.dotproduct(ort1_t);
	angle=acosf(cosinus);
	if(sinus<0.f) angle= -angle;

}

IC void axis_angleB(const Fmatrix& m, const Fvector& axis,float& angle){

	Fvector ort1,ort2;
	if(!(axis.z==0.f&&axis.y==0.f)){
	ort1.set(0.f,-axis.z,axis.y);
	ort2.crossproduct(axis,ort1);
	}
	else{
		ort1.set(0.f,1.f,0.f);
		ort2.crossproduct(axis,ort1);
		}
	ort1.normalize();
	ort2.normalize();
	Fvector ort1_t;
	m.transform_dir(ort1_t,ort1);
	Fvector ort_r;
	float pr1,pr2;
	pr1=ort1.dotproduct(ort1_t);
	pr2=ort2.dotproduct(ort1_t);
	if(pr1==0.f&&pr2==0.f){angle=0.f;return;}
	ort_r.set(pr1*ort1.x+pr2*ort2.x,
			  pr1*ort1.y+pr2*ort2.y,
			  pr1*ort1.z+pr2*ort2.z);

	ort_r.normalize();
	float cosinus=ort1.dotproduct(ort_r);
	float sinus=ort2.dotproduct(ort_r);
	angle=acosf(cosinus);
	if(sinus<0.f) angle= -angle;
}



IC void axis_angleA(const Fmatrix& m, const Fvector& axis,float& angle){

	Fvector ort1,ort2,axis_t;
	m.transform_dir(axis_t,axis);
	if(!(axis_t.z==0.f&&axis_t.y==0.f)){
	ort1.set(0.f,-axis_t.z,axis_t.y);
	ort2.crossproduct(axis_t,ort1);
	}
	else{
		ort1.set(0.f,1.f,0.f);
		ort2.crossproduct(axis_t,ort1);
	}
	ort1.normalize();
	ort2.normalize();
	Fvector ort1_t;
	m.transform_dir(ort1_t,ort1);
	Fvector ort_r;
	float pr1,pr2;
	pr1=ort1.dotproduct(ort1_t);
	pr2=ort2.dotproduct(ort1_t);
	if(pr1==0.f&&pr2==0.f){angle=0.f;return;}
	ort_r.set(pr1*ort1.x+pr2*ort2.x,
			  pr1*ort1.y+pr2*ort2.y,
			  pr1*ort1.z+pr2*ort2.z);

	ort_r.normalize();
	float cosinus=ort1.dotproduct(ort_r);
	float sinus=ort2.dotproduct(ort_r);
	angle=acosf(cosinus);
	if(sinus<0.f) angle= -angle;
	//if(angle>M_PI) angle=angle-2.f*M_PI;
	//if(angle<-M_PI) angle=angle+2.f*M_PI;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif PHYSICS_H