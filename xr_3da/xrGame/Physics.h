#ifndef PHYSICS_H
#define PHYSICS_H

#include "dCylinder/dCylinder.h"
#include "PhysicsShell.h"
#include "PHObject.h"
#include "PHInterpolation.h"
#include "_cylinder.h"
//#define ODE_SLOW_SOLVER
///////////////////////////////////////////////////////////////////////////////
extern const dReal l_limit;
extern const dReal w_limit;
extern const dReal fixed_step;
extern const int	 dis_frames;

void BodyCutForce(dBodyID body);
void SaveContacts(dGeomID o1, dGeomID o2,dJointGroupID jointGroup);

#define ERP(k_p,k_d)		((fixed_step*(k_p)) / (((fixed_step)*(k_p)) + (k_d)))
#define CFM(k_p,k_d)		(1.f / (((fixed_step)*(k_p)) + (k_d)))
#define SPRING(cfm,erp)		((erp)/(cfm)/fixed_step)
#define DAMPING(cfm,erp)	((1.f-(erp))/(cfm))

//const dReal world_spring=24000000.f;//2400000.f;//550000.f;///1000000.f;;
//const dReal world_damping=400000.f;//erp/cfm1.1363636e-006f,0.54545456f
#ifndef  ODE_SLOW_SOLVER
const dReal world_cfm=1.1363636e-008f;
const dReal world_erp=0.74545456f;
#else
const dReal world_cfm=1.1363636e-006f;
const dReal world_erp=0.54545456f;
#endif

const dReal world_spring=SPRING(world_cfm,world_erp);
const dReal world_damping=DAMPING(world_cfm,world_erp);



class CPHMesh {
	dGeomID Geom;
public:
	dGeomID GetGeom(){return Geom;}
	void Create(dSpaceID space, dWorldID world);
	void Destroy();
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
	xr_list<CPHObject*> m_objects;
public:
	double m_frame_sum;
	dReal frame_time;
	float m_update_time;
	//CPHGun Gun;
	//CPHJeep Jeep;
	unsigned int disable_count;
	//xr_vector<CPHElement*> elements;
	CPHWorld(){disable_count=0;frame_time=0.f;m_steps_num=0;m_frame_sum=0.f;
	m_delay=0; m_previous_delay=0;m_reduce_delay=0;m_update_delay_count=0;}
	~CPHWorld(){};
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
	//CPHElement* AddElement(){
	//CPHElement* phelement=new CPHElement(Space);
	//elements.push_back(phelement);
	//return phelement;
	//}
	dGeomID GetMeshGeom() {return Mesh.GetGeom();}

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

class CPHElement;
class CPHShell;
typedef  void __stdcall ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
void __stdcall			ContactShotMark(CDB::TRI* T,dContactGeom* c)   ;

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif PHYSICS_H