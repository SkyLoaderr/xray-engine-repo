#ifndef PHYSICS_H
#define PHYSICS_H


#include "dCylinder/dCylinder.h"
#include "PhysicsShell.h"
#include "PHObject.h"
#include "PHInterpolation.h"
#include "_cylinder.h"
#include "BlockAllocator.h"
#include "PhysicsCommon.h"
//#define ODE_SLOW_SOLVER
///////////////////////////////////////////////////////////////////////////////

void BodyCutForce	(dBodyID body,float l_limit,float w_limit);
void dMassSub		(dMass *a,const dMass *b)				  ;
void SaveContacts	(dGeomID o1, dGeomID o2,dJointGroupID jointGroup);


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
	
	double	m_start_time;
	u32		m_delay;
	u32		m_previous_delay;
	u32		m_reduce_delay;
	u32		m_update_delay_count;
	bool	b_world_freezed;
	static const u32 update_delay=1;
	dSpaceID Space;

	CPHMesh Mesh;
	xr_list<CPHObject*> m_objects;
public:
	u64		m_steps_num;
	double m_frame_sum;
	dReal m_frame_time;
	float m_update_time;
	//CPHGun Gun;
	//CPHJeep Jeep;
	unsigned int disable_count;
	//xr_vector<CPHElement*> elements;
	CPHWorld();
	virtual ~CPHWorld(){};
	//double Time(){return m_start_time+m_steps_num*fixed_step;}
	dSpaceID GetSpace(){return Space;};
	//	dWorldID GetWorld(){return phWorld;};
	void Create();
	xr_list <CPHObject*> ::iterator AddObject(CPHObject* object){
		m_objects.push_back(object);
		//xr_list <CPHObject*> ::iterator i= m_objects.end();
		return (--m_objects.end());
	};
	void RemoveObject(xr_list<CPHObject*> :: iterator i){
		m_objects.erase((i));
	};
	dGeomID GetMeshGeom() {return Mesh.GetGeom();}

	void Destroy();

	void FrameStep	(dReal step=0.025f);
	void Step		();
	void Freeze		();
	void UnFreeze	();
	bool IsFreezed	();
	u32		CalcNumSteps (u32 dTime);
	void Render		();


	virtual void OnFrame ();


};
/////////////////////////////////////////////////////////////////////////////

extern dWorldID phWorld;
/////////////////////////////////
extern class CBlockAllocator<dJointFeedback,128> ContactFeedBacks;

class CPHElement;
class CPHShell;

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif PHYSICS_H