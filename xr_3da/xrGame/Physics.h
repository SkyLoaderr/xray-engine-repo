#ifndef PHYSICS_H
#define PHYSICS_H


#include "dCylinder/dCylinder.h"
#include "PhysicsShell.h"
#include "PHObject.h"
#include "PHInterpolation.h"
#include "_cylinder.h"
#include "BlockAllocator.h"
//#define ODE_SLOW_SOLVER
///////////////////////////////////////////////////////////////////////////////

extern const dReal	default_l_limit;
extern const dReal	default_w_limit;
extern const dReal	default_l_scale;
extern const dReal	default_w_scale;
extern const dReal	default_disw;
extern const dReal	default_disl;
extern const dReal	fixed_step;
extern const u32	dis_frames;
extern const dReal	world_gravity;

void BodyCutForce	(dBodyID body,float l_limit,float w_limit);
void dMassSub		(dMass *a,const dMass *b)				  ;
void SaveContacts	(dGeomID o1, dGeomID o2,dJointGroupID jointGroup);

#define ERP(k_p,k_d)		((fixed_step*(k_p)) / (((fixed_step)*(k_p)) + (k_d)))
#define CFM(k_p,k_d)		(1.f / (((fixed_step)*(k_p)) + (k_d)))
#define SPRING(cfm,erp)		((erp)/(cfm)/fixed_step)
#define DAMPING(cfm,erp)	((1.f-(erp))/(cfm))

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

//const dReal world_spring=24000000.f;//2400000.f;//550000.f;///1000000.f;;
//const dReal world_damping=400000.f;//erp/cfm1.1363636e-006f,0.54545456f

#ifndef  ODE_SLOW_SOLVER
const dReal world_cfm		=1.1363636e-008f;
const dReal world_erp		=0.74545456f;
#else
const dReal world_cfm		=1.1363636e-006f;
const dReal world_erp		=0.54545456f;
#endif

const dReal world_spring	=1.0f*SPRING	(world_cfm,world_erp);
const dReal world_damping	=1.0f*DAMPING	(world_cfm,world_erp);


class CPHMesh {
	dGeomID Geom;
public:
	dGeomID GetGeom(){return Geom;}
	void Create(dSpaceID space, dWorldID world);
	void Destroy();
};

////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CPHWorld	: public pureFrame
{
	u64		m_steps_num;
	double	m_start_time;
	u32		m_delay;
	u32		m_previous_delay;
	u32		m_reduce_delay;
	u32		m_update_delay_count;
	static const u32 update_delay=1;
	dSpaceID Space;

	CPHMesh Mesh;
	xr_list<CPHObject*> m_objects;
public:
	double m_frame_sum;
	dReal m_frame_time;
	float m_update_time;
	//CPHGun Gun;
	//CPHJeep Jeep;
	unsigned int disable_count;
	//xr_vector<CPHElement*> elements;
	CPHWorld(){disable_count=0;m_frame_time=0.f;m_steps_num=0;m_frame_sum=0.f;
	m_delay=0; m_previous_delay=0;m_reduce_delay=0;m_update_delay_count=0;}
	virtual ~CPHWorld(){};
	//double Time(){return m_start_time+m_steps_num*fixed_step;}
	dSpaceID GetSpace(){return Space;};
	//	dWorldID GetWorld(){return phWorld;};
	void Create();
	xr_list <CPHObject*> ::iterator AddObject(CPHObject* object){
		m_objects.push_back(object);
		//xr_list <CPHObject*> ::iterator i= m_objects.end();
		return --(m_objects.end());
	};
	void RemoveObject(xr_list<CPHObject*> :: iterator i){
		m_objects.erase((i));
	};
	dGeomID GetMeshGeom() {return Mesh.GetGeom();}

	void Destroy();

	void Step	(dReal step=0.025f);

	void Render	();


	virtual void OnFrame ();

	//private:
	//static void FUNCCALL NearCallback(void* /*data*/, dGeomID o1, dGeomID o2);
};
/////////////////////////////////////////////////////////////////////////////

extern dWorldID phWorld;
/////////////////////////////////
extern class BlockAllocator<dJointFeedback,128> ContactFeedBacks;

class CPHElement;
class CPHShell;
typedef  void __stdcall ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
void __stdcall			ContactShotMark(CDB::TRI* T,dContactGeom* c)   ;
typedef void __stdcall PushOutCallbackFun(bool& do_colide,dContact& c);
void __stdcall PushOutCallback(bool& do_colide,dContact& c);
void __stdcall PushOutCallback1(bool& do_colide,dContact& c);
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif PHYSICS_H