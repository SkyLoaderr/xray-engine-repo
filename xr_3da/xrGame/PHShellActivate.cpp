#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHShellSplitter.h"
#include "PHFracture.h"
#include "PHJointDestroyInfo.h"
///////////////////////////////////////////////////////////////
///#pragma warning(disable:4995)
//#include "../ode/src/collision_kernel.h"
//#include <../ode/src/joint.h>
//#include <../ode/src/objects.h>

//#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHElement.h"
#include "PHShell.h"

void CPHShell::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){

	if(bActive)return;
	PresetActive();

	if(!CPHObject::is_active()) vis_update_deactivate();
	if(!disable)EnableObject(0);

	ELEMENT_I i;
	mXFORM.set(m0);
	for(i=elements.begin();elements.end() != i;++i){

		(*i)->Activate(m0,dt01, m2, disable);
	}
	bActive=true;
	bActivating=true;
	spatial_register();
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
	//mXFORM.set(m0);
	//Activate(disable);
	//Fvector lin_vel;
	//lin_vel.sub(m2.c,m0.c);
	//set_LinearVel(lin_vel);
}



void CPHShell::Activate(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable){

	if(bActive)return;
	PresetActive();	
	
	if(!CPHObject::is_active()) vis_update_deactivate();
	if(!disable)EnableObject(0);




	ELEMENT_I i;
	mXFORM.set(transform);
	for(i=elements.begin();elements.end() != i;++i){
		(*i)->Activate(transform,lin_vel, ang_vel);
	}

	spatial_register();
	bActive=true;
	bActivating=true;
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
	//mXFORM.set(transform);
	//Activate(disable);
	//set_LinearVel(lin_vel);
	//set_AngularVel(ang_vel);

}



void CPHShell::Activate(bool disable)
{ 
	if(bActive)return;
	PresetActive();
	
	if(!CPHObject::is_active()) vis_update_deactivate();
	if(!disable)EnableObject(0);




	{		
		ELEMENT_I i=elements.begin(),e=elements.end();
			 for(;i!=e;++i)(*i)->Activate(mXFORM,disable);
	}

	{
		JOINT_I i=joints.begin(),e=joints.end();
		for(;i!=e;++i) (*i)->Activate();
	}	

	spatial_register();
	bActive=true;
	bActivating=true;

}


void CPHShell::Build(bool disable/*false*/)
{
	if(bActive)return;

	PresetActive();
	bActive=true;
	bActivating=true;

	{		
		ELEMENT_I i=elements.begin(),e=elements.end();
		for(;i!=e;++i)
		{
			(*i)->build(disable);
		}
	}

	{
		JOINT_I i=joints.begin(),e=joints.end();
		for(;i!=e;++i) (*i)->Create();
	}	
	
}

void CPHShell::RunSimulation(bool place_current_forms/*true*/)
{
	if(!CPHObject::is_active()) vis_update_deactivate();
	EnableObject(0);


	dSpaceSetCleanup(m_space,0);

	{		
		ELEMENT_I i=elements.begin(),e=elements.end();
		if(place_current_forms) for(;i!=e;++i)(*i)->RunSimulation(mXFORM);
	}
	{
		JOINT_I i=joints.begin(),e=joints.end();
		for(;i!=e;++i) (*i)->RunSimulation();
	}	

	spatial_register();
}

void CPHShell::AfterSetActive()
{
	if(bActive)	return;
	PureActivate();
	bActive=true;
	ELEMENT_I i=elements.begin(),e=elements.end();
	for(;i!=e;++i)(*i)->PresetActive();

}

void CPHShell::PureActivate()
{
	if(bActive)	return;
	bActive=true;
	if(!CPHObject::is_active()) vis_update_deactivate();
	EnableObject(0);
	m_object_in_root.identity();
	spatial_register();
}

void CPHShell::PresetActive()
{
	VERIFY(!bActive);
	if(!m_space) 
	{
		m_space=dSimpleSpaceCreate(0);
		dSpaceSetCleanup(m_space,0);
	}
}



void CPHShell::Deactivate(){
	if(!bActive)
		return;
	vis_update_activate();
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
		(*i)->Deactivate();

	JOINT_I j;
	for(j=joints.begin();joints.end() != j;++j)
		(*j)->Deactivate();

		DisableObject();

	if(m_space) {
		dSpaceDestroy(m_space);
		m_space=NULL;
	}
	bActive=false;
	bActivating=false;
	spatial_unregister();
	ZeroCallbacks();
	m_traced_geoms.clear();
	CPHObject::UnsetRayMotions();
}